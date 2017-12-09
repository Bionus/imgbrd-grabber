#include "custom-network-access-manager.h"
#include <QDebug>
#include <QFile>
#include "functions.h"
#include "vendor/qcustomnetworkreply.h"

QQueue<QString> CustomNetworkAccessManager::NextFiles;


CustomNetworkAccessManager::CustomNetworkAccessManager(QObject *parent)
	: QNetworkAccessManager(parent)
{
	connect(this, &QNetworkAccessManager::sslErrors, this, &CustomNetworkAccessManager::sslErrorHandler);
}

QNetworkReply *CustomNetworkAccessManager::get(const QNetworkRequest &request)
{
	if (isTestModeEnabled())
	{
		QString md5 = QString(QCryptographicHash::hash(request.url().toString().toLatin1(), QCryptographicHash::Md5).toHex());
		QString filename = request.url().fileName();
		QString ext = filename.contains('.') ? filename.mid(filename.lastIndexOf('.') + 1) : "html";
		QString host = request.url().host();
		QString path = "tests/resources/pages/" + host + "/" + md5 + "." + ext;

		bool fromQueue = !CustomNetworkAccessManager::NextFiles.isEmpty();
		if (fromQueue)
		{ path = CustomNetworkAccessManager::NextFiles.dequeue(); }

		QFile f(path);
		bool opened = f.open(QFile::ReadOnly);
		bool logFilename = !opened || !fromQueue;
		if (!opened)
		{
			md5 = QString(QCryptographicHash::hash(request.url().toString().toLatin1(), QCryptographicHash::Md5).toHex());
			f.setFileName("tests/resources/pages/" + host + "/" + md5 + "." + ext);

			if (!f.open(QFile::ReadOnly))
			{
				// LCOV_EXCL_START
				if (ext != "jpg" && ext != "png")
				{
					qDebug() << ("Test file not found: " + f.fileName() + " (" + request.url().toString() + ")");
					return nullptr;
				}
				// LCOV_EXCL_STOP

				f.setFileName("tests/resources/image_1x1.png");

				// LCOV_EXCL_START
				if (!f.open(QFile::ReadOnly))
					return nullptr;
				// LCOV_EXCL_STOP
			}
		}

		if (logFilename)
		{ qDebug() << ("Reply from file: " + request.url().toString() + " -> " + f.fileName()); }
		QByteArray content = f.readAll();

		auto *reply = new QCustomNetworkReply(this);
		reply->setHttpStatusCode(200, "OK");
		reply->setContentType("text/html");
		reply->setContent(content);

		return reply;
	}

	log(QString("Loading <a href=\"%1\">%1</a>").arg(request.url().toString().toHtmlEscaped()), Logger::Debug);
	return QNetworkAccessManager::get(request);
}

/**
 * Log SSL errors in debug mode only.
 *
 * @param qnr		The network reply who generated the SSL errors
 * @param errors	The list of SSL errors that occured
 */
void CustomNetworkAccessManager::sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors)
{
	#ifdef QT_DEBUG
		qDebug() << errors;
	#else
		Q_UNUSED(errors);
	#endif
	#ifndef TEST
		qnr->ignoreSslErrors();
	#else
		Q_UNUSED(qnr);
	#endif
}
