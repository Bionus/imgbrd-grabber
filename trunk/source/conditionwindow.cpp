#include "conditionwindow.h"
#include "ui_conditionwindow.h"



conditionWindow::conditionWindow(QWidget *parent) : QDialog(parent), ui(new Ui::conditionWindow)
{
	ui->setupUi(this);
}

conditionWindow::~conditionWindow()
{
	delete ui;
}

void conditionWindow::accept()
{
	emit validated(ui->lineCondition->text(), ui->lineFilename->text());
	close();
}
