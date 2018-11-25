#include "batch/batch-window.h"
#include <QClipboard>
#include <QCloseEvent>
#include <QSettings>
#ifdef Q_OS_WIN
	#include <QWinTaskbarButton>
	#include <QWinTaskbarProgress>
#endif
#include <ui_batch-window.h>
#include "functions.h"
#include "loader/downloadable.h"

#define SPEED_SMOOTHING_IMAGE 0.3
#define SPEED_SMOOTHING_AVERAGE 0.3


BatchWindow::BatchWindow(QSettings *settings, QWidget *parent)
	: QDialog(parent), ui(new Ui::BatchWindow), m_settings(settings), m_imagesCount(0), m_items(0), m_images(0), m_maxSpeeds(0), m_lastDownloading(0), m_mean(SPEED_SMOOTHING_AVERAGE), m_cancel(false), m_paused(false)
{
	ui->setupUi(this);
	ui->tableWidget->resizeColumnToContents(0);
	m_currentSize = size();

	restoreGeometry(m_settings->value("Batch/geometry").toByteArray());
	ui->buttonDetails->setChecked(m_settings->value("Batch/details", true).toBool());
	on_buttonDetails_clicked(m_settings->value("Batch/details", true).toBool());
	ui->comboEnd->setCurrentIndex(m_settings->value("Batch/end", 0).toInt());
	ui->checkRemove->setChecked(m_settings->value("Batch/remove", false).toBool());
	ui->checkScrollToDownload->setChecked(m_settings->value("Batch/scrollToDownload", true).toBool());

	m_time = new QTime;
	m_time->start();
	m_start = new QTime;

	#ifdef Q_OS_WIN
		m_taskBarButton = new QWinTaskbarButton(parent);
		m_taskBarButton->setWindow(parent->windowHandle());

		m_taskBarProgress = m_taskBarButton->progress();
		m_taskBarProgress->setVisible(false);
		m_taskBarProgress->setMinimum(0);
		m_taskBarProgress->setMaximum(0);
		m_taskBarProgress->setValue(0);
	#endif

	// Allow dialog minimization
	setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
}

BatchWindow::~BatchWindow()
{
	delete ui;

	#ifdef Q_OS_WIN
		m_taskBarButton->deleteLater();
		m_taskBarProgress->deleteLater();
	#endif
}
void BatchWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Batch/geometry", saveGeometry());
	m_settings->setValue("Batch/details", ui->buttonDetails->isChecked());
	m_settings->setValue("Batch/end", ui->comboEnd->currentIndex());
	m_settings->setValue("Batch/remove", ui->checkRemove->isChecked());
	m_settings->setValue("Batch/scrollToDownload", ui->checkScrollToDownload->isChecked());
	m_settings->sync();

	if (m_images < m_imagesCount || m_imagesCount == -1)
	{
		cancel();
		emit rejected();
	}
	else
	{ clear(); }

	#ifdef Q_OS_WIN
		m_taskBarProgress->setVisible(false);
	#endif

	emit closed();
	e->accept();
}
void BatchWindow::pause()
{
	m_paused = !m_paused;
	ui->labelSpeed->setText(m_paused ? tr("Paused") : QString());
	ui->buttonPause->setText(m_paused ? tr("Resume") : tr("Pause"));

	// Reset download speeds
	m_mean.clear();
	for (auto it = m_speeds.begin(); it != m_speeds.end(); ++it)
		it.value().clear();

	#ifdef Q_OS_WIN
		m_taskBarProgress->setPaused(m_paused);
	#endif

	emit paused();
}
void BatchWindow::skip()
{
	emit skipped();
}
void BatchWindow::cancel()
{
	m_cancel = true;

	#ifdef Q_OS_WIN
		m_taskBarProgress->setVisible(false);
	#endif
}
bool BatchWindow::cancelled()
{ return m_cancel; }

void BatchWindow::clear()
{
	m_cancel = false;
	m_paused = false;

	m_imagesCount = -1;
	m_images = 0;

	ui->progressTotal->setValue(0);
	ui->progressTotal->setMaximum(100);
	ui->labelImages->setText(QStringLiteral("0/0"));
	ui->cancelButton->setText(tr("Cancel"));

	#ifdef Q_OS_WIN
		m_taskBarProgress->setMinimum(0);
		m_taskBarProgress->setMaximum(0);
		m_taskBarProgress->setValue(0);
		m_taskBarProgress->setVisible(true);
	#endif

	clearImages();
}
void BatchWindow::clearImages()
{
	m_items = 0;
	m_maxSpeeds = 0;
	m_lastDownloading = 0;

	m_time->restart();
	m_start->restart();

	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	ui->labelMessage->setText(QString());
	ui->progressCurrent->setValue(0);
	ui->progressCurrent->setMaximum(100);
	ui->labelSpeed->setText(QString());

	qDeleteAll(m_progressBars);
	m_progressBars.clear();
	m_speeds.clear();
	m_urls.clear();
	m_mean.clear();
}

void BatchWindow::copyToClipboard()
{
	QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
	int count = selected.size();
	QStringList urls = QStringList();
	if (count < 1)
	{
		count = ui->tableWidget->rowCount();
		urls.reserve(count);
		for (int i = 0; i < count; i++)
		{ urls.append(ui->tableWidget->item(i, 2)->text()); }
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			if (selected.at(i)->icon().isNull())
			{ urls.append(selected.at(i)->text()); }
		}
	}
	qApp->clipboard()->setText(urls.join('\n'));
}

void BatchWindow::setCount(int cnt)
{ ui->tableWidget->setRowCount(cnt); }
void BatchWindow::addImage(const QUrl &url, int batch, double size)
{
	m_urls.insert(url, m_items);

	static QIcon pendingIcon(":/images/status/pending.png");
	QTableWidgetItem *id = new QTableWidgetItem(QString::number(m_items + 1));
	id->setIcon(pendingIcon);

	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(QString::number(batch)));
	ui->tableWidget->setItem(m_items, 2, new QTableWidgetItem(url.toString()));
	const QString unit = getUnit(&size);
	ui->tableWidget->setItem(m_items, 3, new QTableWidgetItem(size > 0 ? QLocale::system().toString(size, 'f', size < 10 ? 2 : 0) + " " + unit : QString()));
	ui->tableWidget->setItem(m_items, 4, new QTableWidgetItem());
	ui->tableWidget->setItem(m_items, 5, new QTableWidgetItem(QStringLiteral("0 %")));

	/* auto *progressBar = new QProgressBar(this);
	progressBar->setTextVisible(false);
	m_progressBars.append(progressBar);
	ui->tableWidget->setCellWidget(m_items, 5, progressBar); */

	m_items++;
}
void BatchWindow::updateColumns()
{
	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setSectionResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(0, 60);
	headerView->resizeSection(1, 40);
	headerView->resizeSection(2, 80);
	headerView->setSectionResizeMode(2, QHeaderView::Stretch);
	headerView->resizeSection(3, 80);
	headerView->resizeSection(4, 80);
	headerView->resizeSection(5, 80);
	ui->tableWidget->resizeColumnToContents(0);
	ui->tableWidget->repaint();
}
int BatchWindow::indexOf(const QUrl &url)
{
	const int i = m_urls[url];
	if (i < 0 || ui->tableWidget->item(i, 1) == nullptr)
		return -1;
	return i;
}
int BatchWindow::batch(const QUrl &url)
{
	const int i = indexOf(url);
	if (i == -1)
		return -1;
	return ui->tableWidget->item(i, 1)->text().toInt();
}
void BatchWindow::loadingImage(const QUrl &url)
{
	if (m_start->isNull())
		m_start->start();
	m_speeds.insert(url, ExponentialMovingAverage(SPEED_SMOOTHING_IMAGE));
	if (m_speeds.size() > m_maxSpeeds)
		m_maxSpeeds = m_speeds.size();

	const int i = indexOf(url);
	if (i != -1)
	{
		static QIcon downloadingIcon(":/images/status/downloading.png");
		ui->tableWidget->item(i, 0)->setIcon(downloadingIcon);
		scrollTo(i);
	}
}
void BatchWindow::scrollTo(int i)
{
	// Go to downloading image
	if (ui->checkScrollToDownload->isChecked() && i >= m_lastDownloading)
	{
		ui->tableWidget->scrollToItem(ui->tableWidget->item(i, 0));
		m_lastDownloading = i;
	}
}
void BatchWindow::imageUrlChanged(const QUrl &before, const QUrl &after)
{
	const int i = indexOf(before);
	if (i != -1)
	{
		m_urls.remove(before);
		m_urls.insert(after, i);
		ui->tableWidget->item(i, 2)->setText(after.toString());
		ui->tableWidget->item(i, 3)->setText(QString());
		ui->tableWidget->item(i, 4)->setText(QString());
		ui->tableWidget->item(i, 5)->setText("0 %");
	}
}
void BatchWindow::statusImage(const QUrl &url, int percent)
{
	const int i = indexOf(url);
	if (i != -1)
		ui->tableWidget->item(i, 5)->setText(QString::number(percent) + " %");
}
void BatchWindow::speedImage(const QUrl &url, double speed)
{
	m_speeds[url].addValue(speed);

	double average = m_speeds[url].average();
	const QString unit = getUnit(&average) + "/s";

	int i = indexOf(url);
	if (i != -1)
		ui->tableWidget->item(i, 4)->setText(QLocale::system().toString(average, 'f', average < 10 ? 2 : 0) + " " + unit);

	drawSpeed();
}
void BatchWindow::sizeImage(const QUrl &url, double size)
{
	int i = indexOf(url);
	if (i != -1)
	{
		const QString unit = getUnit(&size);
		const QString label = size > 0
			? QLocale::system().toString(size, 'f', size < 10 ? 2 : 0) + " " + unit
			: QString();
		ui->tableWidget->item(i, 3)->setText(label);
	}
}
void BatchWindow::loadedImage(const QUrl &url, Downloadable::SaveResult result)
{
	static QIcon ignoredIcon(":/images/status/ignored.png");
	static QIcon errorIcon(":/images/status/error.png");
	static QIcon okIcon(":/images/status/ok.png");
	static QIcon unknownIcon(":/images/status/unknown.png");

	// Update speed
	m_speeds.remove(url);
	drawSpeed();

	// Update table
	const int i = indexOf(url);
	if (i != -1)
	{
		scrollTo(i);
		ui->tableWidget->item(i, 4)->setText(QString());
		ui->tableWidget->item(i, 5)->setText(QString());

		switch (result)
		{
			case Downloadable::SaveResult::AlreadyExistsDisk:
			case Downloadable::SaveResult::AlreadyExistsMd5:
			case Downloadable::SaveResult::Blacklisted:
				ui->tableWidget->item(i, 0)->setIcon(ignoredIcon);
				break;

			case Downloadable::SaveResult::Error:
			case Downloadable::SaveResult::NotFound:
			case Downloadable::SaveResult::NetworkError:
				ui->tableWidget->item(i, 0)->setIcon(errorIcon);
				break;

			case Downloadable::SaveResult::Moved:
			case Downloadable::SaveResult::Copied:
			case Downloadable::SaveResult::Linked:
			case Downloadable::SaveResult::Saved:
				ui->tableWidget->item(i, 0)->setIcon(okIcon);
				ui->tableWidget->item(i, 5)->setText("100 %");
				break;

			default:
				ui->tableWidget->item(i, 0)->setIcon(unknownIcon);
				break;
		}
	}
}

void BatchWindow::drawSpeed()
{
	if (m_time->elapsed() < 1000)
	{ return; }
	m_time->restart();

	double speed = 0;
	for (auto sp = m_speeds.constBegin(); sp != m_speeds.constEnd(); ++sp)
	{ speed += sp.value().average(); }
	if (m_speeds.size() == m_maxSpeeds)
	{ m_mean.addValue(speed); }
	const QString unit = getUnit(&speed) + "/s";

	double speedMean = m_mean.average();
	const QString unitMean = getUnit(&speedMean) + "/s";

	const int elapsed = m_start->elapsed();
	const int remaining = m_images > 0 ? (elapsed * m_imagesCount) / m_images : 0;
	QTime tElapsed, tRemaining;
	tElapsed = tElapsed.addMSecs(elapsed);
	tRemaining = tRemaining.addMSecs(remaining);
	const QString fElapsed = elapsed > 3600000 ? tr("h 'h' m 'm' s 's'") : (elapsed > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));
	const QString fRemaining = remaining > 3600000 ? tr("h 'h' m 'm' s 's'") : (remaining > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));

	ui->labelSpeed->setText(QLocale::system().toString(speed, 'f', speed < 10 ? 2 : 0) + " " + unit);
	ui->labelSpeed->setToolTip(tr("<b>Average speed:</b> %1 %2<br/><br/><b>Elapsed time:</b> %3<br/><b>Remaining time:</b> %4").arg(QLocale::system().toString(speedMean, 'f', speedMean < 10 ? 2 : 0), unitMean, tElapsed.toString(fElapsed), tRemaining.toString(fRemaining)));
}

void BatchWindow::on_buttonDetails_clicked(bool visible)
{
	if (ui->details->isHidden() || visible)
	{
		ui->details->show();
		resize(m_currentSize);
	}
	else
	{
		ui->details->hide();
		m_currentSize = size();
		resize(QSize(300, 0));
	}
}

void BatchWindow::setText(const QString &text)
{ ui->labelMessage->setText(text); }

void BatchWindow::setCurrentValue(int val)
{ ui->progressCurrent->setValue(val); }
void BatchWindow::setCurrentMax(int max)
{ ui->progressCurrent->setMaximum(max); }

void BatchWindow::setTotalValue(int val)
{
	m_images = val;
	ui->labelImages->setText(QStringLiteral("%1/%2").arg(m_images).arg(m_imagesCount));
	ui->progressTotal->setValue(val);

	if (val >= m_imagesCount)
	{ ui->cancelButton->setText(tr("Close")); }

	#ifdef Q_OS_WIN
		m_taskBarProgress->setValue(val);
	#endif
}
void BatchWindow::setTotalMax(int max)
{
	m_imagesCount = max;
	ui->labelImages->setText(QStringLiteral("0/%2").arg(max));
	ui->progressTotal->setMaximum(max);

	#ifdef Q_OS_WIN
		m_taskBarProgress->setMaximum(max);
	#endif
}

int BatchWindow::currentValue() const
{ return ui->progressCurrent->value(); }
int BatchWindow::currentMax() const
{ return ui->progressCurrent->maximum(); }
int BatchWindow::totalValue() const
{ return ui->progressTotal->value(); }
int BatchWindow::totalMax() const
{ return ui->progressTotal->maximum(); }

int BatchWindow::endAction() { return ui->comboEnd->currentIndex(); }
bool BatchWindow::endRemove() { return ui->checkRemove->isChecked(); }
bool BatchWindow::isPaused() { return m_paused; }
