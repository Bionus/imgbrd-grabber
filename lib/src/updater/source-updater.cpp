#include "updater/source-updater.h"
#include <QFile>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "custom-network-access-manager.h"


SourceUpdater::SourceUpdater(QString source, QString directory, QString baseUrl)
	: m_source(std::move(source)), m_directory(std::move(directory)), m_baseUrl(std::move(baseUrl))
{
	if (!m_baseUrl.endsWith("/"))
		m_baseUrl += "/";
}


void SourceUpdater::checkForUpdates() const
{
	const QUrl url(m_baseUrl + m_source + "/model.xml");
	const QNetworkRequest request(url);

	auto *reply = m_networkAccessManager->get(request);
	connect(reply, &QNetworkReply::finished, this, &SourceUpdater::checkForUpdatesDone);
}

void SourceUpdater::checkForUpdatesDone()
{
	auto *reply = dynamic_cast<QNetworkReply*>(sender());
	bool isNew = false;

	// TODO: source check for updates is broken since switch to JS model files
	/*QString source = reply->readAll();
	if (source.startsWith("<?xml"))
	{
		QFile current(m_directory + "/model.xml");
		if (current.open(QFile::ReadOnly))
		{
			QString compare = current.readAll();
			current.close();

			compare.replace("\r\n", "\n");
			source.replace("\r\n", "\n");

			if (compare != source)
				isNew = true;
		}
	}*/

	emit finished(m_source, isNew);
	reply->deleteLater();
}
