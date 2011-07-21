#ifndef HEADER_ZOOMWINDOW
#define HEADER_ZOOMWINDOW

#include <QtGui>
#include <QtNetwork>
#include "QAffiche.h"
#include "Image.h"
#include "mainwindow.h"
#include "detailswindow.h"



namespace Ui
{
	class zoomWindow;
}



class zoomWindow : public QWidget
{
    Q_OBJECT

	public:
		zoomWindow(Image *, QStringMap);
		~zoomWindow();
		void load();
	
	public slots:
		void update(bool onlysize = false);
		void replyFinished(Image*);
		void replyFinishedZoom(QNetworkReply*);
		void rR(qint64 a = -1, qint64 b = -1);
		void saveNQuit();
		QString saveImage();
		QString saveImageAs();
		void fullScreen();
		void openUrl(QString);
		void openSaveDir();
		void linkHovered(QString);
		void contextMenu(QPoint);
		void openInNewWindow();
		void favorite();
		void setfavorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
	
	protected:
		void closeEvent(QCloseEvent *);
		void resizeEvent(QResizeEvent *);
		void save(QString, QPushButton *);
		QString getSavePath();

	signals:
		void linkClicked(QString);
		void linkMiddleClicked(QString);

	private:
		mainWindow *m_parent;
		Ui::zoomWindow *ui;
		detailsWindow *m_detailsWindow;
		Image *m_image;
		QStringMap regex, m_details, m_site;
		int timeout, loaded, oldsize, m_mustSave;
		QMap<QString, QStringList> details;
		QString id, m_url, tags, md5, rating, score, user, format;
		QAffiche *labelImage;
		QLabel *m_labelTags;
		QPixmap image;
		QMovie *movie;
		QTimer *timer;
		QNetworkReply *r;
		QByteArray d;
		QNetworkAccessManager *m;
		QPushButton *buttonSave, *buttonSaveNQuit, *buttonSaveas, *m_buttonSaveNQuit;
		QString link, m_program;
		QNetworkReply *m_reply;
		const char* m_format;
		bool m_replyExists, m_finished;
		QStringList m_favorites, m_viewItLater;
};

#endif
