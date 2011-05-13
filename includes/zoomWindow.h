#ifndef HEADER_ZOOMWINDOW
#define HEADER_ZOOMWINDOW

#include <QtGui>
#include <QtNetwork>
#include "QAffiche.h"
#include "mainWindow.h"



class zoomWindow : public QWidget
{
    Q_OBJECT

	public:
		zoomWindow(QString, QString, QStringList, QString, QString, QString, QString, QString, QString, QString, mainWindow *);
		void load();
	
	public slots:
		void update(bool onlysize = false);
		void replyFinished(QNetworkReply*);
		void replyFinishedZoom(QNetworkReply*);
		void rR(qint64 a = -1, qint64 b = -1);
		void saveNQuit();
		QString saveImage();
		QString saveImageAs();
		void fullScreen();
		void openUrl(QString);
		void openSaveDir();
		void linkHovered(QString url);
		void contextMenu();
		void openInNewWindow();
		void favorite();
		void unfavorite();
	
	protected:
		void closeEvent(QCloseEvent *);
		void resizeEvent(QResizeEvent *);
		void save(QString, QPushButton *);
		QString getSavePath();

	private:
		QStringList regex;
		int timeout, loaded, oldsize;
		QMap<QString, QStringList> details;
		QString site, id, url, tags, md5, rating, score, user, format;
		QAffiche *labelImage;
		QLabel *labelTags;
		QPixmap image;
		QTimer *timer;
		QNetworkReply *r;
		QByteArray d;
		QNetworkAccessManager *m;
		QPushButton *buttonSave, *buttonSaveNQuit, *buttonSaveas, *m_buttonSaveNQuit;
		mainWindow *parent;
		QString link, m_program;
		QStringList favorites;
		QNetworkReply *m_reply;
		const char* m_format;
		bool m_mustSave;
};

#endif
