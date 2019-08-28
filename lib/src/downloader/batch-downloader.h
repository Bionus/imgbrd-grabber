#ifndef BATCH_DOWNLOADER_H
#define BATCH_DOWNLOADER_H

#include <QMap>
#include <QObject>
#include <QQueue>
#include <QSharedPointer>
#include "downloader/image-save-result.h"


class DownloadQuery;
class Image;
class ImageDownloader;
class PackLoader;
class Profile;
class QSettings;

class BatchDownloader : public QObject
{
	Q_OBJECT

	public:
		enum BatchDownloadStep
		{
			NotStarted,
			Login,
			PageDownload,
			ImageDownload,
			Finished,
			Aborted,
		};

		enum Counter
		{
			Resumed,
			Errors,
			NotFound,
			AlreadyExists,
			Ignored,
			Downloaded,
			Missing,
		};

		BatchDownloader(DownloadQuery *query, Profile *profile, QObject *parent = nullptr);
		BatchDownloadStep currentStep() const;
		int totalCount() const;
		int downloadedCount() const;
		int downloadedCount(Counter counter) const;
		DownloadQuery *query() const;

	public slots:
		void start();
		void abort();

	protected slots:
		void login();
		void loginFinished();
		void nextPack();
		void nextImages();
		void nextImage();
		void loadImage(QSharedPointer<Image> img);
		void loadImageFinished(const QSharedPointer<Image> &img, QList<ImageSaveResult> result);
		void allFinished();

	protected:
		void setCurrentStep(BatchDownloadStep step);

	signals:
		void stepChanged(BatchDownloadStep step);
		void imageDownloadProgress(const QSharedPointer<Image> &img, qint64 bytesReceived, qint64 bytesTotal);
		void imageDownloadFinished(const QSharedPointer<Image> &img, Image::SaveResult result);
		void finished();

	private:
		DownloadQuery *m_query;
		Profile *m_profile;
		QSettings *m_settings;
		BatchDownloadStep m_step;
		PackLoader *m_packLoader = nullptr;
		QAtomicInt m_currentlyProcessing;
		QQueue<QSharedPointer<Image>> m_pendingDownloads;
		QQueue<QSharedPointer<Image>> m_failedDownloads;
		QMap<QSharedPointer<Image>, ImageDownloader*> m_imageDownloaders;
		int m_totalCount = 0;

		// Counters
		QMap<Counter, int> m_counters;
		int m_counterSum;
};

#endif //BATCH_DOWNLOADER_H
