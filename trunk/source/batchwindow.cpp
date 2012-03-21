#include "batchwindow.h"
#include "functions.h"
#include "ui_batchwindow.h"



batchWindow::batchWindow(QWidget *parent) : QDialog(parent), ui(new Ui::batchWindow), m_items(0), m_images(0)
{
	ui->setupUi(this);
	ui->details->hide();
	ui->tableWidget->resizeColumnToContents(0);
	resize(QSize(300, 0));
	m_currentSize = QSize(300, 225);
}

batchWindow::~batchWindow()
{
    delete ui;
}
void batchWindow::closeEvent(QCloseEvent *e)
{
	if (ui->progressBar->value() != ui->progressBar->maximum())
	{ emit rejected(); }
	emit closed();
	e->accept();
}
void batchWindow::clear()
{
	m_items = 0;
	m_imagesCount = 0;
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	ui->labelMessage->setText("");
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(100);
	ui->labelImages->setText("0/0");
	qDeleteAll(m_progressBars);
	m_progressBars.clear();
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

void batchWindow::addImage(QString url, int batch, int size)
{
	ui->tableWidget->setRowCount(m_items+1);
	QTableWidgetItem *id = new QTableWidgetItem(QString::number(m_items+1));
	id->setIcon(QIcon(":/images/colors/black.png"));
	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(QString::number(batch)));
	ui->tableWidget->setItem(m_items, 2, new QTableWidgetItem(url));
	QString unit = "o/s";
	if (size >= 1024)
	{
		size /= 1024;
		if (size >= 1024)
		{
			size /= 1024;
			unit = "mo";
		}
		else
		{ unit = "ko"; }
	}
	ui->tableWidget->setItem(m_items, 3, new QTableWidgetItem(QString::number(round(size, 2))+" "+unit));
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
void batchWindow::speedImage(QString url, int speed)
{
	m_currentSpeed += speed - m_speeds[url];
	m_speeds[url] = speed;
	QString unit = "o/s";
	if (speed >= 1024)
	{
		speed /= 1024;
		if (speed >= 1024)
		{
			speed /= 1024;
			unit = "mo/s";
		}
		else
		{ unit = "ko/s"; }
	}
	drawSpeed();
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ ui->tableWidget->item(i, 4)->setText(QString::number(round(speed, 2))+" "+unit); }
	}
}
void batchWindow::loadedImage(QString url)
{
	m_speeds.remove(url);
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{
			ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/green.png"));
			ui->tableWidget->item(i, 4)->setText("");
			m_progressBars[i]->setValue(100);
		}
	}
}
void batchWindow::errorImage(QString url)
{
	m_speeds.remove(url);
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 2)->text() == url)
		{ ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/red.png")); }
	}
}

void batchWindow::setSpeed(int speed)
{
	m_currentSpeed = speed;
	drawSpeed();
}
void batchWindow::drawSpeed()
{
	QString unit = "o/s";
	int speed = m_currentSpeed;
	if (speed >= 1024)
	{
		speed /= 1024;
		if (speed >= 1024)
		{
			speed /= 1024;
			unit = "mo/s";
		}
		else
		{ unit = "ko/s"; }
	}
	ui->labelSpeed->setText(QString::number(round(speed, 2))+" "+unit);
}

void batchWindow::on_buttonDetails_clicked()
{
	if (ui->details->isHidden())
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

void batchWindow::setText(QString text)		{ ui->labelMessage->setText(text);						}
void batchWindow::setValue(int value)		{ m_value = value; ui->progressBar->setValue(m_value);	}
void batchWindow::setLittleValue(int value)	{ ui->progressBar->setValue(m_value + value);			}
void batchWindow::setMaximum(int value)		{ ui->progressBar->setMaximum(value);					}
void batchWindow::setImagesCount(int value)	{ m_imagesCount = value; ui->labelImages->setText(QString("0/%2").arg(m_imagesCount));			}
void batchWindow::setImages(int value)		{ m_images = value; ui->labelImages->setText(QString("%1/%2").arg(value).arg(m_imagesCount));	}

int batchWindow::value()					{ return m_value;						}
int batchWindow::maximum()					{ return ui->progressBar->maximum();	}
int batchWindow::images()					{ return m_images;						}
int batchWindow::endAction()				{ return ui->comboEnd->currentIndex();	}
