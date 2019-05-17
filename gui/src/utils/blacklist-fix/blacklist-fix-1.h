#ifndef BLACKLIST_FIX_1_H
#define BLACKLIST_FIX_1_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class BlacklistFix1;
}


class Profile;
class Site;
class Page;

class BlacklistFix1 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix1(Site *selected, Profile *profile, QWidget *parent = nullptr);
		~BlacklistFix1() override;

	private slots:
		void getAll(Page *p = nullptr);
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::BlacklistFix1 *ui;
		Profile *m_profile;
		QMap<QString, Site*> m_sites;
		QList<QMap<QString, QString>> m_details;
		QMap<QString, QMap<QString, QString>> m_getAll;
};

#endif // BLACKLIST_FIX_1_H
