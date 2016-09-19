#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QProcess>
#include <QTranslator>
#include <QSet>
#include <QTableWidgetItem>
#include "sources/sourceswindow.h"
#include "batch/batchwindow.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"
#include "ui/textedit.h"
#include "tabs/search-tab.h"
#include "tabs/pool-tab.h"
#include "tabs/tag-tab.h"
#include "tabs/favorites-tab.h"
#include "commands.h"
#include "models/site.h"
#include "models/profile.h"
#include "downloader.h"



namespace Ui
{
	class mainWindow;
	class poolTab;
	class tagTab;
	class favoritesTab;
}



class searchTab;
class poolTab;
class tagTab;
class favoritesTab;

class mainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit mainWindow(QString, QStringList, QMap<QString,QString>);
		~mainWindow();
		QMap<QString,Site*> m_sites;
		Ui::mainWindow *ui;
		QSettings* settings();

	public slots:
		// Log
		void logShow(QDateTime date, QString msg);
		void logClear();
		void logOpen();
		// Menus
		void options();
		void optionsClosed();
		void aboutAuthor();
		void aboutWebsite();
		void aboutReportBug();
		void saveFolder();
		void openSettingsFolder();
		void blacklistFix();
		void emptyDirsFix();
		void md5FixOpen();
		void renameExisting();
		// Language
		void switchTranslator(QTranslator&, const QString&);
		void loadLanguage(const QString&, bool shutup = false);
		void changeEvent(QEvent*);
		// Favorites
		void updateFavorites(bool dock = false);
		void updateFavoritesDock();
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
		void finishedLoadingPreview(Image*);
		void webZoom(int);*/
		// Batch download management
		void batchClear();
		void batchClearSel();
		QList<int> getSelectedRows(QList<QTableWidgetItem*> selected);
		void batchMoveUp();
		void batchMoveDown();
		void batchSel();
		void batchChange(int);
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const QStringList& values);
		void updateGroupCount();
		void batchAddUnique(QMap<QString,QString> values, bool save = true);
		// Batch download
		void getAll(bool all = true);
		void getAllFinishedPage(Page *page);
		void getAllFinishedImages(QList<Image*> images);
		void getAllImages();
		void getAllGetImage(Image*);
		void getAllPerformTags(Image*);
		void getAllPerformImage(Image*);
		void getAllProgress(Image*, qint64, qint64);
		void getAllCancel();
		void getAllPause();
		void getAllSkip();
		void getAllLogin();
		void getAllGetPages();
		void getAllFinished();
		void getAllFinishedLogin(Site *site, Site::LoginResult result);
		void getAllFinishedLogins();
		void _getAll();
		// Tabs
		int addTab(QString tag = "", bool background = false);
		int addPoolTab(int pool = 0, QString site = "");
		void addSearchTab(searchTab*, bool background = false);
		void updateTabTitle(searchTab*);
		void tabClosed(searchTab*);
		void currentTabChanged(int);
		void closeCurrentTab();
		void loadTag(QString tag, bool newTab = true);
		void loadTagTab(QString tag);
		void loadTagNoTab(QString tag);
		void linkHovered(QString tag);
		bool saveTabs(QString);
		bool loadTabs(QString);
		void updateTabs();
		void focusSearch();
		// Title
		void increaseDownloads();
		void decreaseDownloads();
		void updateDownloads();
		// Others
		void closeEvent(QCloseEvent*);
		void onFirstLoad();
		void advanced();
		void saveAdvanced(sourcesWindow*);
		void init();
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
		void saveImage(Image *img, QNetworkReply *reply = NULL, QString path = "", QString p = "", bool getAll = true);
		void setTags(QList<Tag> tags, searchTab *from = nullptr);
		void initialLoginsFinished();
		QIcon& getIcon(QString path);
		void setWiki(QString);

	protected:
		int getRowForSite(int site_id);
		void getAllGetImageIfNotBlacklisted(Image *img, int site_id);
		void getAllImageOk(Image *img, int site_id, bool del = true);
		QList<Site*> getSelectedSites();
		Site* getSelectedSiteOrDefault();

	private:
		Profile				m_profile;
		int					m_pagemax, m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllCount, m_getAllPageCount, m_getAllBeforeId, m_remainingPics, m_remainingSites, m_countPics, m_currentFav, m_currentFavCount, m_getAllLimit, m_downloads, m_waitForLogin;
		bool				m_allow, m_must_get_tags, m_loaded, m_getAll;
		QSettings			*m_settings;
		batchWindow			*m_progressdialog;
		QString				m_program, m_currLang, m_currentFavorite, m_link;
		QStringList			m_tags;
		QTranslator			m_translator;
		QDateTime			m_loadFavorite;
		QList<QStringList>	m_groupBatchs; // tags, page, perpage, max, blacklist, source, filename, location
		QList<Image*>		m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_images;
		QWidget				*m_currentTab;
		QList<searchTab*>	m_tabs;
		QList<tagTab*>		m_tagTabs;
		QList<poolTab*>		m_poolTabs;
		QList<bool>			m_selectedSources;
		favoritesTab		*m_favoritesTab;
		QList<Favorite>					&m_favorites;
		QMap<QString,QTime*>			m_downloadTime, m_downloadTimeLast;
		QList<QProgressBar*>			m_progressBars;
		QList<QMap<QString, QString>>	m_batchs;
		QSet<int>						m_batchDownloading;
		QStringList			m_lineFilename_completer, m_lineFolder_completer;
		QList<Downloader*>  m_downloaders, m_downloadersDone;
		QList<Site*>		m_getAllLogins;
		int					m_batchAutomaticRetries;
		bool				m_restore, m_showLog;
		QMap<QString, QIcon>	m_icons;
};

#endif // MAINWINDOW_H
