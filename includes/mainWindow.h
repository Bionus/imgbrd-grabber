#ifndef HEADER_MAINWINDOW
#define HEADER_MAINWINDOW

#include <QtNetwork>
#include <QtGui>
#include "QAffiche.h"
#include "QBouton.h"
#include "textedit.h"
#include "advancedWindow.h"



class mainWindow : public QMainWindow
{
    Q_OBJECT

	public:
		mainWindow(QString, QStringList, QMap<QString,QString>);
		void setTags(QString);
	
	public slots:
		void help();
		void aboutAuthor();
		void replyFinishedVersion(QNetworkReply*);
		void replyFinished(QNetworkReply*);
		void replyFinishedPic(QNetworkReply*);
		void options();
		void webUpdate();
		void web(QString tags = "");
		void webZoom(int);
		void batchChange(int);
		void advanced();
		void getAll();
		void getAllPerformTags(QNetworkReply*);
		void getAllPerformImage(QNetworkReply*);
		void getAllCancel();
		void saveAdvanced(advancedWindow *);
		void openUrl(QString);
		void retranslateStrings();
		void switchTranslator(QTranslator& translator, const QString& filename);
		void loadLanguage(const QString& rLanguage);
		void changeEvent(QEvent*);
		void batchClear();
		void getPage();
		void getAllSource(QNetworkReply*);
		void log(QString);
		void logUpdate(QString);
		void logShow();
		void logClear();
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const QStringList& values);
		void batchAddUnique(QMap<QString,QString>);
		void loadFavorite(int);
		void favoriteProperties(int);
		void updateFavorites();
		void viewed();
		void setFavoriteViewed(QString);
		void favoritesBack();

	protected:
		void closeEvent(QCloseEvent*);
		void _getAll();
	
	private:
		bool loaded, allow, changed, m_must_get_tags;
		int ch, updating, filesUpdates, getAllId, getAllDownloaded, getAllExists, getAllIgnored, getAllErrors, pagemax, columns, limit, batchGroups, batchUniques, getAllCount;
		QStringList paths, sources, assoc, files, m_tags;
		QString path, source, artist, copyright, character, m_currLang, m_langPath, m_program;
		QList<QMap<QString, QString> > details, batchs, allImages;
		QList<QLabel *> webSites;
		QList<bool> selected;
		QRadioButton *radio1, *radio2;
		QPixmap pix;
		QAffiche *image;
		QGridLayout *m_web, *m_layoutFavorites, *m_webFavorites;
		QList<QBouton *> webPics;
		TextEdit *search;
		QSpinBox *page;
		QDateEdit *m_date;
		QComboBox *comboSources, *artists, *copyrights, *characters;
		QMdiArea *area;
		QStatusBar *status;
		QLabel *statusCount, *statusPath, *statusSize, *_logLabel;
		QList<QNetworkReply *> replies;
		QProgressDialog *progressdialog;
		QNetworkReply *getAllRequest;
		QMap<QString, QStringList> getAllDetails, sites;
		QPushButton *ok, *adv, *m_favoritesButtonAdvanced, *m_favoritesButtonViewed, *m_favoritesButtonBack, *gA, *clearBatch, *showBatch, *getBatch, *m_logClear, *m_buttonOpenCalendar;
		QMenu *menuOptions, *menuAide;
		QAction *actionOptions, *actionAboutAuthor, *actionAboutQt, *actionHelp;
		QTranslator m_translator, m_translatorQt;
		QMap<QString,QString> m_favorites, m_params;
		QTableWidget *batchTableGroups, *batchTableUniques;
		QList<QStringList> groupBatchs;
		QDateTime m_serverDate;
		QTabWidget *m_tabs;
		QWidget *m_tabExplore, *m_tabBatch, *m_tabLog, *m_tabFavorites;
		QProcess *m_process;
		QMap<QDateTime,QString> *m_log;
		QSettings *m_settings;
		QDateTime m_loadFavorite;
		QVBoxLayout *m_favoritesLayout;
		QCalendarWidget *m_calendar;
		QString m_currentFavorite;
		QList<QAffiche *> m_favoritesCaptions;
		QList<QBouton *> m_favoritesImages;
};

#endif
