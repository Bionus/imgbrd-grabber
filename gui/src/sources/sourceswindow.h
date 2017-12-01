#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QSettings>
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
		void openSite(QString site) const;
		void checkUpdate();
		void checkClicked();
		void checkForUpdates();
		void checkForUpdatesReceived(QString source, bool isNew);
		void addCheckboxes();
		void removeCheckboxes();
		void updateCheckboxes();
		QList<Site*> selected() const;

		// Presets
		QMap<QString, QStringList> loadPresets(QSettings *settings) const;
		void savePresets(QSettings *settings) const;
		void addPreset();
		void deletePreset();
		void editPreset();
		void savePreset();
		void selectPreset(QString name);

	signals:
		void closed();
		void valid(QList<Site*>);
		void siteDeleted(Site *site);

	private:
		Ui::sourcesWindow *ui;
		Profile *m_profile;
		QList<bool> m_selected;
		QList<QCheckBox*> m_checks;
		QList<QLabel*> m_labels;
		QList<QBouton*> m_buttons;
		QMap<QString, Site*> *m_sites;
		QMap<QString, Source*> m_sources;
		QMap<QString, QStringList> m_presets;
};

#endif // SOURCESWINDOW_H
