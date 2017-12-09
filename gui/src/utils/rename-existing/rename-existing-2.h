#ifndef RENAME_EXISTING_2_H
#define RENAME_EXISTING_2_H

#include <QDialog>
#include <QLabel>
#include "utils/rename-existing/rename-existing-file.h"


namespace Ui
{
	class RenameExisting2;
}


class RenameExisting2 : public QDialog
{
	Q_OBJECT

	public:
		explicit RenameExisting2(QList<RenameExistingFile> details, QString folder, QWidget *parent = Q_NULLPTR);
		~RenameExisting2() override;
		void deleteDir(QString path);

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonOk_clicked();

	private:
		Ui::RenameExisting2				*ui;
		QList<RenameExistingFile>		m_details;
		QList<QLabel*>					m_previews;
		QString							m_folder;
};

#endif // RENAME_EXISTING_2_H
