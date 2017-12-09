#include "utils/empty-dirs-fix/empty-dirs-fix-1.h"
#include <QMessageBox>
#include <QSettings>
#include <ui_empty-dirs-fix-1.h>
#include "models/profile.h"
#include "utils/empty-dirs-fix/empty-dirs-fix-2.h"


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
	QStringList dirs = mkList(QDir(ui->lineFolder->text()));

	// We don't continue if there were no folders found
	if (dirs.isEmpty())
	{
		QMessageBox::information(this, tr("Empty folders fixer"), tr("No empty folder found."));
		close();
		return;
	}

	EmptyDirsFix2 *edf2 = new EmptyDirsFix2(dirs);
	close();
	edf2->show();
}

QStringList EmptyDirsFix1::mkList(QDir dir)
{
	QStringList ret;
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{
		if (isEmpty(QDir(dir.path()+"/"+dirs.at(i))))
		{ ret.append(dir.path()+"/"+dirs.at(i)); }
		else
		{ mkList(QDir(dir.path()+"/"+dirs.at(i))); }
	}
	return ret;
}

bool EmptyDirsFix1::isEmpty(QDir dir)
{
	QStringList files = dir.entryList(QDir::Files);
	if (!files.isEmpty())
	{ return false; }

	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	bool empty = true;
	for (int i = 0; i < dirs.size(); i++)
	{ empty = empty && isEmpty(QDir(dir.path()+"/"+dirs.at(i))); }
	return empty;
}
