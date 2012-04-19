#include "emptydirsfix.h"
#include "emptydirsfix2.h"
#include "functions.h"
#include "ui_emptydirsfix.h"

EmptyDirsFix::EmptyDirsFix(QWidget *parent) : QDialog(parent), ui(new Ui::EmptyDirsFix)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
}
EmptyDirsFix::~EmptyDirsFix()
{
	delete ui;
}

void EmptyDirsFix::next()
{
	EmptyDirsFix2 *edf2 = new EmptyDirsFix2(ui->lineFolder->text());
	close();
	edf2->show();
}
