#include "file-downloader.h"


FileDownloader::FileDownloader(QNetworkReply *reply, QString path, QObject *parent)
	: QObject(parent), m_reply(reply)
{
	connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::replyReadyRead);
	connect(reply, &QNetworkReply::finished, this, &FileDownloader::replyFinished);

	m_file = QFile(path);
	m_file.open(QFile::WriteOnly | QFile::Truncate);

	QNetworkRequest request(imageUrl);
	m_WebCtrl.get(request);
}


void FileDownloader::replyReadyRead()
{
	m_file.write(m_reply->readAll());
}

void FileDownloader::replyFinished()
{
	m_file.write(m_reply->readAll());
	m_file.close();

	emit finished();
}
