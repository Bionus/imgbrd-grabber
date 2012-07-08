#include <QSettings>
#include <QTime>
#include <QCloseEvent>
#include <QClipboard>
#include "batchwindow.h"
#include "ui_batchwindow.h"
#include "functions.h"

#define SAMPLES 100



batchWindow::batchWindow(QWidget *parent) : QDialog(), ui(new Ui::batchWindow), m_items(0), m_images(0), m_cancel(false)
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

	m_time = new QTime;
	m_time->start();
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
void batchWindow::cancel()
{ m_cancel = true; }
bool batchWindow::cancelled()
{ return m_cancel; }
void batchWindow::clear()
{
	m_cancel = false;
	m_items = 0;
	m_imagesCount = 0;
	m_time->restart();
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
	QTableWidgetItem *id = new QTableWidgetItem(QString::number(m_items+1));
	id->setIcon(QIcon(":/images/colors/black.png"));
	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(QString::number(batch)));
	ui->tableWidget->setItem(m_items, 2, new QTableWidgetItem(url));
	QString sze = "";
	if (size != 0)
	{
		QString unit = "o";
		if (size >= 1024)
		{
			size /= 1024;
			if (size >= 1024)
			{
				size /= 1024;
				unit = "Mio";
			}
			else
			{ unit = "Kio"; }
		}
		sze = QLocale::system().toString(size, 'f', size < 10 ? 2 : 0)+" "+unit;
	}
	ui->tableWidget->setItem(m_items, 3, new QTableWidgetItem(sze));
	ui->tableWidget->setItem(m_items, 4, new QTableWidgetItem());
	QProgressBar *prog = new QProgressBar();
	prog->setTextVisible(false);
	m_progressBars.append(prog);
	ui->tableWidget->setCellWidget(m_items, 5, prog);
	ui->tableWidget->resizeColumnToContents(0);
	QHeaderView *headerView = ui->tableWidget->horizontalHeader();
	headerView->setResizeMode(QHeaderView::Interactive);
	headerView->resizeSection(1, 40);
	headerView->setResizeMode(2, QHeaderView::Stretch);
	headerView->resizeSection(2, 80);
	headerView->resizeSection(3, 80);
	headerView->resizeSection(4, 80);
	m_items++;
}
int batchWindow::batch(QString url)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ return ui->tableWidget->item(i, 1)->text().toInt() - 1; }
	}
	return -1;
}
void batchWindow::loadingImage(QString url)
{
	m_speeds.insert(url, 0);
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/blue.png")); }
	}
}
void batchWindow::statusImage(QString url, int percent)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ m_progressBars[i]->setValue(percent); }
	}
}
void batchWindow::speedImage(QString url, float speed)
{
	QString unit = "o/s";
	if (speed >= 1024)
	{
		speed /= 1024;
		if (speed >= 1024)
		{
			speed /= 1024;
			unit = "Mio/s";
		}
		else
		{ unit = "Kio/s"; }
	}

	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ ui->tableWidget->item(i, 4)->setText(QLocale::system().toString(speed, 'f', speed < 10 ? 2 : 0)+" "+unit); }
	}

	drawSpeed();
}
void batchWindow::sizeImage(QString url, float size)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{
			QString unit = "o";
			if (size >= 1024)
			{
				size /= 1024;
				if (size >= 1024)
				{
					size /= 1024;
					unit = "Mio";
				}
				else
				{ unit = "Kio"; }
			}
			ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QLocale::system().toString(size, 'f', size < 10 ? 2 : 0)+" "+unit));
			return;
		}
	}
}
void batchWindow::loadedImage(QString url)
{
	m_speeds.remove(url);
	drawSpeed();
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{
			ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/green.png"));
			ui->tableWidget->item(i, 4)->setText("");
			m_progressBars[i]->setValue(100);
			return;
		}
	}
}
void batchWindow::errorImage(QString url)
{
	m_speeds.remove(url);
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{
			ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/red.png"));
			ui->tableWidget->item(i, 4)->setText("");
			return;
		}
	}
}

void batchWindow::drawSpeed()
{
	if (m_time->elapsed() < 1000)
	{ return; }

	m_time->restart();

	float speed = 0;
	foreach (int sp, m_speeds.values())
	{ speed += sp; }
	speed /= m_speeds.count();

	QString unit = "o/s";
	if (speed >= 1024)
	{
		speed /= 1024;
		if (speed >= 1024)
		{
			speed /= 1024;
			unit = "Mio/s";
		}
		else
		{ unit = "Kio/s"; }
	}
	ui->labelSpeed->setText(QLocale::system().toString(speed, 'f', speed < 10 ? 2 : 0)+" "+unit);
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
void batchWindow::setLittleValue(int value)
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
	ui->labelImages->setText(QString("%1/%2").arg(value).arg(m_imagesCount));
	ui->progressBar->setValue(value);
}

int batchWindow::value()		{ return m_value;						}
int batchWindow::maximum()		{ return ui->progressBar->maximum();	}
int batchWindow::images()		{ return m_images;						}
int batchWindow::count()		{ return m_imagesCount;					}
int batchWindow::endAction()	{ return ui->comboEnd->currentIndex();	}
bool batchWindow::endRemove()	{ return ui->checkRemove->isChecked();	}
