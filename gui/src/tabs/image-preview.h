#ifndef IMAGE_PREVIEW_H
#define IMAGE_PREVIEW_H

#include <functional>
#include <QObject>
#include <QPixmap>
#include <QSharedPointer>
#include <QString>
#include <QUrl>


class DownloadQueue;
class Image;
class MainWindow;
class NetworkReply;
class Profile;
class QBouton;
class QMenu;
class QMovie;
class QWidget;

class ImagePreview : public QObject
{
	Q_OBJECT

	public:
		ImagePreview(QSharedPointer<Image> image, QWidget *container, Profile *profile, DownloadQueue *downloadQueue, MainWindow *mainWindow, QObject *parent = nullptr);
		~ImagePreview();
		void setCustomContextMenu(std::function<void (QMenu *)> customContextMenu);

	public slots:
		void load();
		void abort();
		void setChecked(bool checked);
		void setDownloadProgress(qint64 v1, qint64 v2);

	protected:
		void showLoadingMessage();
		void finishedLoading();

	protected slots:
		void finishedLoadingPreview();
		void customContextMenuRequested();
		void contextSaveImage();
		void contextSaveImageAs();
		void contextSaveImageProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2);
		void toggledWithId(int id, bool toggle, bool range);

	signals:
		void finished();
		void clicked();
		void toggled(bool toggle, bool range);

	private:
		QSharedPointer<Image> m_image;
		QWidget *m_container;
		Profile *m_profile;
		DownloadQueue *m_downloadQueue;
		MainWindow *m_mainWindow;
		static QMovie *m_loadingMovie;

		NetworkReply *m_reply = nullptr;
		bool m_aborted = false;
		bool m_checked = false;

		QUrl m_thumbnailUrl;
		QString m_name;
		int m_childrenCount = 0;
		QPixmap m_thumbnail;
		QBouton *m_bouton = nullptr;
		std::function<void (QMenu *)> m_customContextMenu = nullptr;
};

#endif // IMAGE_PREVIEW_H
