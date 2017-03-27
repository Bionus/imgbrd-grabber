#ifndef HEADER_ZOOMWINDOW
#define HEADER_ZOOMWINDOW

#include <QtGui>
#include <QtNetwork>
#include <QStackedWidget>
#include "ui/QAffiche.h"
#include "models/image.h"
#include "models/profile.h"
#include "reverse-search/reverse-search-engine.h"
#include "mainwindow.h"
#include "detailswindow.h"
#include "threads/image-loader.h"
#include "threads/image-loader-queue.h"



namespace Ui
{
	class zoomWindow;
}


class zoomWindow : public QWidget
{
	Q_OBJECT

	public:
		zoomWindow(QList<QSharedPointer<Image>> images, QSharedPointer<Image> image, Site *site, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent);
		~zoomWindow();
		void go();
		void load();

	public slots:
		void update(bool onlysize = false, bool force = false);
		void replyFinishedDetails();
		void replyFinishedZoom(QNetworkReply::NetworkError error = QNetworkReply::NoError, QString errorString = "");
		void display(const QPixmap &, int);
		void saveNQuit();
		void saveNQuitFav();
		void saveImage(bool fav = false);
		void saveImageFav();
		QStringList saveImageNow(bool fav = false);
		QString saveImageAs();
		void openUrl(QString);
		void openPool(QString);
		void openPoolId(Page*);
		void openSaveDir(bool fav = false);
		void openSaveDirFav();
		void linkHovered(QString);
		void contextMenu(QPoint);
		void openInNewTab();
		void openInNewWindow();
		void openInBrowser();
		void favorite();
		void setfavorite();
		void unfavorite();
		void viewitlater();
		void unviewitlater();
		void ignore();
		void unignore();
		void blacklist();
		void unblacklist();
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void colore();
		void urlChanged(QString, QString);
		void showDetails();
		void pendingUpdate();
		void updateButtonPlus();
		void openFile(bool now = false);
		void updateWindowTitle();

		// Context menus
		void imageContextMenu();
		void reverseImageSearch(int i);
		void copyImageFileToClipboard();
		void copyImageDataToClipboard();
		void copyTagToClipboard();
		void copyAllTagsToClipboard();

		// Full screen
		void toggleFullScreen();
		void fullScreen();
		void unfullScreen();
		void prepareNextSlide();
		void toggleSlideshow();

		// Navigation
		void load(QSharedPointer<Image> image);
		void next();
		void previous();

	protected:
		void closeEvent(QCloseEvent *);
		void resizeEvent(QResizeEvent *);
		void save(QString, QPushButton *);
		void showEvent(QShowEvent *);
		void mouseReleaseEvent(QMouseEvent *);
		void wheelEvent(QWheelEvent *);
		void draw();

	private:
		void showThumbnail();
		Qt::Alignment getAlignments(QString type);

	signals:
		void linkClicked(QString);
		void poolClicked(int, QString);
		void linkMiddleClicked(QString);
		void loadImage(const QByteArray &);
		void clearLoadQueue();

	private:
		mainWindow *m_parent;
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		QStringList &m_viewItLater;
		QStringList &m_ignore;
		QSettings *m_settings;
		Ui::zoomWindow *ui;
		detailsWindow *m_detailsWindow;
		QSharedPointer<Image> m_image;
		QMap<QString,QString> regex, m_details;
		Site *m_site;
		int timeout, m_mustSave;
		bool m_loaded, m_loadedImage, m_loadedDetails;
		QString id, m_url, tags, rating, score, user, format;
		QAffiche *m_labelTagsTop, *m_labelTagsLeft;
		QTimer *m_resizeTimer;
		QTime m_imageTime;
		QString link;
		bool m_finished;
		int m_size;
		QMap<QString,Site*> *m_sites;
		QString m_source;
		QString m_imagePath;
		QTime m_lastWheelEvent;

		QAffiche *m_fullScreen;
		QTimer m_slideshow;
		bool m_isFullscreen;
		bool m_isSlideshowRunning;

		QStackedWidget *m_stackedWidget;
		QAffiche *m_labelImage;
		QList<QSharedPointer<Image>> m_images;
		QSignalMapper *m_reverseSearchSignalMapper;
		QList<ReverseSearchEngine> m_reverseSearchEngines;

		// Display
		QPixmap m_displayImage;
		QMovie *m_displayMovie;

		// Threads
		QThread m_imageLoaderThread;
		QThread m_imageLoaderQueueThread;
		ImageLoader *m_imageLoader;
		ImageLoaderQueue *m_imageLoaderQueue;
};

#endif
