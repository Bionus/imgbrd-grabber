#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define CLOSED_TAB_HISTORY_MAX 20

#include <QAtomicInteger>
#include <QLinkedList>
#include <QMainWindow>
#include <QProcess>
#include <QProgressBar>
#include <QQueue>
#include <QSet>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTableWidgetItem>
#include <QTranslator>
#include "downloader/image-downloader.h"
#include "models/image.h"
#include "models/site.h"


namespace Ui
{
	class mainWindow;
}


class searchTab;
class poolTab;
class tagTab;
class favoritesTab;
class batchWindow;
class Profile;
class Downloader;
class Favorite;
class DownloadQueryGroup;
class DownloadQueryImage;

class mainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit mainWindow(Profile *profile);
		~mainWindow() override;
		Ui::mainWindow *ui;

	public slots:
		// Log
		void logShow(const QString &msg);
		void logClear();
		void logOpen();
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
		// Language
		void loadLanguage(const QString&, bool quiet = false);
		void changeEvent(QEvent*) override;
		// Favorites
		void updateFavorites();
		void updateKeepForLater();
		// Batch download management
		void batchClear();
		void batchClearSel();
		void batchClearSelGroups();
		void batchClearSelUniques();
		void batchRemoveGroups(QList<int> rows);
		void batchRemoveUniques(QList<int> rows);
		void batchMove(int);
		void batchMoveUp();
		void batchMoveDown();
		void batchSel();
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const DownloadQueryGroup& values);
		void updateGroupCount();
		void batchAddUnique(const DownloadQueryImage &query, bool save = true);
		// Batch download
		void getAll(bool all = true);
		void getAllFinishedPage(Page *page);
		void getAllFinishedImages(const QList<QSharedPointer<Image> > &images);
		void getAllImages();
		void getAllGetImage(QSharedPointer<Image> img);
		void getAllGetImageSaved(QSharedPointer<Image> img, QMap<QString, Image::SaveResult> result);
		void getAllPerformTags();
		void getAllProgress(qint64, qint64);
		void getAllCancel();
		void getAllPause();
		void getAllSkip();
		void getAllLogin();
		void getNextPack();
		void getAllGetPages();
		void getAllFinished();
		void getAllFinishedLogin(Site *site, Site::LoginResult result);
		void getAllFinishedLogins();
		bool needExactTags(QSettings *settings);
		void _getAll();
		// Tabs
		void addTab(const QString &tag = "", bool background = false, bool save = true);
		void addPoolTab(int pool = 0, const QString &site = "", bool background = false, bool save = true);
		void addSearchTab(searchTab*, bool background = false, bool save = true);
		void updateTabTitle(searchTab*);
		void tabClosed(searchTab*);
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
		void closeEvent(QCloseEvent*) override;
		void onFirstLoad();
		void init(const QStringList &args, const QMap<QString, QString> &params);
		void parseArgs(const QStringList &args, const QMap<QString, QString> &params);
		void on_buttonSaveLinkList_clicked();
		void on_buttonLoadLinkList_clicked();
		bool saveLinkList(const QString &filename);
		bool loadLinkList(const QString &filename);
		void on_buttonSaveSettings_clicked();
		void on_buttonInitSettings_clicked();
		void saveSettings();
		void on_buttonFolder_clicked();
		void imageUrlChanged(const QString &, const QString &);
		void updateCompleters();
		void setSource(const QString &site);
		void setTags(const QList<Tag> &tags, searchTab *from = nullptr);
		void initialLoginsFinished();
		QIcon& getIcon(const QString &path);
		void setWiki(const QString &);
		void siteDeleted(Site *site);

		// Drag & drop
		void dragEnterEvent(QDragEnterEvent *event) override;
		void dropEvent(QDropEvent* event) override;

	protected:
		int getRowForSite(int site_id);
		void getAllGetImageIfNotBlacklisted(QSharedPointer<Image> img, int site_id);
		void getAllImageOk(QSharedPointer<Image> img, int site_id);
		Site* getSelectedSiteOrDefault();
		void initialLoginsDone();
		void addTableItem(QTableWidget *table, int row, int col, const QString &text);

	private:
		Profile				*m_profile;
		QList<Favorite>		&m_favorites;
		int					m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAllIgnoredPre, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllLimit, m_downloads, m_waitForLogin;
		bool				m_allow, m_mustGetTags, m_loaded, m_getAll;
		int					m_forcedTab;
		QSettings			*m_settings;
		batchWindow			*m_progressDialog;
		QString				m_currLang, m_link;
		QTranslator			m_translator, m_qtTranslator;
		QList<DownloadQueryGroup>		m_groupBatchs;
		QList<QSharedPointer<Image>>	m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_images, m_getAllSkippedImages;
		QMap<QSharedPointer<Image>, ImageDownloader*>	m_getAllImageDownloaders;
		QWidget				*m_currentTab;
		QList<searchTab*>	m_tabs, m_tabsWaitingForPreload;
		QList<Site*>		m_selectedSites;
		favoritesTab		*m_favoritesTab;
		QMap<QString, QTime>			m_downloadTime, m_downloadTimeLast;
		QList<QProgressBar*>			m_progressBars;
		QList<DownloadQueryImage>		m_batchs;
		QMap<int, DownloadQueryGroup>	m_batchPending;
		QSet<int>						m_batchDownloading;
		QStringList			m_lineFilename_completer, m_lineFolder_completer;
		QList<Downloader*>  m_downloaders;
		Downloader			*m_lastDownloader;
		QQueue<Downloader*>	m_waitingDownloaders;
		QList<Site*>		m_getAllLogins;
		int					m_batchAutomaticRetries, m_getAllImagesCount, m_batchCurrentPackSize;
		bool				m_restore, m_showLog;
		QMap<QString, QIcon>	m_icons;
		QList<Tag>				m_currentTags;
		QLinkedList<QJsonObject>	m_closedTabs;
		QNetworkAccessManager m_networkAccessManager;
		QAtomicInteger<int> m_getAllCurrentlyProcessing;

		// System tray
		QSystemTrayIcon *m_trayIcon;
		bool m_closeFromTray = false;
};

#endif // MAIN_WINDOW_H
