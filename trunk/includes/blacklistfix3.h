#ifndef BLACKLISTFIX3_H
#define BLACKLISTFIX3_H

#include <QtGui>

namespace Ui {
	class BlacklistFix3;
}

class BlacklistFix3 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix3(QList<QMap<QString,QString> > details, QStringList blacklist, QWidget *parent = 0);
		~BlacklistFix3();

	private slots:
		void on_buttonSelectBlacklisted_clicked();
		void on_buttonCancel_clicked();
		void on_buttonOk_clicked();

	private:
		Ui::BlacklistFix3				*ui;
		QList<QMap<QString,QString> >	m_details;
		QList<QLabel*>					m_previews;
		QStringList						m_blacklist;
};

#endif // BLACKLISTFIX3_H
