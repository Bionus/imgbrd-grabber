#include "source-updater.h"
#include <QFile>
#include <QNetworkRequest>
#include <QNetworkReply>


SourceUpdater::SourceUpdater(QString source, QString directory, QString baseUrl)
	: m_source(source), m_directory(directory), m_baseUrl(baseUrl)
{
	if (!m_baseUrl.endsWith("/"))
		m_baseUrl += "/";
}


void SourceUpdater::checkForUpdates() const
{
	QUrl url(m_baseUrl + m_source + "/model.xml");
	QNetworkRequest request(url);

	auto *reply = m_networkAccessManager->get(request);
	connect(reply, &QNetworkReply::finished, this, &SourceUpdater::checkForUpdatesDone);
}

void SourceUpdater::checkForUpdatesDone()
{
	auto *reply = dynamic_cast<QNetworkReply*>(sender());
	bool isNew = false;

	QString source = reply->readAll();
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
