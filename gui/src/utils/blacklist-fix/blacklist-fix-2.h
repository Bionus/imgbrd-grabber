#ifndef BLACKLIST_FIX_2_H
#define BLACKLIST_FIX_2_H

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
		explicit BlacklistFix2(QList<QMap<QString,QString> > details, QStringList blacklist, QWidget *parent = Q_NULLPTR);
		~BlacklistFix2() override;

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

#endif // BLACKLIST_FIX_2_H
