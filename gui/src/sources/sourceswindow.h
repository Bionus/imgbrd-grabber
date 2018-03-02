#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QNetworkReply>
#include <QSettings>


namespace Ui
{
	class sourcesWindow;
}


class Profile;
class QBouton;
class Site;
class Source;

class sourcesWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit sourcesWindow(Profile *profile, const QList<Site*> &selected, QWidget *parent = Q_NULLPTR);
		~sourcesWindow() override;

	public slots:
		void valid();
		void closeEvent(QCloseEvent *) override;
		void checkAll(int check = 2);
		void addSite();
		void settingsSite(const QString &site);
		void deleteSite(const QString &site);
		void openSite(const QString &site) const;
		void checkUpdate();
		void checkClicked();
		void checkForUpdates();
		void checkForUpdatesReceived(const QString &source, bool isNew);
		void checkForSourceIssues();
		void checkForSourceIssuesReceived();
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
		void selectPreset(const QString &name);

	signals:
		void closed();
		void valid(const QList<Site*> &selectedSites);

	private:
		Ui::sourcesWindow *ui;
		Profile *m_profile;
		QList<Site*> m_selected;
		QList<QCheckBox*> m_checks;
		QList<QLabel*> m_labels;
		QList<QBouton*> m_buttons;
		const QMap<QString, Site*> &m_sites;
		const QMap<QString, Source*> &m_sources;
		QMap<QString, QStringList> m_presets;
		QNetworkReply *m_checkForSourceReply;
};

#endif // SOURCESWINDOW_H
