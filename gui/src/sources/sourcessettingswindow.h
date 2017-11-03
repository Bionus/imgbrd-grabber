#ifndef SOURCESSETTINGSWINDOW_H
#define SOURCESSETTINGSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QMap>
#include "models/site.h"
#include "models/profile.h"


namespace Ui
{
	class SourcesSettingsWindow;
}


class SourcesSettingsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SourcesSettingsWindow(Profile *profile, Site* site, QWidget *parent = Q_NULLPTR);
		~SourcesSettingsWindow() override;

	public slots:
		void on_buttonAuthHash_clicked();
		void deleteSite();
		void addCookie();
		void addHeader();
		void save();
		void testLogin();
		void loginTested(Site*, Site::LoginResult);

	signals:
		void siteDeleted(QString);

	private:
		Ui::SourcesSettingsWindow *ui;
		Site *m_site;
		QSettings *m_globalSettings;
};

#endif // SOURCESSETTINGSWINDOW_H
