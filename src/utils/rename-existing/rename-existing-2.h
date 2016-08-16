#ifndef RENAME_EXISTING_2_H
#define RENAME_EXISTING_2_H

#include <QDialog>
#include <QLabel>



namespace Ui
{
	class RenameExisting2;
}



class RenameExisting2 : public QDialog
{
	Q_OBJECT

	public:
		explicit RenameExisting2(QList<QPair<QString,QString>> details, QString folder, QWidget *parent = 0);
		~RenameExisting2();
		void deleteDir(QString path);

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonOk_clicked();

	private:
		Ui::RenameExisting2				*ui;
		QList<QPair<QString,QString>>	m_details;
		QList<QLabel*>					m_previews;
		QString							m_folder;
};

#endif // RENAME_EXISTING_2_H
