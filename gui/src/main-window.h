#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define CLOSED_TAB_HISTORY_MAX 20

#include <QLinkedList>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QSystemTrayIcon>
#include "language-loader.h"


namespace Ui
{
	class MainWindow;
}


class SearchTab;
class FavoritesTab;
class Profile;
class DownloadsTab;
class Favorite;
class MonitoringCenter;
class QSettings;
class Site;
class Tag;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(Profile *profile);
		~MainWindow() override;
		Ui::MainWindow *ui;
		Site *getSelectedSiteOrDefault();

	public slots:
		// Menus
		void options();
		void optionsClosed();
		void aboutAuthor();
		void aboutWebsite();
		void aboutGithub();
		void aboutReportBug();
		void saveFolder();
		void openSettingsFolder();
		void blacklistFix();
		void emptyDirsFix();
		void md5FixOpen();
		void renameExisting();
		void utilTagLoader();
		void changeEvent(QEvent *event) override;
		// Favorites
		void updateFavorites();
		void updateKeepForLater();
		// Tabs
		void addTab(const QString &tag = "", bool background = false, bool save = true);
		void addPoolTab(int pool = 0, const QString &site = "", bool background = false, bool save = true);
		void addGalleryTab(Site *site, QString name, QString id, bool background = false, bool save = true);
		void addSearchTab(SearchTab*, bool background = false, bool save = true);
		void updateTabTitle(SearchTab*);
		void tabClosed(SearchTab*);
		void restoreLastClosedTab();
		void currentTabChanged(int);
		void closeCurrentTab();
		bool saveTabs(const QString &filename);
		bool loadTabs(const QString &filename);
		void updateTabs();
		void focusSearch();
		void tabNext();
		void tabPrev();
		// Tag list
		void loadMd5(const QString &path, bool newTab = true, bool background = true, bool save = true);
		void loadTag(const QString &tag, bool newTab = true, bool background = true, bool save = true);
		void loadTagTab(const QString &tag);
		void loadTagNoTab(const QString &tag);
		void linkHovered(const QString &tag);
		void contextMenu();
		void openInNewTab();
		// System tray
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
		void trayMessageClicked();
		void trayClose();
		// Others
		void closeEvent(QCloseEvent *) override;
		void onFirstLoad();
		void init(const QStringList &args, const QMap<QString, QString> &params);
		void parseArgs(const QStringList &args, const QMap<QString, QString> &params);
		void on_buttonSaveSettings_clicked();
		void on_buttonInitSettings_clicked();
		void saveSettings();
		void on_buttonFolder_clicked();
		void updateCompleters();
		void setSource(const QString &site);
		void setTags(const QList<Tag> &tags, SearchTab *from = nullptr);
		void initialLoginsFinished();
		void setWiki(const QString &wiki, SearchTab *from = nullptr);

		// Drag & drop
		void dragEnterEvent(QDragEnterEvent *event) override;
		void dropEvent(QDropEvent *event) override;

	protected:
		void initialLoginsDone();

	private:
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		int m_waitForLogin;
		bool m_loaded;
		int m_forcedTab;
		QSettings *m_settings;
		QString m_link;
		LanguageLoader m_languageLoader;
		SearchTab *m_currentTab;
		QList<SearchTab*> m_tabs, m_tabsWaitingForPreload;
		QList<Site*> m_selectedSites;
		FavoritesTab *m_favoritesTab;
		DownloadsTab *m_downloadsTab;
		QStringList m_lineFilename_completer, m_lineFolder_completer;
		bool m_restore;
		QList<Tag> m_currentTags;
		QLinkedList<QJsonObject> m_closedTabs;
		QNetworkAccessManager m_networkAccessManager;

		// System tray
		QSystemTrayIcon *m_trayIcon;
		bool m_closeFromTray = false;
		MonitoringCenter *m_monitoringCenter;
};

#endif // MAIN_WINDOW_H
