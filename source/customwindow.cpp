#include "customwindow.h"
#include "ui_customwindow.h"



customWindow::customWindow(QWidget *parent) : QDialog(parent), ui(new Ui::customWindow)
{
	ui->setupUi(this);
}

customWindow::~customWindow()
{
	delete ui;
}

void customWindow::accept()
{
	emit validated(ui->lineName->text(), ui->textTags->toPlainText());
	close();
}
