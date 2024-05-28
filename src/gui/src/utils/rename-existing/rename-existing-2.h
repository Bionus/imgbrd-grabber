#ifndef RENAME_EXISTING_2_H
#define RENAME_EXISTING_2_H

#include <QDialog>
#include <QLabel>
#include "utils/rename-existing/rename-existing-file.h"


namespace Ui
{
	class RenameExisting2;
}


class RenameExistingTableModel;

class RenameExisting2 : public QDialog
{
	Q_OBJECT

	public:
		explicit RenameExisting2(QList<RenameExistingFile> details, QString folder, QWidget *parent = nullptr);
		~RenameExisting2() override;
		void deleteDir(const QString &path);

	private slots:
		void cancel();
		void nextStep();

	private:
		Ui::RenameExisting2 *ui;
		QList<RenameExistingFile> m_details;
		QList<QLabel*> m_previews;
		QString m_folder;
		RenameExistingTableModel *m_model;
};

#endif // RENAME_EXISTING_2_H
