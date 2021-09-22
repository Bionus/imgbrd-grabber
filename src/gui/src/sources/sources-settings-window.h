#ifndef SOURCESSETTINGSWINDOW_H
#define SOURCESSETTINGSWINDOW_H

#include <QDialog>
#include <QMap>
#include <QString>
#include "models/site.h"


namespace Ui
{
	class SourcesSettingsWindow;
}


class Profile;
class QLineEdit;
class QSettings;
class QWidget;

class SourcesSettingsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SourcesSettingsWindow(Profile *profile, Site *site, QWidget *parent = nullptr);
		~SourcesSettingsWindow() override;

	public slots:
		void deleteSite();
		void addCookie();
		void addHeader();
		void save();
		void setLoginType(int index);
		void testLogin();
		void loginTested(Site *site, Site::LoginResult result);

	signals:
		void siteDeleted(const QString &);

	protected:
		void saveSettings();
		void setLoginStatus(const QString &msg);
		void updateFields();

	private:
		Ui::SourcesSettingsWindow *ui;
		Site *m_site;
		QSettings *m_globalSettings;
		QMap<QString, QMap<QString, QLineEdit*>> m_credentialFields;
		QList<bool> m_canTestLogin;
};

#endif // SOURCESSETTINGSWINDOW_H
