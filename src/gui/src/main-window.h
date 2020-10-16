#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define CLOSED_TAB_HISTORY_MAX 20

#include <QMainWindow>
#include <QStack>
#include <QSystemTrayIcon>
#include <QVariant>
#include "language-loader.h"
#include "network/network-manager.h"


namespace Ui
{
	class MainWindow;
}


class SearchTab;
class FavoritesTab;
class Profile;
class DownloadQueue;
class DownloadsTab;
class Image;
class LogTab;
class Favorite;
class MonitoringCenter;
class MonitorsTab;
class QSettings;
class SettingsDock;
class Site;
class TabSelector;
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
		void utilMd5DatabaseConverter();
		void changeEvent(QEvent *event) override;
		// Tabs
		void addTab(const QString &tag = "", bool background = false, bool save = true, SearchTab *source = nullptr);
		void addPoolTab(int pool = 0, const QString &site = "", bool background = false, bool save = true, SearchTab *source = nullptr);
		void addGalleryTab(Site *site, QSharedPointer<Image> gallery, bool background = false, bool save = true, SearchTab *source = nullptr);
		void addSearchTab(SearchTab*, bool background = false, bool save = true, SearchTab *source = nullptr);
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
		void setCurrentTab(QWidget *widget);
		// Tag list
		void loadMd5(const QString &path, bool newTab = true, bool background = true, bool save = true, SearchTab *source = nullptr);
		void loadTag(const QString &tag, bool newTab = true, bool background = true, bool save = true, SearchTab *source = nullptr);
		void loadTagTab(const QString &tag);
		void loadTagNoTab(const QString &tag);
		// System tray
		void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
		void trayMessageClicked();
		void trayClose();
		// Others
		void closeEvent(QCloseEvent *) override;
		void onFirstLoad();
		void init(const QStringList &args, const QMap<QString, QString> &params);
		void parseArgs(const QStringList &args, const QMap<QString, QString> &params);
		void setSource(const QString &site);
		void initialLoginsFinished();
		void tabContextMenuRequested(const QPoint &pos);

		// Drag & drop
		void dragEnterEvent(QDragEnterEvent *event) override;
		void dropEvent(QDropEvent *event) override;

	signals:
		void tabChanged(SearchTab *tab);

	protected:
		void initialLoginsDone();

	private:
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		int m_waitForLogin;
		bool m_loaded;
		QVariant m_forcedTab;
		QSettings *m_settings;
		QString m_link;
		LanguageLoader m_languageLoader;
		SearchTab *m_currentTab;
		QList<SearchTab*> m_tabs, m_tabsWaitingForPreload;
		QList<Site*> m_selectedSites;
		FavoritesTab *m_favoritesTab;
		DownloadsTab *m_downloadsTab;
		MonitorsTab *m_monitorsTab;
		LogTab *m_logTab;
		bool m_restore;
		QList<Tag> m_currentTags;
		QStack<QJsonObject> m_closedTabs;
		NetworkManager m_networkManager;
		TabSelector *m_tabSelector;
		DownloadQueue *m_downloadQueue;
		SettingsDock *m_settingsDock;

		// System tray
		QSystemTrayIcon *m_trayIcon;
		bool m_closeFromTray = false;
		MonitoringCenter *m_monitoringCenter;
};

#endif // MAIN_WINDOW_H
