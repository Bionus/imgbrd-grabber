#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QProcess>
#include <QTranslator>
#include "sourceswindow.h"
#include "batchwindow.h"
#include "QAffiche.h"
#include "QBouton.h"
#include "textedit.h"
#include "searchtab.h"
#include "ui_searchtab.h"



namespace Ui
{
	class mainWindow;
}



class searchTab;

class mainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit mainWindow(QString, QStringList, QMap<QString,QString>);
		~mainWindow();
		QMap<QString,QMap<QString,QString> > m_sites;
		Ui::mainWindow *ui;

	public slots:
		// Log
		void logShow();
		void logClear();
		void logOpen();
		// Menus
		void options();
		void optionsClosed();
		void aboutAuthor();
		void saveFolder();
		void blacklistFix();
		void emptyDirsFix();
		// Language
		void switchTranslator(QTranslator&, const QString&);
		void loadLanguage(const QString&, bool shutup = false);
		void changeEvent(QEvent*);
		// Favorites
		void loadFavorite(int);
		void favoriteProperties(int);
		void updateFavorites(bool dock = false);
		void updateFavoritesDock();
		void updateKeepForLater();
		void viewed();
		void setFavoriteViewed(QString);
		void favoritesBack();
		void checkFavorites();
		void loadNextFavorite();
		// Download
		void web(QString tags = "");
		void finishedLoading(Page*);
		void finishedLoadingPreview(Image*);
		void webZoom(int);
		// Batch download management
		void batchClear();
		void batchClearSel();
		void batchSel();
		void batchChange(int);
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const QStringList& values);
		void batchAddUnique(QMap<QString,QString>);
		// Batch download
		void getAll(bool all = true);
		void getAllImages();
		void getAllFinishedLoading(Page*);
		void getAllPerformTags(Image*);
		void getAllPerformImage(Image*);
		void getAllProgress(Image*, qint64, qint64);
		void getAllCancel();
		void _getAll();
		// Others
		void closeEvent(QCloseEvent*);
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
		// Tabs
		int addTab(QString tag = "");
		void addTabFavorite(int);
		void updateTabTitle(searchTab*);
		void tabClosed(searchTab *);
		void currentTabChanged(int);
		void closeCurrentTab();
		void loadTag(QString tag);
		bool saveTabs(QString);
		bool loadTabs(QString);
		void updateTabs();

	private:
		int					m_pagemax, m_timezonedecay, m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAll404s, m_getAllErrors, m_getAllCount, m_getAllPageCount, m_getAllBeforeId, m_remainingPics, m_remainingSites, m_countPics, m_currentFav, m_currentFavCount, m_getAllLimit;
		bool				m_allow, m_must_get_tags, m_loaded, m_getAllRequestExists, m_getAll;
		QSettings			*m_settings;
		QProcess			*m_process;
		QNetworkReply		*m_getAllRequest;
		batchWindow			*m_progressdialog;
		QString				m_program, m_currLang, m_currentFavorite;
		QStringList			m_tags, m_assoc, m_gotMd5;
		QTranslator			m_translator;
		QDateTime			m_serverDate, m_loadFavorite;
		QMap<QString,int>	m_countPage;
		QList<QStringList>	m_groupBatchs;
		QList<Image*>		m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_images;
		QList<Page*>		m_getAllPages, m_pages;
		QList<QAffiche*>	m_favoritesCaptions;
		QList<QBouton*>		m_favoritesImages, m_mergeButtons, m_webPics;
		QList<searchTab*>	m_tabs;
		QList<bool>			m_selectedSources;
		QList<QLabel*>		m_webSites;
		QMap<QString,double>			m_getAllDownloadingSpeeds;
		QMap<QString,QString>			m_favorites, m_params;
		QMap<QString,QStringList>		m_getAllDetails;
		QMap<QString,QTime*>			m_downloadTime, m_downloadTimeLast;
		QList<QProgressBar*>			m_progressBars;
		QList<QNetworkReply*>			m_replies;
		QList<QMap<QString,QString> >	m_details, m_batchs;
		QSet<int>						m_batchDownloading;
};

#endif // MAINWINDOW_H
