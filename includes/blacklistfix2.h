#ifndef BLACKLISTFIX2_H
#define BLACKLISTFIX2_H

#include <QDialog>
#include <QLabel>



namespace Ui
{
	class BlacklistFix2;
}



class BlacklistFix2 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix2(QList<QMap<QString,QString> > details, QStringList blacklist, QWidget *parent = 0);
		~BlacklistFix2();

	private slots:
		void on_buttonSelectBlacklisted_clicked();
		void on_buttonCancel_clicked();
		void on_buttonOk_clicked();

	private:
		Ui::BlacklistFix2				*ui;
		QList<QMap<QString,QString> >	m_details;
		QList<QLabel*>					m_previews;
		QStringList						m_blacklist;
};

#endif // BLACKLISTFIX2_H
