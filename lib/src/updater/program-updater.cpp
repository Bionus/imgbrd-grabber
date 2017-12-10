#include "updater/program-updater.h"
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "logger.h"
#include "vendor/json.h"


ProgramUpdater::ProgramUpdater()
	: ProgramUpdater("https://api.github.com/repos/Bionus/imgbrd-grabber")
{ }

ProgramUpdater::ProgramUpdater(const QString &baseUrl)
	: m_baseUrl(baseUrl), m_downloadReply(Q_NULLPTR)
{ }

void ProgramUpdater::checkForUpdates() const
{
	#ifdef NIGHTLY
		QUrl url(m_baseUrl + "/releases/tags/nightly");
	#else
		QUrl url(m_baseUrl + "/releases/latest");
	#endif
	QNetworkRequest request(url);

	auto *reply = m_networkAccessManager->get(request);
	connect(reply, &QNetworkReply::finished, this, &ProgramUpdater::checkForUpdatesDone);
}

void ProgramUpdater::checkForUpdatesDone()
{
	auto *reply = dynamic_cast<QNetworkReply*>(sender());
	m_source = reply->readAll();

	QVariant json = Json::parse(m_source);
	QMap<QString, QVariant> lastRelease = json.toMap();

	QString changelog;
	#if defined NIGHTLY
		QString latest = lastRelease["target_commitish"].toString();
		QString current = QString(NIGHTLY_COMMIT);
		bool isNew = !current.isEmpty() && latest != current;
		latest = latest.left(8);
	#else
		QString latest = lastRelease["name"].toString().mid(1);
		bool isNew = compareVersions(latest, QString(VERSION)) > 0;
		changelog = lastRelease["body"].toString();
	#endif

	m_newVersion = latest;
	emit finished(latest, isNew, changelog);
}


QUrl ProgramUpdater::latestUrl() const
{
	QVariant json = Json::parse(m_source);
	QMap<QString, QVariant> lastRelease = json.toMap();
	return QUrl(lastRelease["html_url"].toString());
}

void ProgramUpdater::downloadUpdate()
{
	QVariant json = Json::parse(m_source);
	QMap<QString, QVariant> lastRelease = json.toMap();
	QMap<QString, QVariant> lastAsset = lastRelease["assets"].toList().first().toMap();

	QUrl url(lastAsset["browser_download_url"].toString());
	m_updateFilename = url.fileName();
	QNetworkRequest request(url);
	log(QString("Downloading installer from \"%1\".").arg(url.toString()));

	m_downloadReply = m_networkAccessManager->get(request);
	connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &ProgramUpdater::downloadProgress);
	connect(m_downloadReply, &QNetworkReply::finished, this, &ProgramUpdater::downloadDone);
}

void ProgramUpdater::downloadDone()
{
	QUrl redirection = m_downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty())
	{
		log(QString("Installer download redirected to \"%1\".").arg(redirection.toString()));
		QNetworkRequest request(redirection);
		m_downloadReply = m_networkAccessManager->get(request);
		connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &ProgramUpdater::downloadProgress);
		connect(m_downloadReply, &QNetworkReply::finished, this, &ProgramUpdater::downloadDone);
		return;
	}

	QFile file(QDir::tempPath() + QDir::separator() + m_updateFilename);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
	{
		log(QString("Error opening installer file \"%1\".").arg(file.fileName()));
		return;
	}

	file.write(m_downloadReply->readAll());
	file.close();
	log(QString("Installer file written to \"%1\"").arg(file.fileName()));

	emit downloadFinished(file.fileName());
}
