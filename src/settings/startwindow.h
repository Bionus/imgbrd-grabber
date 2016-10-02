#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>
#include "models/site.h"
#include "models/profile.h"



namespace Ui
{
	class startWindow;
}


class mainWindow;

class startWindow : public QDialog
{
	Q_OBJECT

	public:
		startWindow(QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent);
		~startWindow();

	public slots:
		void save();
		void openOptions();
		void on_buttonFolder_clicked();
		void on_buttonFilenamePlus_clicked();

	signals:
		void sourceChanged(QString);
		void languageChanged(QString);
		void settingsChanged();

	private:
		Ui::startWindow *ui;
		mainWindow *m_parent;
		Profile *m_profile;
		QMap<QString, Site*> *m_sites;

};

#endif // STARTWINDOW_H
