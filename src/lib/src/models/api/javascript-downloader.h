#ifndef JAVASCRIPT_DOWNLOADER_H
#define JAVASCRIPT_DOWNLOADER_H

#include <QObject>
#include <QJSValue>
#include <QRegularExpression>
#include <QString>
#include <QUrl>
#include <QList>
#include <QHash>


struct DownloadableUrl
{
	QString error;

	QUrl url;
	QHash<QString, QString> headers;
};

struct DownloadableFile
{
	QUrl url;
	int width;
	int height;
	int filesize;
	QString ext;
};

struct DownloadableData
{
	QString error;

	QHash<QString, QString> tokens;
	QList<DownloadableFile> files;
};


class JavascriptDownloader : public QObject
{
	Q_OBJECT

	public:
		JavascriptDownloader(QString name, const QJSValue &handler);

		QString getName() const;
		bool canHandle(const QUrl &url) const;
		DownloadableUrl url(const QUrl &url) const;
		DownloadableData parse(const QString &source, int statusCode) const;

	private:
		DownloadableFile parseDownloadableFile(const QJSValue &jsVal) const;

	private:
		QString m_name;
		const QJSValue &m_handler;
		QList<QRegularExpression> m_regularExpressions;
};

#endif // JAVASCRIPT_DOWNLOADER_H
