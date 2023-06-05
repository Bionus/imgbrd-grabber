#include "downloader/file-downloader.h"
#include <QFile>
#include "functions.h"
#include "logger.h"
#include "network/network-reply.h"

#define WRITE_BUFFER_SIZE (200 * 1024)


FileDownloader::FileDownloader(bool allowHtmlResponses, QObject *parent)
	: QObject(parent), m_allowHtmlResponses(allowHtmlResponses), m_reply(nullptr), m_readSize(0), m_writeError(false)
{}

bool FileDownloader::start(NetworkReply *reply, const QString &path)
{
	m_file.setFileName(path);
	const bool ok = m_file.open(QFile::WriteOnly | QFile::Truncate);

	m_readSize = 0;
	m_writeError = false;
	m_reply = reply;

	if (ok) {
		connect(reply, &NetworkReply::readyRead, this, &FileDownloader::replyReadyRead);
		connect(reply, &NetworkReply::finished, this, &FileDownloader::replyFinished);
	} else {
		log(QStringLiteral("Unable to open file '%1': %2 (%3)").arg(path, m_file.errorString(), QString::number(m_file.error())), Logger::Error);
	}

	return ok;
}


void FileDownloader::replyReadyRead()
{
	if (m_reply->bytesAvailable() < WRITE_BUFFER_SIZE) {
		return;
	}

	const QByteArray data = m_reply->readAll();
	m_readSize += data.size();

	if (m_file.write(data) < 0) {
		m_writeError = true;
		m_reply->abort();
	} else {
		m_file.flush();
	}
}

void FileDownloader::replyFinished()
{
	const QByteArray data = m_reply->readAll();
	m_readSize += data.size();

	const qint64 written = m_file.write(data);
	m_file.close();

	const auto error = m_reply->error();
	const auto msg = m_reply->errorString();
	const QUrl redirectUrl = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	const bool failedLastWrite = data.length() > 0 && written < 0;
	const bool invalidHtml = !m_allowHtmlResponses && isHtml(data) && redirectUrl.isEmpty();
	const bool emptyFile = m_readSize == 0 && redirectUrl.isEmpty();

	if (error != NetworkReply::NetworkError::NoError || failedLastWrite || invalidHtml || emptyFile) {
		// Ignore those errors as they are caused by a bug in Qt
		if (error != NetworkReply::NetworkError::NoError && msg.contains("140E0197")) {
			log(QStringLiteral("Ignored network error '140E0197' for the image: `%1`: %2 (%3)").arg(m_reply->url().toString().toHtmlEscaped()).arg(error).arg(msg), Logger::Info);
			emit success();
			return;
		}

		m_file.remove();
		if (failedLastWrite || m_writeError) {
			emit writeError();
		} else if (invalidHtml && error == NetworkReply::NetworkError::NoError) {
			log(QString("Invalid HTML content returned for url '%1'").arg(m_reply->url().toString()), Logger::Info);
			emit networkError(NetworkReply::NetworkError::ContentNotFoundError, "Invalid HTML content returned");
		} else if (emptyFile && error == NetworkReply::NetworkError::NoError) {
			log(QString("Empty file returned for url '%1'").arg(m_reply->url().toString()), Logger::Info);
			emit networkError(NetworkReply::NetworkError::ContentNotFoundError, "Empty file returned");
		} else {
			emit networkError(error, msg);
		}
		return;
	}

	emit success();
}
