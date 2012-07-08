#ifndef HEADER_ZOOMWINDOW
#define HEADER_ZOOMWINDOW

#include <QtGui>
#include <QtNetwork>
#include "QAffiche.h"
#include "image.h"
#include "mainwindow.h"
#include "detailswindow.h"
#include "imagethread.h"



namespace Ui
{
	class zoomWindow;
}



class zoomWindow : public QDialog
{
	Q_OBJECT

	public:
		zoomWindow(Image *, QMap<QString,QString>, QMap<QString,QMap<QString,QString> > *sites, QWidget *parent = 0);
		void go();
		~zoomWindow();
		void load();
	
	public slots:
		void update(bool onlysize = false);
		void replyFinished(Image*);
        void replyFinishedZoom();
        void display(QPixmap, int);
		void display(QPixmap*, int);
        void display(QImage, int);
		void saveNQuit();
		void saveNQuitFav();
		QString saveImage(bool fav = false);
		QString saveImageFav();
		QString saveImageAs();
		void fullScreen();
		void openUrl(QString);
		void openPool(QString);
		void openPoolId(Page*);
		void openSaveDir(bool fav = false);
		void openSaveDirFav();
		void linkHovered(QString);
		void contextMenu(QPoint);
		void openInNewTab();
		void openInNewWindow();
		void favorite();
		void setfavorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
		void ignore();
		void unignore();
		void downloadProgress(qint64, qint64);
		void colore();
		void sslErrorHandler(QNetworkReply*, QList<QSslError>);
	
	protected:
		void closeEvent(QCloseEvent *);
		void resizeEvent(QResizeEvent *);
		void save(QString, QPushButton *);

	signals:
		void linkClicked(QString);
		void linkMiddleClicked(QString);

	private:
		mainWindow *m_parent;
		Ui::zoomWindow *ui;
		detailsWindow *m_detailsWindow;
		Image *m_image;
		QMap<QString,QString> regex, m_details, m_site;
		int timeout, loaded, oldsize, m_mustSave;
		QMap<QString, QStringList> details;
		QString id, m_url, tags, md5, rating, score, user, format;
		QAffiche *labelImage;
		QLabel *m_labelTags;
		QPixmap *image;
		QMovie *movie;
		QTimer *timer;
		QByteArray d;
		QNetworkAccessManager *m;
		QPushButton *buttonSave, *buttonSaveNQuit, *buttonSaveas, *m_buttonSaveNQuit;
		QString link, m_program;
		QNetworkReply *m_reply;
		const char* m_format;
		bool m_replyExists, m_finished, m_thread;
		QStringList m_favorites, m_viewItLater, m_ignore;
		QByteArray m_data;
		int m_size;
		QMap<QString,QMap<QString,QString> > *m_sites;
		QString m_source;
		ImageThread *m_th;
};

#endif
