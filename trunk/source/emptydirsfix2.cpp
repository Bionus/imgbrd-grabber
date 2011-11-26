#include "emptydirsfix2.h"
#include "functions.h"
#include "ui_emptydirsfix2.h"

EmptyDirsFix2::EmptyDirsFix2(QString folder, QWidget *parent) : QDialog(parent), ui(new Ui::EmptyDirsFix2), m_folder(folder)
{
	ui->setupUi(this);
	mkList(QDir(folder));
	ui->listWidget->selectAll();
}
EmptyDirsFix2::~EmptyDirsFix2()
{
	delete ui;
}

void EmptyDirsFix2::mkList(QDir dir)
{
	QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dirs.size(); i++)
	{
		if (isEmpty(QDir(dir.path()+"/"+dirs.at(i))))
		{ ui->listWidget->addItem(new QListWidgetItem(dir.path()+"/"+dirs.at(i))); }
		else
		{ mkList(QDir(dir.path()+"/"+dirs.at(i))); }
	}
}
bool EmptyDirsFix2::isEmpty(QDir dir)
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
	for (int i = 0; i < sel.size(); i++)
	{ folders.append(sel.at(i)->text()); }

	int reponse = QMessageBox::question(this, tr("Réparateur de dossiers vides"), tr("Vous vous apprêtez à supprimer %n dossier(s). Êtes-vous sûr de vouloir continuer ?", "", folders.size()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{
		for (int i = 0; i < folders.size(); i++)
		{ removeDir(folders.at(i)); }
		close();
	}
}
