#include "condition-window.h"
#include "ui_condition-window.h"
#include "functions.h"


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
	const QString dir = fixFilename("", ui->lineFolder->text())
	emit validated(ui->lineCondition->text(), ui->lineFilename->text(), dir);
	close();
}
