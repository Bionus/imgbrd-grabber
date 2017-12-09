#ifndef ZOOM_WINDOW_H
#define ZOOM_WINDOW_H

#include <QPushButton>
#include <QStackedWidget>
#include <QtGui>
#include <QtNetwork>
#include "models/favorite.h"
#include "models/image.h"


namespace Ui
{
	class ZoomWindow;
}


class QAffiche;
class Profile;
class mainWindow;
class DetailsWindow;
class ImageLoader;
class ImageLoaderQueue;

class ZoomWindow : public QWidget
{
	Q_OBJECT

	public:
		enum SaveButtonState
		{
			Save,
			Saving,
			Saved,
			Copied,
			Moved,
			ExistsMd5,
			ExistsDisk,
			Delete
		};

		ZoomWindow(QList<QSharedPointer<Image>> images, QSharedPointer<Image> image, Site *site, Profile *profile, mainWindow *parent);
		~ZoomWindow() override;
		void go();
		void load(bool force = false);

	public slots:
		void update(bool onlySize = false, bool force = false);
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
		void setfavorite();
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void colore();
		void urlChanged(QString, QString);
		void showDetails();
		void pendingUpdate();
		void updateButtonPlus();
		void openFile(bool now = false);
		void updateWindowTitle();
		void showLoadingError(QString error);
		void setButtonState(bool fav, SaveButtonState state);

		// Context menus
		void imageContextMenu();
		void reloadImage();
		void copyImageFileToClipboard();
		void copyImageDataToClipboard();

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
		void closeEvent(QCloseEvent *) override;
		void resizeEvent(QResizeEvent *) override;
		void showEvent(QShowEvent *) override;
		void mouseReleaseEvent(QMouseEvent *) override;
		void wheelEvent(QWheelEvent *) override;
		void draw();

	private:
		void showThumbnail();
		int firstNonBlacklisted(int direction);
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
		Ui::ZoomWindow *ui;
		DetailsWindow *m_detailsWindow;
		QSharedPointer<Image> m_image;
		QMap<QString, QString> regex, m_details;
		Site *m_site;
		int m_timeout, m_mustSave;
		bool m_tooBig, m_loadedImage, m_loadedDetails;
		QString id, m_url, m_saveUrl, rating, score, user;
		QAffiche *m_labelTagsTop, *m_labelTagsLeft;
		QTimer *m_resizeTimer;
		QTime m_imageTime;
		QString m_link;
		bool m_finished;
		int m_size;
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
		SaveButtonState m_saveButonState, m_saveButonStateFav;

		// Display
		QString m_isAnimated;
		QPixmap m_displayImage;
		QMovie *m_displayMovie;
		bool m_labelImageScaled;

		// Threads
		QThread m_imageLoaderThread;
		QThread m_imageLoaderQueueThread;
		ImageLoader *m_imageLoader;
		ImageLoaderQueue *m_imageLoaderQueue;
};

#endif // ZOOM_WINDOW_H
