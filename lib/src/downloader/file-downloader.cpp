#include "file-downloader.h"

#define WRITE_BUFFER_SIZE (200 * 1024)


FileDownloader::FileDownloader(QObject *parent)
	: QObject(parent), m_reply(Q_NULLPTR)
{}

bool FileDownloader::start(QNetworkReply *reply, QString path)
{
	return start(reply, QStringList(path));
}
bool FileDownloader::start(QNetworkReply *reply, QStringList paths)
{
	m_file.setFileName(paths.takeFirst());
	bool ok = m_file.open(QFile::WriteOnly | QFile::Truncate);

	m_copies = paths;
	m_reply = reply;

	if (ok)
	{
		connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::replyReadyRead);
		connect(reply, &QNetworkReply::finished, this, &FileDownloader::replyFinished);
	}

	return ok;
}


void FileDownloader::replyReadyRead()
{
	if (m_reply->bytesAvailable() < WRITE_BUFFER_SIZE)
		return;

	m_file.write(m_reply->readAll());
}

void FileDownloader::replyFinished()
{
	m_file.write(m_reply->readAll());
	m_file.close();

	if (m_reply->error() != QNetworkReply::NoError)
	{
		m_file.remove();
		return;
	}

	for (QString copy : m_copies)
		m_file.copy(copy);

	emit finished();
}
