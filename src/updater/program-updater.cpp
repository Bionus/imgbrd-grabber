#include "program-updater.h"
#include <QNetworkRequest>
#include <QNetworkReply>


ProgramUpdater::ProgramUpdater(QString baseUrl)
	: m_baseUrl(baseUrl)
{ }

void ProgramUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + "/tags");
	QNetworkRequest request(url);

	connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &ProgramUpdater::checkForUpdatesDone);
	m_networkAccessManager.get(request);
}

void ProgramUpdater::checkForUpdatesDone(QNetworkReply *r)
{
	QString json = r->readAll();

	int max = 0;
	QString latest;

	QRegExp rx("\"name\":\\s*\"v([^\"]+)\"");
	int pos = 0;
	while ((pos = rx.indexIn(json, pos)) != -1)
	{
		QString version = rx.cap(1);
		int intVersion = versionToInt(rx.cap(1));

		if (intVersion > max)
		{
			max = intVersion;
			latest = version;
		}

		pos += rx.matchedLength();
	}

	int current = versionToInt(QString(VERSION));
	emit finished(latest, max > current);
}
