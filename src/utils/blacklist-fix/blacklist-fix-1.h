#ifndef BLACKLIST_FIX_1_H
#define BLACKLIST_FIX_1_H

#include <QDialog>
#include <QMap>
#include "models/site.h"



namespace Ui
{
	class BlacklistFix1;
}

class BlacklistFix1 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix1(Profile *profile, QMap<QString,Site*> sites, QWidget *parent = 0);
		~BlacklistFix1();

	private slots:
		void getAll(Page *p = nullptr);
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::BlacklistFix1 *ui;
		Profile *m_profile;
		QMap<QString,Site*> m_sites;
		QList<QMap<QString,QString> > m_details;
		QMap<QString,QMap<QString,QString> > m_getAll;
};

#endif // BLACKLIST_FIX_1_H
