#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define CLOSED_TAB_HISTORY_MAX 20

#include <QMainWindow>
#include <QSettings>
#include <QProcess>
#include <QTranslator>
#include <QSet>
#include <QQueue>
#include <QLinkedList>
#include <QTableWidgetItem>
#include <QProgressBar>
#include "models/site.h"
#include "models/image.h"
#include "downloader/image-downloader.h"


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
		~mainWindow();
		Ui::mainWindow *ui;

	public slots:
		// Log
		void logShow(QString msg);
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
		void loadLanguage(const QString&, bool shutup = false);
		void changeEvent(QEvent*);
		// Favorites
		void updateFavorites();
		void updateKeepForLater();
		/*void loadFavorite(int);
		void favoriteProperties(int);
		void viewed();
		void setFavoriteViewed(QString);
		void favoritesBack();
		void checkFavorites();
		void loadNextFavorite();*/
		// Download
		/*void web(QString tags = "");
		void finishedLoading(Page*);
		void finishedLoadingPreview(QSharedPointer<Image>);
		void webZoom(int);*/
		// Batch download management
		void batchClear();
		void batchClearSel();
		void batchClearSelGroups();
		void batchClearSelUniques();
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
		void getAllFinishedImages(QList<QSharedPointer<Image>> images);
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
		void addTab(QString tag = "", bool background = false, bool save = true);
		void addPoolTab(int pool = 0, QString site = "", bool background = false, bool save = true);
		void addSearchTab(searchTab*, bool background = false, bool save = true);
		void updateTabTitle(searchTab*);
		void tabClosed(searchTab*);
		void restoreLastClosedTab();
		void currentTabChanged(int);
		void closeCurrentTab();
		bool saveTabs(QString);
		bool loadTabs(QString);
		void updateTabs();
		void focusSearch();
		void tabNext();
		void tabPrev();
		// Tag list
		void loadMd5(QString path, bool newTab = true, bool background = true, bool save = true);
		void loadTag(QString tag, bool newTab = true, bool background = true, bool save = true);
		void loadTagTab(QString tag);
		void loadTagNoTab(QString tag);
		void linkHovered(QString tag);
		void contextMenu();
		void openInNewTab();
		// Others
		void closeEvent(QCloseEvent*);
		void onFirstLoad();
		void init(QStringList args, QMap<QString,QString> params);
		void parseArgs(QStringList args, QMap<QString,QString> params);
		void on_buttonSaveLinkList_clicked();
		void on_buttonLoadLinkList_clicked();
		bool saveLinkList(QString filename);
		bool loadLinkList(QString filename);
		void on_buttonSaveSettings_clicked();
		void on_buttonInitSettings_clicked();
		void saveSettings();
		void on_buttonFolder_clicked();
		void loadSites();
		void imageUrlChanged(QString, QString);
		void updateCompleters();
		void setSource(QString site);
		void setTags(QList<Tag> tags, searchTab *from = nullptr);
		void initialLoginsFinished();
		QIcon& getIcon(QString path);
		void setWiki(QString);

		// Drag & drop
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent* event);

	protected:
		int getRowForSite(int site_id);
		void getAllGetImageIfNotBlacklisted(QSharedPointer<Image> img, int site_id);
		void getAllImageOk(QSharedPointer<Image> img, int site_id, bool del = true);
		QList<Site*> getSelectedSites();
		Site* getSelectedSiteOrDefault();
		void initialLoginsDone();
		void addTableItem(QTableWidget *table, int row, int col, QString text);

	private:
		Profile				*m_profile;
		QList<Favorite>		&m_favorites;
		int					m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllLimit, m_downloads, m_waitForLogin;
		bool				m_allow, m_mustGetTags, m_loaded, m_getAll;
		int					m_forcedTab;
		QSettings			*m_settings;
		batchWindow			*m_progressdialog;
		QString				m_currLang, m_link;
		QTranslator			m_translator, m_qtTranslator;
		QList<DownloadQueryGroup>		m_groupBatchs;
		QList<QSharedPointer<Image>>	m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_images, m_getAllSkippedImages;
		QMap<QSharedPointer<Image>, ImageDownloader*>	m_getAllImageDownloaders;
		QWidget				*m_currentTab;
		QList<searchTab*>	m_tabs, m_tabsWaitingForPreload;
		QList<bool>			m_selectedSources;
		favoritesTab		*m_favoritesTab;
		QMap<QString,QTime*>			m_downloadTime, m_downloadTimeLast;
		QList<QProgressBar*>			m_progressBars;
		QList<DownloadQueryImage>		m_batchs;
		QMap<int, DownloadQueryGroup>	m_batchPending;
		QSet<int>						m_batchDownloading;
		QStringList			m_lineFilename_completer, m_lineFolder_completer;
		QList<Downloader*>  m_downloaders, m_downloadersDone;
		QQueue<Downloader*>	m_waitingDownloaders;
		QList<Site*>		m_getAllLogins;
		int					m_batchAutomaticRetries, m_getAllImagesCount;
		bool				m_restore, m_showLog;
		QMap<QString, QIcon>	m_icons;
		QMap<QString, Site*>	m_sites;
		QList<Tag>				m_currentTags;
		QLinkedList<QJsonObject>	m_closedTabs;
		QNetworkAccessManager m_networkAccessManager;
};

#endif // MAINWINDOW_H
