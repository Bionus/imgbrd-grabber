#include "source-guesser.h"
#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QRegularExpression>
#include "source.h"
#include "api.h"
#include "custom-network-access-manager.h"
#include "logger.h"


SourceGuesser::SourceGuesser(QString url, QList<Source*> sources)
	: m_url(std::move(url)), m_sources(std::move(sources))
{
	m_manager = new CustomNetworkAccessManager(this);
}

Source *SourceGuesser::start()
{
	m_cache.clear();
	int current = 0;

	for (Source *source : m_sources)
	{
		if (source->getApis().isEmpty())
			continue;

		Api *map = source->getApis().first();
		if (map->contains("Check/Url") && map->contains("Check/Regex"))
		{
			QString checkUrl = map->value("Check/Url");
			if (!m_cache.contains(checkUrl))
			{
				QUrl getUrl(m_url + checkUrl);
				QNetworkReply *reply;
				do
				{
					reply = m_manager->get(QNetworkRequest(getUrl));
					QEventLoop loop;
						connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();

					getUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
				} while (!getUrl.isEmpty());

				if (reply->error() != 0)
				{
					log(QString("Error getting the test page: %1.").arg(reply->errorString()), Logger::Error);
					emit progress(++current);
					continue;
				}

				m_cache[checkUrl] = reply->readAll();
			}

			QRegularExpression rx(map->value("Check/Regex"));
			if (rx.match(m_cache[checkUrl]).hasMatch())
			{
				emit finished(source);
				return source;
			}

			emit progress(++current);
		}
	}

	emit finished(nullptr);
	return nullptr;
}
