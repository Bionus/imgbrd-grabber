#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "sourceswindow.h"
#include "batchwindow.h"
#include "QAffiche.h"
#include "QBouton.h"
#include "textedit.h"
#include "searchtab.h"



namespace Ui
{
    class mainWindow;
}

class mainWindow : public QMainWindow
{
    Q_OBJECT

	public:
		explicit mainWindow(QString, QStringList, QStringMap);
		~mainWindow();

	public slots:
		// Log
		void logShow();
		void logClear();
		// Menus
		void options();
		void optionsClosed();
		void aboutAuthor();
		void saveFolder();
		// Language
		void switchTranslator(QTranslator&, const QString&);
		void loadLanguage(const QString&, bool shutup = false);
		void changeEvent(QEvent*);
		// Favorites
		void loadFavorite(int);
		void favoriteProperties(int);
		void updateFavorites();
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
		void batchChange(int);
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const QStringList& values);
		void batchAddUnique(QStringMap);
		// Batch download
		void getAll();
		void getAllFinishedLoading(Page*);
		void getAllPerformTags(Image*);
		void getAllPerformImage();
		void getAllProgress(qint64, qint64);
		void getAllCancel();
		void _getAll();
		// Others
		void closeEvent(QCloseEvent*);
		void advanced();
		void saveAdvanced(sourcesWindow*);
		void init();
		// Tabs
		void addTab(QString tag = "");
		void addTabFavorite(int);
		void updateTabTitle(searchTab*);
		void closeCurrentTab();

	private:
		Ui::mainWindow *ui;
		QSettings *m_settings;
		QDateTime m_serverDate;
		int m_timezonedecay, m_getAllId, m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAllErrors, m_getAllCount, m_getAllPageCount, m_getAllBeforeId, m_remainingPics, m_remainingSites, m_countPics, m_currentFav, m_currentFavCount;
		QStringMap m_favorites, m_params;
		QString m_program;
		QStringList m_tags, m_assoc, m_gotMd5;
		QList<QStringList> m_groupBatchs;
		bool m_allow, m_must_get_tags;
		QList<QStringMap> m_details, m_batchs, m_allImages;
		QString m_currLang;
		QTranslator m_translator;
		QCalendarWidget *m_calendar;
		QStringMapMap m_sites;
		QList<Image*>				m_getAllImages;
		QList<Page*>				m_getAllPages;
		QMap<QString,QStringList>	m_getAllDetails;
		QList<QAffiche*>			m_favoritesCaptions;
		QList<QBouton*>				m_favoritesImages;
		QDateTime					m_loadFavorite;
		QString						m_currentFavorite;
		QList<QNetworkReply*>		m_replies;
		QList<QBouton*>				m_webPics;
		QList<QLabel*>				m_webSites;
		QMap<QString,int>			m_countPage;
		QProcess					*m_process;
		QNetworkReply				*m_getAllRequest;
		batchWindow					*m_progressdialog;
		QList<QBouton*>				m_mergeButtons;
		QList<searchTab*>			m_tabs;
		bool						m_loaded, m_getAllRequestExists;
		QList<Page*>				m_pages;
		QList<Image*>				m_images;
		int							m_pagemax;
		QList<bool>					m_selectedSources;
		QTime						*m_downloadTime;
};

#endif // MAINWINDOW_H
