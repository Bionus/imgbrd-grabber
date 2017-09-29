#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "models/source.h"


namespace Ui
{
	class sourcesWindow;
}


class QBouton;
class Site;
class Profile;

class sourcesWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit sourcesWindow(Profile *profile, QList<bool> selected, QMap<QString,Site*> *sites, QWidget *parent = Q_NULLPTR);
		~sourcesWindow() override;
		QList<bool> getSelected();

	public slots:
		void valid();
		void closeEvent(QCloseEvent *) override;
		void checkAll(int check = 2);
		void addSite();
		void settingsSite(QString);
		void deleteSite(QString);
		void checkUpdate();
		void checkClicked();
		void checkForUpdates();
		void checkForUpdatesReceived(QString source, bool isNew);
		void addCheckboxes();
		void removeCheckboxes();
		void updateCheckboxes();

	signals:
		void closed();
		void valid(QList<bool>);

	private:
		Ui::sourcesWindow *ui;
		Profile *m_profile;
		QList<bool> m_selected;
		QList<QCheckBox*> m_checks;
		QList<QLabel*> m_labels;
		QList<QBouton*> m_buttons;
		QMap<QString, Site*> *m_sites;
		QMap<QString, Source*> m_sources;
};

#endif // SOURCESWINDOW_H
