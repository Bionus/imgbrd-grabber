#ifndef HEADER_ZOOMWINDOW
#define HEADER_ZOOMWINDOW

#include <QtGui>
#include <QtNetwork>
#include "QAffiche.h"
#include "mainWindow.h"
#include "detailswindow.h"



namespace Ui
{
	class zoomWindow;
}



class zoomWindow : public QWidget
{
    Q_OBJECT

	public:
		zoomWindow(QString, QString, QStringList, QString, QString, QString, QString, QString, QString, QString, mainWindow *);
		~zoomWindow();
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
		void setfavorite();
		void unfavorite();
	
	protected:
		void closeEvent(QCloseEvent *);
		void resizeEvent(QResizeEvent *);
		void save(QString, QPushButton *);
		QString getSavePath();

	private:
		mainWindow *m_parent;
		Ui::zoomWindow *ui;
		detailsWindow *m_detailsWindow;
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
		QString link, m_program;
		QNetworkReply *m_reply;
		const char* m_format;
		bool m_mustSave, m_replyExists;
		QStringList favorites;
};

#endif
