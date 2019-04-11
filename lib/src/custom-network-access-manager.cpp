#include "custom-network-access-manager.h"
#include <QDebug>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include "functions.h"
#include "logger.h"
#include "vendor/qcustomnetworkreply.h"

QQueue<QString> CustomNetworkAccessManager::NextFiles;


CustomNetworkAccessManager::CustomNetworkAccessManager(QObject *parent)
	: QNetworkAccessManager(parent)
{
	connect(this, &QNetworkAccessManager::sslErrors, this, &CustomNetworkAccessManager::sslErrorHandler);
}

QNetworkReply *CustomNetworkAccessManager::makeErrorReply(const QNetworkRequest &request, const QString &code)
{
	auto *reply = new QCustomNetworkReply(this);
	reply->setUrl(request.url());

	if (code == QLatin1String("404")) {
		reply->setHttpStatusCode(404, "Not Found");
		reply->setNetworkError(QNetworkReply::ContentNotFoundError, QStringLiteral("Not Found"));
	} else if (code == QLatin1String("cookie")) {
		cookieJar()->insertCookie(QNetworkCookie("test_cookie", "test_value"));
		reply->setHttpStatusCode(200, "OK");
	} else if (code == QLatin1String("redirect")) {
		reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute, QUrl("https://www.test-redirect.com"));
		reply->setHttpStatusCode(200, "OK");
	} else {
		reply->setHttpStatusCode(500, "Internal Server Error");
		reply->setNetworkError(QNetworkReply::UnknownNetworkError, QStringLiteral("Internal Server Error"));
	}

	reply->setContentType("text/html");
	reply->setContent(QByteArray());
	return reply;
}

QNetworkReply *CustomNetworkAccessManager::makeTestReply(const QNetworkRequest &request)
{
	QString md5 = QString(QCryptographicHash::hash(request.url().toString().toLatin1(), QCryptographicHash::Md5).toHex());
	const QString filename = request.url().fileName();
	const QString ext = filename.contains('.') ? filename.mid(filename.lastIndexOf('.') + 1) : QStringLiteral("html");
	const QString host = request.url().host();
	QString path = "tests/resources/pages/" + host + "/" + md5 + "." + ext;

	const bool fromQueue = !CustomNetworkAccessManager::NextFiles.isEmpty();
	if (fromQueue) {
		path = CustomNetworkAccessManager::NextFiles.dequeue();
	}

	// Error testing
	if (path == QLatin1String("404") || path == QLatin1String("500") || path == QLatin1String("cookie") || path == QLatin1String("redirect")) {
		return makeErrorReply(request, path);
	}

	QFile f(path);
	const bool opened = f.open(QFile::ReadOnly);
	const bool logFilename = !opened || !fromQueue;
	if (!opened) {
		if (fromQueue) {
			qDebug() << ("Test file not found: " + f.fileName() + " (" + request.url().toString() + ")");
		}

		md5 = QString(QCryptographicHash::hash(request.url().toString().toLatin1(), QCryptographicHash::Md5).toHex());
		f.setFileName("tests/resources/pages/" + host + "/" + md5 + "." + ext);

		if (!f.open(QFile::ReadOnly)) {
			// LCOV_EXCL_START
			if (ext != QLatin1String("jpg") && ext != QLatin1String("png")) {
				qDebug() << ("Test file not found: " + f.fileName() + " (" + request.url().toString() + ")");
				return makeErrorReply(request, "404");
			}
			// LCOV_EXCL_STOP

			f.setFileName(QStringLiteral("tests/resources/image_1x1.png"));

			// LCOV_EXCL_START
			if (!f.open(QFile::ReadOnly)) {
				return nullptr;
			}
			// LCOV_EXCL_STOP
		}
	}

	if (logFilename) {
		qDebug() << ("Reply from file: " + request.url().toString() + " -> " + f.fileName());
	}
	const QByteArray content = f.readAll();

	auto *reply = new QCustomNetworkReply(this);
	reply->setUrl(request.url());
	reply->setHttpStatusCode(200, "OK");
	reply->setContentType("text/html");
	reply->setContent(content);

	return reply;
}

QNetworkReply *CustomNetworkAccessManager::get(const QNetworkRequest &request)
{
	if (isTestModeEnabled()) {
		return makeTestReply(request);
	}

	log(QStringLiteral("Loading `%1`").arg(request.url().toString().toHtmlEscaped()), Logger::Debug);
	return QNetworkAccessManager::get(request);
}

QNetworkReply *CustomNetworkAccessManager::post(const QNetworkRequest &request, const QByteArray &data)
{
	if (isTestModeEnabled()) {
		return makeTestReply(request);
	}

	log(QStringLiteral("Posting to `%1`").arg(request.url().toString().toHtmlEscaped()), Logger::Debug);
	return QNetworkAccessManager::post(request, data);
}

/**
 * Log SSL errors in debug mode only.
 *
 * @param reply		The network reply who generated the SSL errors
 * @param errors	The list of SSL errors that occurred
 */
void CustomNetworkAccessManager::sslErrorHandler(QNetworkReply *reply, const QList<QSslError> &errors)
{
	#ifdef QT_DEBUG
		qDebug() << errors;
	#else
		Q_UNUSED(errors);
	#endif
	#ifndef TEST
		reply->ignoreSslErrors();
	#else
		Q_UNUSED(reply);
	#endif
}
