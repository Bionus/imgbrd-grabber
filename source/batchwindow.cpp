#include "batchwindow.h"
#include "ui_batchwindow.h"



batchWindow::batchWindow(QWidget *parent) : QDialog(parent), ui(new Ui::batchWindow), m_items(0)
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
	ui->tableWidget->clear();
	ui->tableWidget->setRowCount(0);
	ui->labelMessage->setText("");
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(100);
	ui->labelImages->setText("0/0");
}
void batchWindow::copyToClipboard()
{
	QList<QTableWidgetItem *> selected = ui->tableWidget->selectedItems();
	QStringList urls = QStringList();
	for (int i = 0; i < selected.size(); i++)
	{
		if (selected.at(i)->icon().isNull())
		{ urls.append(selected.at(i)->text()); }
	}
	qApp->clipboard()->setText(urls.join("\n"));
}

void batchWindow::addImage(QString url)
{
	ui->tableWidget->setRowCount(m_items+1);
	QTableWidgetItem *id = new QTableWidgetItem(QString::number(m_items+1));
	id->setIcon(QIcon(":/images/colors/black.png"));
	ui->tableWidget->setItem(m_items, 0, id);
	ui->tableWidget->setItem(m_items, 1, new QTableWidgetItem(url));
	ui->tableWidget->resizeColumnToContents(0);
	m_items++;
}
void batchWindow::loadingImage(QString url)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 1)->text() == url)
		{ ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/blue.png")); }
	}
}
void batchWindow::loadedImage(QString url)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 1)->text() == url)
		{ ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/green.png")); }
	}
}
void batchWindow::errorImage(QString url)
{
	for (int i = 0; i < m_items; i++)
	{
		if (ui->tableWidget->item(i, 1)->text() == url)
		{ ui->tableWidget->item(i, 0)->setIcon(QIcon(":/images/colors/red.png")); }
	}
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

void batchWindow::setText(QString text)		{ ui->labelMessage->setText(text);		}
void batchWindow::setValue(int value)		{ ui->progressBar->setValue(value);		}
void batchWindow::setMaximum(int value)		{ ui->progressBar->setMaximum(value);	}
void batchWindow::setImagesCount(int value)	{ m_imagesCount = value; ui->labelImages->setText(QString("0/%2").arg(m_imagesCount));	}
void batchWindow::setImages(int value)		{ ui->labelImages->setText(QString("%1/%2").arg(value).arg(m_imagesCount));				}

int batchWindow::value()					{ return ui->progressBar->value();		}
int batchWindow::maximum()					{ return ui->progressBar->maximum();	}
