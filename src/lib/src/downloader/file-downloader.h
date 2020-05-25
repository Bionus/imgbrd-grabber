#ifndef FILE_DOWNLOADER_H
#define FILE_DOWNLOADER_H

#include <QFile>
#include <QObject>
#include "network/network-reply.h"


class QString;

class FileDownloader : public QObject
{
	Q_OBJECT

	public:
		explicit FileDownloader(bool allowHtmlResponses, QObject *parent = nullptr);
		bool start(NetworkReply *reply, const QString &path);

	signals:
		void writeError();
		void networkError(NetworkReply::NetworkError error, const QString &errorString);
		void success();

	private slots:
		void replyReadyRead();
		void replyFinished();

	private:
		bool m_allowHtmlResponses;
		NetworkReply *m_reply;
		QFile m_file;
		bool m_writeError;
};

#endif // FILE_DOWNLOADER_H
