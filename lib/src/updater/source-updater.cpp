#include "source-updater.h"
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "models/source.h"


SourceUpdater::SourceUpdater(Source *source, QString baseUrl)
	: m_source(source), m_baseUrl(baseUrl)
{ }


void SourceUpdater::checkForUpdates()
{
	QUrl url(m_baseUrl + m_source->getName() + "/model.xml");
	QNetworkRequest request(url);

	connect(&m_networkAccessManager, &QNetworkAccessManager::finished, this, &SourceUpdater::checkForUpdatesDone);
	m_networkAccessManager.get(request);
}

void SourceUpdater::checkForUpdatesDone(QNetworkReply *reply)
{
	bool isNew = false;

	QString source = reply->readAll();
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
