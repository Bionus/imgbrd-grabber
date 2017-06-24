#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>


class FileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit FileDownloader(QNetworkReply *reply, QString path, QObject *parent = Q_NULLPTR);

	signals:
		void finished();

	private slots:
		void replyReadyRead();
		void replyFinished();

	private:
		QNetworkReply *m_reply;
		QFile m_file;
};

#endif // FILE_DOWNLOADER_H
