#ifndef EMPTY_DIRS_FIX_2_H
#define EMPTY_DIRS_FIX_2_H

#include <QDialog>
#include <QDir>
#include <QTreeWidgetItem>



namespace Ui
{
	class EmptyDirsFix2;
}



class EmptyDirsFix2 : public QDialog
{
	Q_OBJECT

	public:
		explicit EmptyDirsFix2(QString folder, QWidget *parent = 0);
		~EmptyDirsFix2();
		void mkList(QDir);
		bool isEmpty(QDir);
		bool removeDir(QString);
		QString remakePath(QTreeWidgetItem*);

	private slots:
		void deleteSel();

	private:
		Ui::EmptyDirsFix2	*ui;
		QString				m_folder;
};

#endif // EMPTY_DIRS_FIX_2_H
