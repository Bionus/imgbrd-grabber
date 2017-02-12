#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QSettings>
#include <QProcess>
#include <QTranslator>
#include <QSet>
#include <QTableWidgetItem>
#include "batch/batchwindow.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"
#include "ui/textedit.h"
#include "tabs/search-tab.h"
#include "tabs/pool-tab.h"
#include "tabs/tag-tab.h"
#include "tabs/favorites-tab.h"
#include "models/site.h"
#include "models/profile.h"
#include "downloader/downloader.h"
#include "updater/update-dialog.h"
#include "downloader/download-query-image.h"



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
		explicit mainWindow(Profile *profile, QString program, QStringList tags, QMap<QString,QString> params);
		~mainWindow();
		Ui::mainWindow *ui;

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
		QList<int> getSelectedRows(QList<QTableWidgetItem*> selected);
		void batchMove(int);
		void batchMoveUp();
		void batchMoveDown();
		void batchSel();
		void batchChange(int);
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
		void getAllPerformTags();
		void getAllPerformImage(QNetworkReply::NetworkError error, QString errorString);
		void getAllProgress(qint64, qint64);
		void getAllCancel();
		void getAllPause();
		void getAllSkip();
		void getAllLogin();
		void getAllGetPages();
		void getAllFinished();
		void getAllFinishedLogin(Site *site, Site::LoginResult result);
		void getAllFinishedLogins();
		bool needExactTags(QSettings *settings);
		void _getAll();
		// Tabs
		int addTab(QString tag = "", bool background = false);
		int addPoolTab(int pool = 0, QString site = "");
		void addSearchTab(searchTab*, bool background = false);
		void updateTabTitle(searchTab*);
		void tabClosed(searchTab*);
		void currentTabChanged(int);
		void closeCurrentTab();
		bool saveTabs(QString);
		bool loadTabs(QString);
		void updateTabs();
		void focusSearch();
		// Title
		void increaseDownloads();
		void decreaseDownloads();
		void updateDownloads();
		// Tag list
		void loadTag(QString tag, bool newTab = true, bool background = true);
		void loadTagTab(QString tag);
		void loadTagNoTab(QString tag);
		void linkHovered(QString tag);
		void contextMenu();
		void openInNewTab();
		void openInNewWindow();
		void favorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
		// Others
		void closeEvent(QCloseEvent*);
		void onFirstLoad();
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
		void saveImage(QSharedPointer<Image> img, QString path = "", QString p = "", bool getAll = true);
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
		UpdateDialog		*m_updateDialog;
		QList<Favorite>		&m_favorites;
		int					m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllLimit, m_downloads, m_waitForLogin;
		bool				m_allow, m_mustGetTags, m_loaded, m_getAll;
		QSettings			*m_settings;
		batchWindow			*m_progressdialog;
		QString				m_program, m_currLang, m_link;
		QStringList			m_tags;
		QTranslator			m_translator;
		QList<DownloadQueryGroup>		m_groupBatchs;
		QList<QSharedPointer<Image>>	m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_images, m_getAllSkippedImages;
		QWidget				*m_currentTab;
		QList<searchTab*>	m_tabs;
		QList<tagTab*>		m_tagTabs;
		QList<poolTab*>		m_poolTabs;
		QList<bool>			m_selectedSources;
		favoritesTab		*m_favoritesTab;
		QMap<QString,QTime*>			m_downloadTime, m_downloadTimeLast;
		QList<QProgressBar*>			m_progressBars;
		QList<DownloadQueryImage>		m_batchs;
		QSet<int>						m_batchDownloading;
		QStringList			m_lineFilename_completer, m_lineFolder_completer;
		QList<Downloader*>  m_downloaders, m_downloadersDone;
		QList<Site*>		m_getAllLogins;
		int					m_batchAutomaticRetries;
		bool				m_restore, m_showLog;
		QMap<QString, QIcon>	m_icons;
		QMap<QString, Site*>	m_sites;
};

#endif // MAINWINDOW_H
