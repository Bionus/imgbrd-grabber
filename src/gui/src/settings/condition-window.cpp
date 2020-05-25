#include "condition-window.h"
#include "ui_condition-window.h"


ConditionWindow::ConditionWindow(QWidget *parent)
	: QDialog(parent), ui(new Ui::ConditionWindow)
{
	ui->setupUi(this);
}

ConditionWindow::~ConditionWindow()
{
	delete ui;
}

void ConditionWindow::accept()
{
	emit validated(ui->lineCondition->text(), ui->lineFilename->text(), ui->lineFolder->text());
	close();
}
