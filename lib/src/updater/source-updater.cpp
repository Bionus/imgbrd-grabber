#include "source-updater.h"
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>


SourceUpdater::SourceUpdater(QString source, QString directory, QString baseUrl)
	: m_source(source), m_directory(directory), m_baseUrl(baseUrl), m_checkForUpdatesReply(Q_NULLPTR)
{
	if (!m_baseUrl.endsWith("/"))
		m_baseUrl += "/";
}


void SourceUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + m_source + "/model.xml");
	QNetworkRequest request(url);

	m_checkForUpdatesReply = m_networkAccessManager->get(request);
	connect(m_checkForUpdatesReply, &QNetworkReply::finished, this, &SourceUpdater::checkForUpdatesDone);
}

void SourceUpdater::checkForUpdatesDone()
{
	bool isNew = false;

	QString source = m_checkForUpdatesReply->readAll();
	if (source.startsWith("<?xml"))
	{
		QFile current(m_directory + "/model.xml");
		if (current.open(QFile::ReadOnly))
		{
			QString compare = current.readAll();
			current.close();

			if (compare != source)
				isNew = true;
		}
	}

	emit finished(m_source, isNew);
}
