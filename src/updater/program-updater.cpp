#include <QNetworkRequest>
#include <QNetworkReply>
#include "program-updater.h"
#include "json.h"


ProgramUpdater::ProgramUpdater()
	: ProgramUpdater("https://api.github.com/repos/Bionus/imgbrd-grabber")
{ }

ProgramUpdater::ProgramUpdater(QString baseUrl)
	: m_baseUrl(baseUrl)
{ }

void ProgramUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + "/releases");
	QNetworkRequest request(url);

	m_checkForUpdatesReply = m_networkAccessManager.get(request);
	connect(m_checkForUpdatesReply, &QNetworkReply::finished, this, &ProgramUpdater::checkForUpdatesDone);
}

void ProgramUpdater::checkForUpdatesDone()
{
	m_source = m_checkForUpdatesReply->readAll();

	QVariant json = Json::parse(m_source);
	QMap<QString, QVariant> lastRelease = json.toList().first().toMap();

	QString latest = lastRelease["name"].toString().mid(1);
	QString changelog = lastRelease["body"].toString();

	int max = versionToInt(latest);
	int current = versionToInt(QString(VERSION)) - 1;
	bool isNew = max > current;

	emit finished(latest, isNew, changelog);
}

QNetworkReply *ProgramUpdater::downloadUpdate()
{
	QUrl url(m_baseUrl + "/releases");
	QNetworkRequest request(url);

	return m_networkAccessManager.get(request);
}
