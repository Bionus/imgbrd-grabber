#ifndef ZOOM_WINDOW_H
#define ZOOM_WINDOW_H

#include <QPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QtNetwork>
#include "downloader/image-save-result.h"
#include "models/favorite.h"
#include "models/image.h"
#include "custom-buttons.h"


namespace Ui
{
	class ZoomWindow;

	enum SaveButtonState : unsigned short
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

	const ButtonState DefaultPrevState (0, QStringLiteral("<"), QStringLiteral("Previous search result"));
	const ButtonState DefaultNextState (0, QStringLiteral(">"), QStringLiteral("Next search result"));
	const ButtonState DefaultDetailsState (0, QStringLiteral("Details"), QStringLiteral("Media details"));
	const ButtonState DefaultSaveAsState (0, QStringLiteral("Save as..."), QStringLiteral("Save to irregular location"));

	const ButtonState DefaultSaveStateSave (Ui::SaveButtonState::Save, QStringLiteral("Save"), QStringLiteral("Save to usual location"));
	const ButtonState DefaultSaveStateSaving (Ui::SaveButtonState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveStateSaved (Ui::SaveButtonState::Saved, QStringLiteral("Saved..."), QStringLiteral(""));
	const ButtonState DefaultSaveStateCopied (Ui::SaveButtonState::Copied, QStringLiteral("Copied!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateMoved (Ui::SaveButtonState::Moved, QStringLiteral("Moved!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateLinked (Ui::SaveButtonState::Linked, QStringLiteral("Link created!"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsMd5 (Ui::SaveButtonState::ExistsMd5, QStringLiteral("MD5 already exists"), QStringLiteral(""));
	const ButtonState DefaultSaveStateExistsDisk (Ui::SaveButtonState::ExistsDisk, QStringLiteral("Already saved"), QStringLiteral(""));
	const ButtonState DefaultSaveStateDelete (Ui::SaveButtonState::Delete, QStringLiteral("Delete"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitStateSave (Ui::SaveButtonState::Save, QStringLiteral("Save & close"), QStringLiteral("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitStateSaving (Ui::SaveButtonState::Saving, QStringLiteral("Saving..."), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitStateClose (2, QStringLiteral("Close"), QStringLiteral(""));	// Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenState (0, QStringLiteral("Open"), QStringLiteral("Open usual save location in new window"));

	const ButtonState DefaultSaveFavStateSave (Ui::SaveButtonState::Save, QStringLiteral("Save (fav)"), QStringLiteral("Save to favourite location"));
	const ButtonState DefaultSaveFavStateSaving (Ui::SaveButtonState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateSaved (Ui::SaveButtonState::Saved, QStringLiteral("Saved... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateCopied (Ui::SaveButtonState::Copied, QStringLiteral("Copied! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateMoved (Ui::SaveButtonState::Moved, QStringLiteral("Moved! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateLinked (Ui::SaveButtonState::Linked, QStringLiteral("Link created! (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsMd5 (Ui::SaveButtonState::ExistsMd5, QStringLiteral("MD5 already exists (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateExistsDisk (Ui::SaveButtonState::ExistsDisk, QStringLiteral("Already saved (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveFavStateDelete (Ui::SaveButtonState::Delete, QStringLiteral("Delete (fav)"), QStringLiteral(""));

	const ButtonState DefaultSaveNQuitFavStateSave (Ui::SaveButtonState::Save, QStringLiteral("Save (fav)"), QStringLiteral("Save to usual location and close window"));
	const ButtonState DefaultSaveNQuitFavStateSaving (Ui::SaveButtonState::Saving, QStringLiteral("Saving... (fav)"), QStringLiteral(""));
	const ButtonState DefaultSaveNQuitFavStateClose (2, QStringLiteral("Close (fav)"), QStringLiteral(""));	// Consider adding this to SaveButtonState.

	const ButtonState DefaultOpenFavState (0, QStringLiteral("Open (fav)"), QStringLiteral("Open favourite save location in new window"));
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
		enum SaveButtonState : unsigned short
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
		void saveImageNotFav();	// This is stupid.
		void saveImageNow();
		void saveImageNowSaved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result);
		void saveImageAs();
		void openUrl(const QString &);
		void openPool(const QString &);
		void openPoolId(Page*);
		void openSaveDir(bool fav = false);
		void openSaveDirFav();
		void openSaveDirNotFav();	// This is stupid.
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
		void showEvent(QShowEvent *) override;
		void mouseReleaseEvent(QMouseEvent *) override;
		void wheelEvent(QWheelEvent *) override;
		void draw();

	private:
		void configureButtons();
		//void resizeButtons();
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
		std::unordered_map<QString, ButtonInstance> buttons;
		std::vector<QPushButton*> drawerButtons;
		/*unsigned short countOnShelf = 0;
		unsigned short countInDrawer = 0;
		unsigned short smallButtonsOnShelf = 0;
		QWidget *scaleRef = nullptr;	// For resizeButtons().
		//short shelfDrawerDiff = 0;	// For resizeButtons().*/

		// Threads
		QThread m_imageLoaderThread;
		QThread m_imageLoaderQueueThread;
		ImageLoader *m_imageLoader;
		ImageLoaderQueue *m_imageLoaderQueue;
};

#endif // ZOOM_WINDOW_H
