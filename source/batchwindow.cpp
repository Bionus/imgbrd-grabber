#include <QSettings>
#include <QTime>
#include <QCloseEvent>
#include <QClipboard>
#include <QDebug>
#include "batchwindow.h"
#include "ui_batchwindow.h"
#include "functions.h"

#define SAMPLES 100



batchWindow::batchWindow(QWidget*) : QDialog(), ui(new Ui::batchWindow), m_imagesCount(0), m_items(0), m_images(0), m_maxSpeeds(0), m_cancel(false), m_paused(false)
{
	ui->setupUi(this);
	ui->tableWidget->resizeColumnToContents(0);
	m_currentSize = size();

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	restoreGeometry(settings.value("Batch/geometry").toByteArray());
	ui->buttonDetails->setChecked(settings.value("Batch/details", true).toBool());
	on_buttonDetails_clicked(settings.value("Batch/details", true).toBool());
	ui->comboEnd->setCurrentIndex(settings.value("Batch/end", 0).toInt());
	ui->checkRemove->setChecked(settings.value("Batch/remove", false).toBool());

	m_speeds = QMap<QString, int>();
	m_urls = QStringList();

	m_time = new QTime;
	m_time->start();
	m_start = new QTime;
}

batchWindow::~batchWindow()
{
	delete ui;
}
void batchWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	settings.setValue("Batch/geometry", saveGeometry());
	settings.setValue("Batch/details", ui->buttonDetails->isChecked());
	settings.setValue("Batch/end", ui->comboEnd->currentIndex());
	settings.setValue("Batch/remove", ui->checkRemove->isChecked());
	settings.sync();

	if (m_images < m_imagesCount)
	{
		cancel();
		emit rejected();
	}
	else
	{ clear(); }

	emit closed();
	e->accept();
}
void batchWindow::pause()
{
	m_paused = !m_paused;
	if (m_paused)
	{ ui->labelSpeed->setText(tr("En pause")); }
	else
	{ ui->labelSpeed->setText(""); }
	emit paused();
}
void batchWindow::cancel()
{ m_cancel = true; }
bool batchWindow::cancelled()
{ return m_cancel; }
void batchWindow::clear()
{
	m_cancel = false;
	m_items = 0;
	m_imagesCount = 0;
	m_maxSpeeds = 0;
	m_time->restart();
	m_start->restart();
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	ui->labelMessage->setText("");
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(100);
	ui->labelImages->setText("0/0");
	ui->labelSpeed->setText("");
	ui->cancelButton->setText(tr("Annuler"));
	qDeleteAll(m_progressBars);
	m_progressBars.clear();
	m_speeds.clear();
	m_urls.clear();
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
	id->setIcon(QIcon(":/images/colors/black.png"));
	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(QString::number(batch)));
	ui->tableWidget->setItem(m_items, 2, new QTableWidgetItem(url));
	QString unit = getUnit(&size);
	ui->tableWidget->setItem(m_items, 3, new QTableWidgetItem(size != 0 ? QLocale::system().toString(size, 'f', size < 10 ? 2 : 0)+" "+unit : ""));
	ui->tableWidget->setItem(m_items, 4, new QTableWidgetItem());
	ui->tableWidget->setItem(m_items, 5, new QTableWidgetItem("0 %"));
	/* QProgressBar *prog = new QProgressBar(this);
	prog->setTextVisible(false);
	m_progressBars.append(prog);
	ui->tableWidget->setCellWidget(m_items, 5, prog); */
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
	if (i < 0 || ui->tableWidget->item(i, 1) == NULL)
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
		ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/blue.png"));
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
	m_speeds[url] = (int)speed;
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
void batchWindow::loadedImage(QString url)
{
	m_speeds.remove(url);
	drawSpeed();

	int i = indexOf(url);
	if (i != -1)
	{
		ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/green.png"));
		ui->tableWidget->item(i, 4)->setText("");
		ui->tableWidget->item(i, 5)->setText("100 %");
	}
}
void batchWindow::errorImage(QString url)
{
	m_speeds.remove(url);

	int i = indexOf(url);
	if (i != -1)
	{
		ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/red.png"));
		ui->tableWidget->item(i, 4)->setText("");
	}
}

void batchWindow::drawSpeed()
{
	if (m_time->elapsed() < 1000)
	{ return; }
	m_time->restart();

	int speed = 0;
	foreach (int sp, m_speeds.values())
	{ speed += sp; }
	if (m_speeds.size() == m_maxSpeeds)
	{ m_mean.append(speed); }
	QString unit = getUnit(&speed)+"/s";

	int speedMean = 0, count = qMin(m_mean.count(), 60);
	if (count > 0)
	{
		for (int i = m_mean.count() - count; i < m_mean.count() - 1; i++)
		{ speedMean += m_mean[i]; }
		speedMean = (int)(speedMean/count);
	}
	QString unitMean = getUnit(&speedMean)+"/s";

	int elapsed = m_start->elapsed();
	int remaining = m_images > 0 ? (int)((elapsed * m_imagesCount) / m_images) : 0;
	QTime tElapsed, tRemaining;
	tElapsed = tElapsed.addMSecs(elapsed);
	tRemaining = tRemaining.addMSecs(remaining);
	QString fElapsed = elapsed > 3600000 ? tr("h 'h' m 'm' s 's'") : (elapsed > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));
	QString fRemaining = remaining > 3600000 ? tr("h 'h' m 'm' s 's'") : (remaining > 60000 ? tr("m 'm' s 's'") : tr("s 's'"));

	ui->labelSpeed->setText(QLocale::system().toString((float)speed, 'f', speed < 10 ? 2 : 0)+" "+unit);
	ui->labelSpeed->setToolTip(tr("<b>Vitesse moyenne :</b> %1 %2<br/><br/><b>Temps écoulé :</b> %3<br/><b>Temps restant :</b> %4").arg(QLocale::system().toString((float)speedMean, 'f', speedMean < 10 ? 2 : 0), unitMean, tElapsed.toString(fElapsed), tRemaining.toString(fRemaining)));
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
void batchWindow::setValue(int value)
{
	m_value = value;
	ui->progressBar->setValue(m_value);
	if (ui->progressBar->maximum() <= m_value)
	{ ui->cancelButton->setText(tr("Fermer")); }
}
void batchWindow::setLittleValue(int)
{ /*ui->progressBar->setValue(m_value + value);*/ }
void batchWindow::setMaximum(int value)
{ ui->progressBar->setMaximum(value); }
void batchWindow::setImagesCount(int value)
{
	m_imagesCount = value;
	ui->labelImages->setText(QString("0/%2").arg(m_imagesCount));
	ui->progressBar->setMaximum(value);
}
void batchWindow::setImages(int value)
{
	m_images = value;
	ui->labelImages->setText(QString("%1/%2").arg(m_images).arg(m_imagesCount));
	ui->progressBar->setValue(value);
}

int batchWindow::value()		{ return m_value;						}
int batchWindow::maximum()		{ return ui->progressBar->maximum();	}
int batchWindow::images()		{ return m_images;						}
int batchWindow::count()		{ return m_imagesCount;					}
int batchWindow::endAction()	{ return ui->comboEnd->currentIndex();	}
bool batchWindow::endRemove()	{ return ui->checkRemove->isChecked();	}
bool batchWindow::isPaused()	{ return m_paused;						}
