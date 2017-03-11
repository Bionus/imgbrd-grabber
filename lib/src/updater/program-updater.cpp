#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDir>
#include <QFile>
#include "program-updater.h"
#include "functions.h"
#include "vendor/json.h"


ProgramUpdater::ProgramUpdater()
	: ProgramUpdater("https://api.github.com/repos/Bionus/imgbrd-grabber")
{ }

ProgramUpdater::ProgramUpdater(QString baseUrl)
	: m_baseUrl(baseUrl)
{ }

void ProgramUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + "/releases/latest");
	QNetworkRequest request(url);

	m_checkForUpdatesReply = m_networkAccessManager.get(request);
	connect(m_checkForUpdatesReply, &QNetworkReply::finished, this, &ProgramUpdater::checkForUpdatesDone);
}

void ProgramUpdater::checkForUpdatesDone()
{
	m_source = m_checkForUpdatesReply->readAll();

	QVariant json = Json::parse(m_source);
	QMap<QString, QVariant> lastRelease = json.toMap();

	QString latest = lastRelease["name"].toString().mid(1);
	QString changelog = lastRelease["body"].toString();

	m_newVersion = latest;
	bool isNew = compareVersions(latest, QString(VERSION)) > 0;

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

	m_downloadReply = m_networkAccessManager.get(request);
	connect(m_downloadReply, &QNetworkReply::downloadProgress, this, &ProgramUpdater::downloadProgress);
	connect(m_downloadReply, &QNetworkReply::finished, this, &ProgramUpdater::downloadDone);
}

void ProgramUpdater::downloadDone()
{
	QUrl redir = m_downloadReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		log(QString("Installer download redirected to \"%1\".").arg(redir.toString()));
		QNetworkRequest request(redir);
		m_downloadReply = m_networkAccessManager.get(request);
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
