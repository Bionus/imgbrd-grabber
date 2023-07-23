#ifndef VIEWER_WINDOW_H
#define VIEWER_WINDOW_H

#include <QElapsedTimer>
#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QtNetwork>
#include <unordered_map>
#include <vector>
#include "custom-buttons.h"
#include "downloader/image-save-result.h"
#include "models/favorite.h"
#include "models/image.h"
#include "viewer/viewer-window-buttons.h"

using SaveButtonState = ViewerWindowButtons::SaveState;


namespace Ui
{
	class ViewerWindow;
}


class GifPlayer;
class QAffiche;
class Profile;
class MainWindow;
class DetailsWindow;
class ImageDownloader;
class ImageLoader;
class ImageLoaderQueue;
class SearchTab;
class VideoPlayer;

class ViewerWindow : public QWidget
{
	Q_OBJECT

	public:
		enum PendingAction
		{
			PendingNothing,
			PendingSave,
			PendingSaveFav,
			PendingSaveAs,
			PendingOpen,
		};

		ViewerWindow(QList<QSharedPointer<Image>> images, const QSharedPointer<Image> &image, Site *site, Profile *profile, MainWindow *parent, SearchTab *tab);
		~ViewerWindow() override;
		void go();
		void load(bool force = false);

	public slots:
		void update(bool onlySize = false, bool force = false);
		void replyFinishedDetails();
		void replyFinishedImage(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result);
		void display(const QPixmap &, int);
		void saveNQuit(bool fav = false);
		void saveImage(bool fav = false);
		void saveImageNow();
		void saveImageNowSaved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result);
		void saveImageAs();
		void openUrl(const QString &);
		void openPool(const QString &);
		void openPoolId(Page*);
		void openSaveDir(bool fav = false);
		void linkHovered(const QString &);
		void contextMenu(const QPoint &pos);
		void openInNewTab();
		void setfavorite();
		void downloadProgress(QSharedPointer<Image> img, qint64 bytesReceived, qint64 bytesTotal);
		void colore();
		void showDetails();
		void pendingUpdate();
		void updateButtonPlus();
		void openFile(bool now = false);
		void updateWindowTitle();
		void showLoadingError(const QString &message);
		void setButtonState(bool fav, ViewerWindowButtons::SaveState state);
		void reuse(const QList<QSharedPointer<Image>> &images, const QSharedPointer<Image> &image, Site *site);

		// Context menus
		void imageContextMenu();
		void reloadImage();
		void copyImageFileToClipboard();
		void copyImageDataToClipboard();
		void copyImageLinkToClipboard();

		// Full screen
		void toggleFullScreen();
		void fullScreen();
		void unfullScreen();
		void prepareNextSlide();
		void toggleSlideshow();

		// Navigation
		void load(const QSharedPointer<Image> &image);
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
		void configureButtons();
		void showThumbnail();
		int firstNonBlacklisted(int direction);
		Qt::Alignment getAlignments(const QString &type);

	signals:
		void linkClicked(const QString &);
		void poolClicked(int, const QString &);
		void linkMiddleClicked(const QString &);
		void loadImage(const QByteArray &);
		void clearLoadQueue();

	private:
		MainWindow *m_parent;
		QPointer<SearchTab> m_tab;
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		QStringList &m_viewItLater;
		QStringList &m_ignore;
		QSettings *m_settings;
		Ui::ViewerWindow *ui;
		DetailsWindow *m_detailsWindow;
		QSharedPointer<Image> m_image;
		Site *m_site;
		int m_timeout;
		PendingAction m_pendingAction;
		bool m_pendingClose;
		bool m_tooBig, m_loadedImage, m_loadedDetails;
		QAffiche *m_labelTagsTop, *m_labelTagsLeft;
		QTimer m_resizeTimer;
		QElapsedTimer m_imageTime;
		QString m_link;
		bool m_finished;
		int m_size;
		QString m_source;
		QString m_imagePath;
		QElapsedTimer m_lastWheelEvent;
		QString m_saveAsPending;

		QWidget *m_fullScreen = nullptr;
		QTimer m_slideshow;
		bool m_isFullscreen;
		bool m_isSlideshowRunning;

		QStackedWidget *m_stackedWidget;
		QAffiche *m_labelImage;
		QList<QSharedPointer<Image>> m_images;
		SaveButtonState m_saveButtonState, m_saveButtonStateFav;

		QMap<QSharedPointer<Image>, ImageDownloader*> m_imageDownloaders;

		// Display
		QString m_isAnimated;
		QPixmap m_displayImage;
		bool m_labelImageScaled;
		GifPlayer *m_gifPlayer = nullptr;
		VideoPlayer *m_videoPlayer = nullptr;

		// Buttons
		std::unordered_map<QString, ButtonInstance> m_buttons;
		std::vector<QPushButton*> m_drawerButtons;

		// Threads
		QThread m_imageLoaderThread;
		QThread m_imageLoaderQueueThread;
		ImageLoader *m_imageLoader;
		ImageLoaderQueue *m_imageLoaderQueue;
};

#endif // VIEWER_WINDOW_H
