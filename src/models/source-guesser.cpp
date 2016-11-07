#include "source-guesser.h"
#include <QEventLoop>
#include "functions.h"


SourceGuesser::SourceGuesser(QString url, QList<Source*> sources)
	: m_url(url), m_sources(sources)
{
	m_manager = new QNetworkAccessManager(this);
}

void SourceGuesser::start()
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
				QUrl getUrl("http://" + m_url + checkUrl);
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
					log(tr("Erreur lors de la récupération de la page de test : %1.").arg(reply->errorString()), Error);
					emit progress(++current);
					continue;
				}

				m_cache[checkUrl] = reply->readAll();
			}

			QRegExp rx(map->value("Check/Regex"));
			if (rx.indexIn(m_cache[checkUrl]) != -1)
			{
				emit finished(source);
				return;
			}

			emit progress(++current);
		}
	}

	emit finished(nullptr);
}
