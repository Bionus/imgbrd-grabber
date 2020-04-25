#include "downloader/file-downloader.h"
#include "functions.h"
#include "logger.h"
#include "network/network-reply.h"

#define WRITE_BUFFER_SIZE (200 * 1024)


FileDownloader::FileDownloader(bool allowHtmlResponses, QObject *parent)
	: QObject(parent), m_allowHtmlResponses(allowHtmlResponses), m_reply(nullptr), m_writeError(false)
{}

bool FileDownloader::start(NetworkReply *reply, const QString &path)
{
	m_file.setFileName(path);
	const bool ok = m_file.open(QFile::WriteOnly | QFile::Truncate);

	m_writeError = false;
	m_reply = reply;

	if (ok) {
		connect(reply, &NetworkReply::readyRead, this, &FileDownloader::replyReadyRead);
		connect(reply, &NetworkReply::finished, this, &FileDownloader::replyFinished);
	}

	return ok;
}


void FileDownloader::replyReadyRead()
{
	if (m_reply->bytesAvailable() < WRITE_BUFFER_SIZE) {
		return;
	}

	if (m_file.write(m_reply->readAll()) < 0) {
		m_writeError = true;
		m_reply->abort();
	} else {
		m_file.flush();
	}
}

void FileDownloader::replyFinished()
{
	QByteArray data = m_reply->readAll();
	const qint64 written = m_file.write(data);
	m_file.close();

	const auto error = m_reply->error();
	const auto msg = m_reply->errorString();
	const bool failedLastWrite = data.length() > 0 && written < 0;
	const bool invalidHtml = !m_allowHtmlResponses && QString(data.left(100)).trimmed().startsWith("<!DOCTYPE", Qt::CaseInsensitive);
	if (error != NetworkReply::NetworkError::NoError || failedLastWrite || invalidHtml) {
		// Ignore those errors as they are caused by a bug in Qt
		if (error != NetworkReply::NetworkError::NoError && msg.contains("140E0197")) {
			log(QStringLiteral("Ignored network error '140E0197' for the image: `%1`: %2 (%3)").arg(m_reply->url().toString().toHtmlEscaped()).arg(error).arg(msg), Logger::Info);
			emit success();
			return;
		}

		m_file.remove();
		if (failedLastWrite || m_writeError) {
			emit writeError();
		} else if (invalidHtml) {
			log(QString("Invalid HTML content returned for url '%1'").arg(m_reply->url().toString()), Logger::Info);
			emit networkError(NetworkReply::NetworkError::ContentNotFoundError, "Invalid HTML content returned");
		} else {
			emit networkError(m_reply->error(), m_reply->errorString());
		}
		return;
	}

	emit success();
}
