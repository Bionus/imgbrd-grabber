#ifndef SOURCESSETTINGSWINDOW_H
#define SOURCESSETTINGSWINDOW_H

#include <QDialog>
#include "models/site.h"


namespace Ui
{
	class SourcesSettingsWindow;
}


class Profile;

class SourcesSettingsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SourcesSettingsWindow(Profile *profile, Site* site, QWidget *parent = nullptr);
		~SourcesSettingsWindow() override;

	public slots:
		void on_buttonAuthHash_clicked();
		void deleteSite();
		void addCookie();
		void addHeader();
		void save();
		void testLogin();
		void loginTested(Site *site, Site::LoginResult result);

	signals:
		void siteDeleted(const QString &);

	protected:
		void setLoginStatus(const QString &msg);

	private:
		Ui::SourcesSettingsWindow *ui;
		Site *m_site;
		QSettings *m_globalSettings;
};

#endif // SOURCESSETTINGSWINDOW_H
