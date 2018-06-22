#include "downloader/file-downloader.h"
#include <QNetworkReply>
#include "functions.h"

#define WRITE_BUFFER_SIZE (200 * 1024)


FileDownloader::FileDownloader(QObject *parent)
	: QObject(parent), m_reply(Q_NULLPTR), m_writeError(false)
{}

bool FileDownloader::start(QNetworkReply *reply, const QString &path)
{
	return start(reply, QStringList(path));
}
bool FileDownloader::start(QNetworkReply *reply, const QStringList &paths)
{
	m_copies = paths;
	m_file.setFileName(m_copies.takeFirst());
	const bool ok = m_file.open(QFile::WriteOnly | QFile::Truncate);

	m_writeError = false;
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

	if (m_file.write(m_reply->readAll()) < 0)
	{
		m_writeError = true;
		m_reply->abort();
	}
}

void FileDownloader::replyFinished()
{
	QByteArray data = m_reply->readAll();
	const qint64 written = m_file.write(data);
	m_file.close();

	const bool failedLastWrite = data.length() > 0 && written < 0;
	if (m_reply->error() != QNetworkReply::NoError || failedLastWrite)
	{
		m_file.remove();
		if (failedLastWrite || m_writeError)
			emit writeError();
		else
			emit networkError(m_reply->error(), m_reply->errorString());
		return;
	}

	for (const QString &copy : qAsConst(m_copies))
		m_file.copy(copy);

	emit success();
}
