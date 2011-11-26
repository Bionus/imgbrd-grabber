#ifndef EmptyDirsFix2_H
#define EmptyDirsFix2_H

#include <QtGui>
#include "page.h"

namespace Ui {
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

#endif // EmptyDirsFix2_H
