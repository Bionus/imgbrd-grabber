#ifndef RENAME_EXISTING_1_H
#define RENAME_EXISTING_1_H

#include <QDialog>
#include <QMap>
#include "models/site.h"



namespace Ui
{
	class RenameExisting1;
}

class RenameExisting1 : public QDialog
{
	Q_OBJECT

	public:
		explicit RenameExisting1(QMap<QString,Site*> sites, QWidget *parent = 0);
		~RenameExisting1();

	private slots:
		void getAll(Page *p = nullptr);
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::RenameExisting1						*ui;
		QMap<QString, Site*>					m_sites;
		QList<QMap<QString, QString>>			m_details;
		QMap<QString, QPair<QString, QString>>	m_getAll;
};

#endif // RENAME_EXISTING_1_H
