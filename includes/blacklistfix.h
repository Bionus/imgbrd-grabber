#ifndef BLACKLISTFIX_H
#define BLACKLISTFIX_H

#include <QDialog>
#include <QMap>
#include "site.h"



namespace Ui
{
	class BlacklistFix;
}

class BlacklistFix : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix(QMap<QString,Site*> sites, QWidget *parent = 0);
		~BlacklistFix();

	private slots:
		void getAll(Page *p = nullptr);
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::BlacklistFix *ui;
		QMap<QString,Site*> m_sites;
		QList<QMap<QString,QString> > m_details;
		QMap<QString,QMap<QString,QString> > m_getAll;
};

#endif // BLACKLISTFIX_H
