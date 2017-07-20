#include "source-updater.h"
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "models/source.h"


SourceUpdater::SourceUpdater(Source *source, QString baseUrl)
	: m_source(source), m_baseUrl(baseUrl)
{
	if (!m_baseUrl.endsWith("/"))
		m_baseUrl += "/";
}


void SourceUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + m_source->getName() + "/model.xml");
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
		QFile current(m_source->getPath() + "/model.xml");
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
