#include "customwindow.h"
#include "ui_customwindow.h"


CustomWindow::CustomWindow(QWidget *parent)
	: QDialog(parent), ui(new Ui::CustomWindow)
{
	ui->setupUi(this);
}

CustomWindow::~CustomWindow()
{
	delete ui;
}

void CustomWindow::accept()
{
	emit validated(ui->lineName->text(), ui->textTags->toPlainText());
	close();
}
