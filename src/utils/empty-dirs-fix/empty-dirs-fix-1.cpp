#include <QSettings>
#include "empty-dirs-fix-1.h"
#include "empty-dirs-fix-2.h"
#include "ui_empty-dirs-fix-1.h"



EmptyDirsFix1::EmptyDirsFix1(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::EmptyDirsFix1)
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
}
EmptyDirsFix1::~EmptyDirsFix1()
{
	delete ui;
}

void EmptyDirsFix1::next()
{
	EmptyDirsFix2 *edf2 = new EmptyDirsFix2(ui->lineFolder->text());
	close();
	edf2->show();
}
