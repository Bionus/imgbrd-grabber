#ifndef SOURCESSETTINGSWINDOW_H
#define SOURCESSETTINGSWINDOW_H

#include <QDialog>
#include <QMap>
#include "site.h"



namespace Ui
{
	class SourcesSettingsWindow;
}



class SourcesSettingsWindow : public QDialog
{
	Q_OBJECT
	
	public:
		explicit SourcesSettingsWindow(Site* site, QWidget *parent = 0);
		~SourcesSettingsWindow();

	public slots:
		void on_buttonAuthHash_clicked();
		void deleteSite();
		void save();

	signals:
		void siteDeleted(QString);

	private:
		Ui::SourcesSettingsWindow *ui;
		Site *m_site;
};

#endif // SOURCESSETTINGSWINDOW_H
