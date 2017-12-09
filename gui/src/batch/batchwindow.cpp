#include <QSettings>
#include <QTime>
#include <QCloseEvent>
#include <QClipboard>
#include "batch/batchwindow.h"
#include "ui_batchwindow.h"
#include "functions.h"



batchWindow::batchWindow(QSettings *settings, QWidget *parent)
	: QDialog(parent), ui(new Ui::batchWindow), m_settings(settings), m_imagesCount(0), m_items(0), m_images(0), m_maxSpeeds(0), m_lastDownloading(0), m_cancel(false), m_paused(false)
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

	m_speeds = QMap<QString, int>();
	m_urls = QStringList();

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

batchWindow::~batchWindow()
{
	delete ui;

	#ifdef Q_OS_WIN
		m_taskBarButton->deleteLater();
		m_taskBarProgress->deleteLater();
	#endif
}
void batchWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Batch/geometry", saveGeometry());
	m_settings->setValue("Batch/details", ui->buttonDetails->isChecked());
	m_settings->setValue("Batch/end", ui->comboEnd->currentIndex());
	m_settings->setValue("Batch/remove", ui->checkRemove->isChecked());
	m_settings->setValue("Batch/scrollToDownload", ui->checkScrollToDownload->isChecked());
	m_settings->sync();

	if (m_images < m_imagesCount)
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
void batchWindow::pause()
{
	m_paused = !m_paused;
	ui->labelSpeed->setText(m_paused ? tr("Paused") : "");
	ui->buttonPause->setText(m_paused ? tr("Resume") : tr("Pause"));

	#ifdef Q_OS_WIN
		m_taskBarProgress->setPaused(m_paused);
	#endif

	emit paused();
}
void batchWindow::skip()
{
	emit skipped();
}
void batchWindow::cancel()
{
	m_cancel = true;

	#ifdef Q_OS_WIN
		m_taskBarProgress->setVisible(false);
	#endif
}
bool batchWindow::cancelled()
{ return m_cancel; }

void batchWindow::clear()
{
	m_cancel = false;
	m_paused = false;

	m_imagesCount = 0;
	m_images = 0;

	ui->progressTotal->setValue(0);
	ui->progressTotal->setMaximum(100);
	ui->labelImages->setText("0/0");
	ui->cancelButton->setText(tr("Cancel"));

	#ifdef Q_OS_WIN
		m_taskBarProgress->setMinimum(0);
		m_taskBarProgress->setMaximum(0);
		m_taskBarProgress->setValue(0);
		m_taskBarProgress->setVisible(true);
	#endif

	clearImages();
}
void batchWindow::clearImages()
{
	m_items = 0;
	m_maxSpeeds = 0;
	m_lastDownloading = 0;

	m_time->restart();
	m_start->restart();

	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	ui->labelMessage->setText("");
	ui->progressCurrent->setValue(0);
	ui->progressCurrent->setMaximum(100);
	ui->labelSpeed->setText("");

	qDeleteAll(m_progressBars);
	m_progressBars.clear();
	m_speeds.clear();
	m_urls.clear();
	m_mean.clear();
}

void batchWindow::copyToClipboard()
{
	QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
	int count = selected.size();
	QStringList urls = QStringList();
	if (count < 1)
	{
		count = ui->tableWidget->rowCount();
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
	qApp->clipboard()->setText(urls.join("\n"));
}

void batchWindow::setCount(int cnt)
{ ui->tableWidget->setRowCount(cnt); }
void batchWindow::addImage(QString url, int batch, float size)
{
	m_urls.append(url);
	QTableWidgetItem *id = new QTableWidgetItem(QString::number(m_items+1));
	id->setIcon(QIcon(":/images/status/pending.png"));
	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(QString::number(batch)));
	ui->tableWidget->setItem(m_items, 2, new QTableWidgetItem(url));
	QString unit = getUnit(&size);
	ui->tableWidget->setItem(m_items, 3, new QTableWidgetItem(size != 0 ? QLocale::system().toString(size, 'f', size < 10 ? 2 : 0)+" "+unit : ""));
	ui->tableWidget->setItem(m_items, 4, new QTableWidgetItem());
	ui->tableWidget->setItem(m_items, 5, new QTableWidgetItem("0 %"));

	/* auto *progressBar = new QProgressBar(this);
	progressBar->setTextVisible(false);
	m_progressBars.append(progressBar);
	ui->tableWidget->setCellWidget(m_items, 5, progressBar); */

	m_items++;
}
void batchWindow::updateColumns()
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
int batchWindow::indexOf(QString url)
{
	int i = m_urls.indexOf(url);
	if (i < 0 || ui->tableWidget->item(i, 1) == Q_NULLPTR)
		return -1;
	return i;
}
int batchWindow::batch(QString url)
{
	int i = indexOf(url);
	if (i == -1)
		return -1;
	return ui->tableWidget->item(i, 1)->text().toInt();
}
void batchWindow::loadingImage(QString url)
{
	if (m_start->isNull())
		m_start->start();
	m_speeds.insert(url, 0);
	if (m_speeds.size() > m_maxSpeeds)
		m_maxSpeeds = m_speeds.size();

	int i = indexOf(url);
	if (i != -1)
	{
		ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/status/downloading.png"));

		// Go to downloading image
		if (ui->checkScrollToDownload->isChecked() && i >= m_lastDownloading)
		{
			ui->tableWidget->scrollToItem(ui->tableWidget->item(i, 0));
			m_lastDownloading = i;
		}
	}
}
void batchWindow::imageUrlChanged(QString before, QString after)
{
	int i = indexOf(before);
	if (i != -1)
	{
		m_urls[i] = after;
		ui->tableWidget->item(i, 2)->setText(after);
		ui->tableWidget->item(i, 3)->setText("");
		ui->tableWidget->item(i, 4)->setText("");
		ui->tableWidget->item(i, 5)->setText("0 %");
	}
}
void batchWindow::statusImage(QString url, int percent)
{
	int i = indexOf(url);
	if (i != -1)
		ui->tableWidget->item(i, 5)->setText(QString::number(percent)+" %");
}
void batchWindow::speedImage(QString url, float speed)
{
	m_speeds[url] = static_cast<int>(speed);
	QString unit = getUnit(&speed)+"/s";

	int i = indexOf(url);
	if (i != -1)
		ui->tableWidget->item(i, 4)->setText(QLocale::system().toString(speed, 'f', speed < 10 ? 2 : 0)+" "+unit);

	drawSpeed();
}
void batchWindow::sizeImage(QString url, float size)
{
	int i = indexOf(url);
	if (i != -1)
	{
		QString unit = getUnit(&size);
		ui->tableWidget->setItem(i, 3, new QTableWidgetItem(size != 0 ? QLocale::system().toString(size, 'f', size < 10 ? 2 : 0)+" "+unit : ""));
	}
}
void batchWindow::loadedImage(QString url, Downloadable::SaveResult result)
{
	// Update speed
	m_speeds.remove(url);
	drawSpeed();

	// Update table
	int i = indexOf(url);
	if (i != -1)
	{
		ui->tableWidget->item(i, 4)->setText("");
		ui->tableWidget->item(i, 5)->setText("");

		switch (result)
		{
			case Downloadable::SaveResult::AlreadyExists:
			case Downloadable::SaveResult::Ignored:
				ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/status/ignored.png"));
				break;

			case Downloadable::SaveResult::Error:
			case Downloadable::SaveResult::NotFound:
			case Downloadable::SaveResult::NetworkError:
				ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/status/error.png"));
				break;

			case Downloadable::SaveResult::Moved:
			case Downloadable::SaveResult::Copied:
			case Downloadable::SaveResult::Saved:
				ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/status/ok.png"));
				ui->tableWidget->item(i, 5)->setText("100 %");
				break;

			default:
				ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/status/unknown.png"));
				break;
		}
	}
}

void batchWindow::drawSpeed()
{
	if (m_time->elapsed() < 1000)
	{ return; }
	m_time->restart();

	float speed = 0;
	for (int sp : m_speeds.values())
	{ speed += sp; }
	if (m_speeds.size() == m_maxSpeeds)
	{ m_mean.append(speed); }
	QString unit = getUnit(&speed)+"/s";

	float speedMean = 0, count = qMin(m_mean.count(), 60);
	if (count > 0)
	{
		for (int i = m_mean.count() - count; i < m_mean.count() - 1; i++)
		{ speedMean += m_mean[i]; }
		speedMean = static_cast<int>(speedMean/count);
	}
	QString unitMean = getUnit(&speedMean)+"/s";

	int elapsed = m_start->elapsed();
	int remaining = m_images > 0 ? static_cast<int>((elapsed * m_imagesCount) / m_images) : 0;
	QTime tElapsed, tRemaining;
	tElapsed = tElapsed.addMSecs(elapsed);
	tRemaining = tRemaining.addMSecs(remaining);
	QString fElapsed = elapsed > 3600000 ? tr("h 'h' m 'm' s 's'") : (elapsed > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));
	QString fRemaining = remaining > 3600000 ? tr("h 'h' m 'm' s 's'") : (remaining > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));

	ui->labelSpeed->setText(QLocale::system().toString(static_cast<float>(speed), 'f', speed < 10 ? 2 : 0)+" "+unit);
	ui->labelSpeed->setToolTip(tr("<b>Average speed:</b> %1 %2<br/><br/><b>Elapsed time:</b> %3<br/><b>Remaining time:</b> %4").arg(QLocale::system().toString(static_cast<float>(speedMean), 'f', speedMean < 10 ? 2 : 0), unitMean, tElapsed.toString(fElapsed), tRemaining.toString(fRemaining)));
}

void batchWindow::on_buttonDetails_clicked(bool visible)
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

void batchWindow::setText(QString text)
{ ui->labelMessage->setText(text); }

void batchWindow::setCurrentValue(int val)
{ ui->progressCurrent->setValue(val); }
void batchWindow::setCurrentMax(int max)
{ ui->progressCurrent->setMaximum(max); }

void batchWindow::setTotalValue(int val)
{
	m_images = val;
	ui->labelImages->setText(QString("%1/%2").arg(m_images).arg(m_imagesCount));
	ui->progressTotal->setValue(val);

	if (val >= m_imagesCount)
	{ ui->cancelButton->setText(tr("Close")); }

	#ifdef Q_OS_WIN
		m_taskBarProgress->setValue(val);
	#endif
}
void batchWindow::setTotalMax(int max)
{
	m_imagesCount = max;
	ui->labelImages->setText(QString("0/%2").arg(max));
	ui->progressTotal->setMaximum(max);

	#ifdef Q_OS_WIN
		m_taskBarProgress->setMaximum(max);
	#endif
}

int batchWindow::currentValue() const
{ return ui->progressCurrent->value(); }
int batchWindow::currentMax() const
{ return ui->progressCurrent->maximum(); }
int batchWindow::totalValue() const
{ return ui->progressTotal->value(); }
int batchWindow::totalMax() const
{ return ui->progressTotal->maximum(); }

int batchWindow::endAction()	{ return ui->comboEnd->currentIndex();		}
bool batchWindow::endRemove()	{ return ui->checkRemove->isChecked();		}
bool batchWindow::isPaused()	{ return m_paused;							}
