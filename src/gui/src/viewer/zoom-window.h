#ifndef ZOOM_WINDOW_H
#define ZOOM_WINDOW_H

#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QtNetwork>
#include "downloader/image-save-result.h"
#include "models/favorite.h"
#include "models/image.h"


namespace Ui
{
	class ZoomWindow;

	class ButtonSettings;

	// Button type masks:

	constexpr unsigned short IsUtilityButton		=          0b00000001 ;	// Nib0.0, bit 0.
		constexpr unsigned short IsButtonDetails	=  0b0000000100000001 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonOpen		=  0b0000010000000001 ;	// Nib2.2, bit10.

	constexpr unsigned short IsNavButton			=          0b00000010 ;	// Nib0.1, bit 1.
		//constexpr unsigned short IsQuit		=  0b0000000100000010 ;	// Nib2.0, bit 8.
		constexpr unsigned short IsButtonPrev		=  0b0000001000000010 ;	// Nib2.1, bit 9.
		constexpr unsigned short IsButtonNext		=  0b0000010000000010 ;	// Nib2.2, bit 10.

	constexpr unsigned short IsSavingButton			=          0b00000100 ;	// Nib0.2, bit 2.
		constexpr unsigned short IsButtonSave		=  0b0000000100000100 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonSaveAs		=  0b0000001000000100 ;	// Nib2.1, bit9.
		constexpr unsigned short IsButtonSaveNQuit	=  0b0000010000000100 ;	// Nib2.2, bit10.

	constexpr unsigned short IsFavoriteButton		=          0b00001000 ;	// Nib0.3, bit 3.
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
			Linked,
			ExistsMd5,
			ExistsDisk,
			Delete
		};
		enum PendingAction
		{
			PendingNothing,
			PendingSave,
			PendingSaveFav,
			PendingSaveAs,
			PendingOpen,
		};

		ZoomWindow(QList<QSharedPointer<Image>> images, const QSharedPointer<Image> &image, Site *site, Profile *profile, MainWindow *parent, SearchTab *tab);
		~ZoomWindow() override;
		void go();
		void load(bool force = false);

	public slots:
		void update(bool onlySize = false, bool force = false);
		void replyFinishedDetails();
		void replyFinishedZoom(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result);
		void display(const QPixmap &, int);
		void saveNQuit();
		void saveNQuitFav();
		void saveImage(bool fav = false);
		void saveImageFav();
		void saveImageNow();
		void saveImageNowSaved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result);
		void saveImageAs();
		void openUrl(const QString &);
		void openPool(const QString &);
		void openPoolId(Page*);
		void openSaveDir(bool fav = false);
		void openSaveDirFav();
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
		void setButtonState(bool fav, SaveButtonState state);
		void reuse(const QList<QSharedPointer<Image>> &images, const QSharedPointer<Image> &image, Site *site);

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
		void load(const QSharedPointer<Image> &image);
		void next();
		void previous();

	protected:
		void closeEvent(QCloseEvent *) override;
		void resizeEvent(QResizeEvent *) override;
		void resizeButtons();
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
		Ui::ZoomWindow *ui;
		DetailsWindow *m_detailsWindow;
		QSharedPointer<Image> m_image;
		Site *m_site;
		int m_timeout;
		PendingAction m_pendingAction;
		bool m_pendingClose;
		bool m_tooBig, m_loadedImage, m_loadedDetails;
		QAffiche *m_labelTagsTop, *m_labelTagsLeft;
		QTimer *m_resizeTimer;
		QTime m_imageTime;
		QString m_link;
		bool m_finished;
		int m_size;
		QString m_source;
		QString m_imagePath;
		QTime m_lastWheelEvent;
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
		bool hasShelf = false;
		bool hasDrawer = false;
		QWidget *scaleRef = nullptr;	// For resizeButtons().
		short shelfDrawerDiff = 0;	// For resizeButtons().
		std::string buttonSaveText;
		std::string buttonSaveFavText;
		bool hasButtonSave = false;
		bool hasButtonSaveNQuit = false;
		bool hasButtonSaveFav = false;
		bool hasButtonSaveNQuitFav = false;

		// Threads
		QThread m_imageLoaderThread;
		QThread m_imageLoaderQueueThread;
		ImageLoader *m_imageLoader;
		ImageLoaderQueue *m_imageLoaderQueue;
};

class ButtonSettings
{
	public:
		enum ButtonWindow
		{
			Zoom
		};

		ButtonWindow onWindow;
		//unsigned int mask = 0;
		unsigned short type;
		unsigned short position;	// Based on horizontal placement of the set bit. Left is top for vertical layouts.
		QString text;
		bool isEnabled;
		bool isInDrawer;

	bool operator < (const ButtonSettings& str) const {return (type < str.type);}	// https://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-objects

	// https://stackoverflow.com/questions/37333084/how-to-save-custom-type-to-qsettings
	friend QDataStream & operator << (QDataStream &arch, const ButtonSettings & object)
	{
		arch << object.type;
		arch << object.position;
		arch << object.text;
		arch << object.isEnabled;
		arch << object.isInDrawer;
		return arch;
	}
	friend QDataStream & operator >> (QDataStream &arch, ButtonSettings & object)
	{
		arch >> object.type;
		arch >> object.position;
		arch >> object.text;
		arch >> object.isEnabled;
		arch >> object.isInDrawer;
		return arch;
	}
};

Q_DECLARE_METATYPE(ButtonSettings)

#endif // ZOOM_WINDOW_H
