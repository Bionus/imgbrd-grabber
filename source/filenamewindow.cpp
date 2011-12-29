#include "filenamewindow.h"
#include "ui_filenamewindow.h"



filenameWindow::filenameWindow(QWidget *parent) : QDialog(parent), ui(new Ui::filenameWindow)
{
	ui->setupUi(this);
}

filenameWindow::~filenameWindow()
{
	delete ui;
}

void filenameWindow::accept()
{
	emit validated(ui->lineCondition->text(), ui->lineFilename->text());
	close();
}
