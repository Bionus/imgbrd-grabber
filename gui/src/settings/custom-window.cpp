#include "custom-window.h"
#include "ui_custom-window.h"


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
