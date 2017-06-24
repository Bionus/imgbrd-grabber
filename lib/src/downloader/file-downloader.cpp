#include "file-downloader.h"


FileDownloader::FileDownloader(QNetworkReply *reply, QString path, QObject *parent)
	: FileDownloader(reply, QStringList(path), parent)
{}

FileDownloader::FileDownloader(QNetworkReply *reply, QStringList paths, QObject *parent)
	: QObject(parent), m_reply(reply), m_file(paths.takeFirst())
{
	connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::replyReadyRead);
	connect(reply, &QNetworkReply::finished, this, &FileDownloader::replyFinished);

	m_file.open(QFile::WriteOnly | QFile::Truncate);
	m_copies = paths;
}


void FileDownloader::replyReadyRead()
{
	m_file.write(m_reply->readAll());
}

void FileDownloader::replyFinished()
{
	m_file.write(m_reply->readAll());
	m_file.close();

	for (QString copy : m_copies)
		m_file.copy(copy);

	emit finished();
}
