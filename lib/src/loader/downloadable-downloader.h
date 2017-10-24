#ifndef DOWNLOADABLE_DOWNLOADER_H
#define DOWNLOADABLE_DOWNLOADER_H

#include <QObject>
#include <QSharedPointer>
#include "downloadable.h"
#include "downloader/file-downloader.h"


class Site;

class DownloadableDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit DownloadableDownloader(QSharedPointer<Downloadable> downloadable, const Filename &filename, const QString &folder, Site *site, int count, bool addMd5, bool startCommands, bool loadTags, QObject *parent = Q_NULLPTR);
		void save();

	protected slots:
		void preloaded();
		void writeError();
		void networkError(QNetworkReply::NetworkError error, QString errorString);
		void success();

	signals:
		void saved(QSharedPointer<Downloadable> downloadable, QMap<QString, Downloadable::SaveResult> result);

	protected:
		void setResult(QStringList keys, Downloadable::SaveResult value);
		void postSave();

	private:
		QSharedPointer<Downloadable> m_downloadable;
		Filename m_filename;
		QString m_folder;
		Site *m_site;
		int m_count;
		bool m_addMd5;
		bool m_startCommands;
		bool m_loadTags;
		QUrl m_url;
		QStringList m_paths;
		FileDownloader m_fileDownloader;
		QMap<QString, Downloadable::SaveResult> m_result;
};

#endif // DOWNLOADABLE_DOWNLOADER_H
