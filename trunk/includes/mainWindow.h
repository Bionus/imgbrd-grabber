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
		mainWindow(QString, QStringList);
		void setTags(QString);
	
	public slots:
		void help();
		void aboutAuthor();
		void replyFinishedVersion(QNetworkReply*);
		void replyFinished(QNetworkReply*);
		void replyFinishedPic(QNetworkReply*);
		void options();
		void webUpdate();
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
		void logClear();
		void loadFavorites();
		void updateBatchGroups(int, int);
		void addGroup();
		void addUnique();
		void batchAddGroup(const QStringList& values);
		void batchAddUnique(QMap<QString,QString>);

	protected:
		void closeEvent(QCloseEvent*);
		void _getAll();
	
	private:
		bool loaded, allow, changed, m_must_get_tags;
		int ch, updating, filesUpdates, getAllId, getAllDownloaded, getAllExists, getAllIgnored, getAllErrors, pagemax, columns, limit;
		QStringList paths, sources, assoc;
		QString path;
		QList<QMap<QString, QString> > details, batchs, allImages;
		QList<QLabel *> webSites;
		QList<bool> selected;
		QString source, artist, copyright, character;
		QRadioButton *radio1, *radio2;
		QPixmap pix;
		QAffiche *image;
		QGridLayout *web;
		QList<QBouton *> webPics;
		QStringList files;
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
		QPushButton *ok, *adv, *gA, *clearBatch, *showBatch, *getBatch;
		QMenu *menuOptions, *menuAide;
		QAction *actionOptions, *actionAboutAuthor, *actionAboutQt, *actionHelp;
		QTranslator m_translator, m_translatorQt;
		QString m_currLang, m_langPath;
		QMap<QString,int> favorites;
		QTableWidget *batchTableGroups, *batchTableUniques;
		int batchGroups, batchUniques, getAllCount;
		QList<QStringList> groupBatchs;
		QString m_program;
		QStringList m_params;
		QDateTime m_serverDate;
		QPushButton *m_logClear;
		QTabWidget *m_tabs;
		QWidget *m_tabExplore, *m_tabBatch, *m_tabLog;
		QProcess *m_process;
		QMap<QDateTime,QString> *m_log;
};

#endif
