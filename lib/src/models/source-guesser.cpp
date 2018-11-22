#include "models/source-guesser.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include "custom-network-access-manager.h"
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/source.h"


SourceGuesser::SourceGuesser(QString url, QList<Source*> sources)
	: m_url(std::move(url)), m_sources(std::move(sources))
{
	m_manager = new CustomNetworkAccessManager(this);
}

Source *SourceGuesser::start()
{
	m_cache.clear();
	int current = 0;

	for (Source *source : qAsConst(m_sources))
	{
		Api *api = source->getApis().first();
		if (api->canLoadCheck())
		{
			const QString checkUrl = api->checkUrl().url;
			if (!m_cache.contains(checkUrl))
			{
				QUrl getUrl(m_url + checkUrl);
				QNetworkReply *reply;
				do
				{
					reply = m_manager->get(QNetworkRequest(getUrl));
					QEventLoop loop;
					connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
					loop.exec();

					getUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
				} while (!getUrl.isEmpty());

				if (reply->error() != 0)
				{
					log(QStringLiteral("Error getting the test page: %1.").arg(reply->errorString()), Logger::Error);
					emit progress(++current);
					continue;
				}

				m_cache[checkUrl] = reply->readAll();
			}

			if (api->parseCheck(m_cache[checkUrl], 200).ok)
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
