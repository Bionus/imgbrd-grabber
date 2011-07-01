#include "batchwindow.h"
#include "ui_batchwindow.h"



batchWindow::batchWindow(QWidget *parent) : QDialog(parent), ui(new Ui::batchWindow)
{
	ui->setupUi(this);
	ui->scrollArea->hide();
	resize(QSize(300, 0));
	m_currentSize = QSize(300, 225);
}

batchWindow::~batchWindow()
{
    delete ui;
}
void batchWindow::closeEvent(QCloseEvent *e)
{
	emit closed();
	e->accept();
}

void batchWindow::on_buttonDetails_clicked()
{
	if (ui->scrollArea->isHidden())
	{
		ui->scrollArea->show();
		resize(m_currentSize);
	}
	else
	{
		ui->scrollArea->hide();
		m_currentSize = size();
		resize(QSize(300, 0));
	}
}

void batchWindow::setText(QString text)		{ ui->labelMessage->setText(text);		}
void batchWindow::setLog(QString text)		{ ui->labelLog->setText(text);			}
void batchWindow::setValue(int value)		{ ui->progressBar->setValue(value);		}
void batchWindow::setMaximum(int value)		{ ui->progressBar->setMaximum(value);	}
void batchWindow::setImagesCount(int value)	{ m_imagesCount = value; ui->labelImages->setText(QString("0/%2").arg(m_imagesCount));	}
void batchWindow::setImages(int value)		{ ui->labelImages->setText(QString("%1/%2").arg(value).arg(m_imagesCount));				}

int batchWindow::value()					{ return ui->progressBar->value();		}
int batchWindow::maximum()					{ return ui->progressBar->maximum();	}
