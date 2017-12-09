#include "utils/empty-dirs-fix/empty-dirs-fix-2.h"
#include <QMessageBox>
#include <ui_empty-dirs-fix-2.h>



EmptyDirsFix2::EmptyDirsFix2(QStringList folders, QWidget *parent) : QDialog(parent), ui(new Ui::EmptyDirsFix2)
{
	ui->setupUi(this);

	for (const QString &folder : folders)
	{ ui->listWidget->addItem(new QListWidgetItem(folder)); }
	ui->listWidget->selectAll();
}
EmptyDirsFix2::~EmptyDirsFix2()
{
	delete ui;
}

bool EmptyDirsFix2::removeDir(QString path)
{
	path = QDir::toNativeSeparators(path);
	QDir dir(path);
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{ removeDir(path+"/"+dirs.at(i)); }
	return QDir().rmdir(path);
}

void EmptyDirsFix2::deleteSel()
{
	QStringList folders;
	QList<QListWidgetItem*> sel = ui->listWidget->selectedItems();
	for (QListWidgetItem *s : sel)
	{ folders.append(s->text()); }

	if (folders.isEmpty())
	{
		QMessageBox::information(this, tr("Empty folders fixer"), tr("No folder selected."));
		return;
	}

	int response = QMessageBox::question(this, tr("Empty folders fixer"), tr("You are about to delete %n folder. Are you sure you want to continue?", "", folders.size()), QMessageBox::Yes | QMessageBox::No);
	if (response == QMessageBox::Yes)
	{
		for (int i = 0; i < folders.size(); i++)
		{ removeDir(folders.at(i)); }
		close();
	}
}
