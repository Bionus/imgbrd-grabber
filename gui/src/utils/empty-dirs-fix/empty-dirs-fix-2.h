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
		explicit EmptyDirsFix2(QStringList folders, QWidget *parent = 0);
		~EmptyDirsFix2();
		bool removeDir(QString);

	private slots:
		void deleteSel();

	private:
		Ui::EmptyDirsFix2 *ui;
};

#endif // EMPTY_DIRS_FIX_2_H
