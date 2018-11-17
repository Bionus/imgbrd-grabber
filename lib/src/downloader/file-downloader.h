#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include <QFile>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QStringList>


class FileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit FileDownloader(bool allowHtmlResponses, QObject *parent = nullptr);
		bool start(QNetworkReply *reply, const QString &path);
		bool start(QNetworkReply *reply, const QStringList &paths);

	signals:
		void writeError();
		void networkError(QNetworkReply::NetworkError error, const QString &errorString);
		void success();

	private slots:
		void replyReadyRead();
		void replyFinished();

	private:
		bool m_allowHtmlResponses;
		QNetworkReply *m_reply;
		QFile m_file;
		bool m_writeError;
		QStringList m_copies;
};

#endif // FILE_DOWNLOADER_H
