#include "program-updater.h"
#include <QNetworkRequest>
#include <QNetworkReply>


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

	int max = 0;
	QString latest;

	QRegExp rx("\"name\":\\s*\"v([^\"]+)\"");
	int pos = 0;
	while ((pos = rx.indexIn(m_source, pos)) != -1)
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

	int current = versionToInt(QString(VERSION)) - 1;
	emit finished(latest, max > current, "* updated\n*test");
}

QNetworkReply *ProgramUpdater::downloadUpdate()
{
	QUrl url(m_baseUrl + "/releases");
	QNetworkRequest request(url);

	return m_networkAccessManager.get(request);
}
