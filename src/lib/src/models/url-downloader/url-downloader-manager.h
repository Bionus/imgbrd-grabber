#ifndef URL_DOWNLOADER_MANAGER_H
#define URL_DOWNLOADER_MANAGER_H

#include <QList>
#include <QObject>
#include <QString>


class UrlDownloader;

class UrlDownloaderManager : public QObject
{
	Q_OBJECT

	public:
		explicit UrlDownloaderManager(const QString &root, QObject *parent = nullptr);
		bool load(const QString &file);
		UrlDownloader *canDownload(const QUrl &url) const;

	private:
		QJSEngine *m_engine;
		QList<UrlDownloader*> m_downloaders;
};

#endif // URL_DOWNLOADER_MANAGER_H
