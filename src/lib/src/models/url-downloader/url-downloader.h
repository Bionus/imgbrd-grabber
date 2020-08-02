#ifndef URL_DOWNLOADER_H
#define URL_DOWNLOADER_H

#include <QList>
#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QVariant>


struct UrlDownloaderUrl
{
	QString error;
	QString url;
	QMap<QString, QString> headers;
};

struct UrlDownloaderFile
{
    QString url;
    int width;
    int height;
    int filesize;
    QString ext;
};

struct UrlDownloaderResult
{
	QString error;
    QMap<QString, QVariant> tokens;
	QList<UrlDownloaderFile> files;
};


class QJSValue;

class UrlDownloader : public QObject
{
	Q_OBJECT

	public:
		UrlDownloader(QJSValue downloader, int index, QObject *parent = nullptr);
		const QString &name() const;
		bool canDownload(const QUrl &url) const;
		UrlDownloaderUrl url(const QUrl &url) const;
		UrlDownloaderResult parse(const QString &source, int statusCode) const;

	private:
		QJSValue m_downloader;
		int m_index;
		QString m_name;
		QList<QRegularExpression> m_regexes;
};

#endif // URL_DOWNLOADER_H
