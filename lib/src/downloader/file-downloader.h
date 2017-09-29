#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>


class FileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit FileDownloader(QObject *parent = Q_NULLPTR);
		bool start(QNetworkReply *reply, QString path);
		bool start(QNetworkReply *reply, QStringList paths);

	signals:
		void writeError();
		void networkError(QNetworkReply::NetworkError error, QString errorString);
		void success();

	private slots:
		void replyReadyRead();
		void replyFinished();

	private:
		QNetworkReply *m_reply;
		QFile m_file;
		bool m_writeError;
		QStringList m_copies;
};

#endif // FILE_DOWNLOADER_H
