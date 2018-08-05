#include "tags/tag-api.h"
#include <QRegularExpression>
#include <QTimer>
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/site.h"


TagApi::TagApi(Profile *profile, Site *site, Api *api, int page, int limit, QObject *parent)
	: QObject(parent), m_profile(profile), m_site(site), m_api(api), m_page(page), m_limit(limit), m_reply(nullptr)
{
	const QString url = api->tagsUrl(page, limit, site).url;
	m_url = m_site->fixUrl(url);
}

TagApi::~TagApi()
{
	if (m_reply != nullptr)
		m_reply->deleteLater();
}

void TagApi::load(bool rateLimit)
{
	// Load the request with a possible delay
	int ms = m_site->msToRequest(rateLimit ? Site::QueryType::Retry : Site::QueryType::List);
	if (ms > 0)
	{ QTimer::singleShot(ms, this, SLOT(loadNow())); }
	else
	{ loadNow(); }
}

void TagApi::loadNow()
{
	log(QStringLiteral("[%1] Loading tags page <a href=\"%2\">%2</a>").arg(m_site->url(), m_url.toString().toHtmlEscaped()), Logger::Info);

	if (m_reply != nullptr)
	{
		if (m_reply->isRunning())
			m_reply->abort();

		m_reply->deleteLater();
	}

	m_reply = m_site->get(m_url);
	connect(m_reply, &QNetworkReply::finished, this, &TagApi::parse);
}

void TagApi::abort()
{
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

void TagApi::parse()
{
	log(QStringLiteral("[%1] Receiving tags page <a href=\"%2\">%2</a>").arg(m_site->url(), m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redirection.toString(), m_url);
		log(QStringLiteral("[%1] Redirecting tags page <a href=\"%2\">%2</a> to <a href=\"%3\">%3</a>").arg(m_site->url(), m_url.toString().toHtmlEscaped(), newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_url = newUrl;
		load();
		return;
	}

	// Try to read the reply
	QString source = m_reply->readAll();
	if (source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QStringLiteral("[%1][%2] Loading error: %3 (%4)").arg(m_site->url(), m_api->getName(), m_reply->errorString()).arg(m_reply->error()), Logger::Error); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Parse source
	ParsedTags ret = m_api->parseTags(source, m_site);
	if (!ret.error.isEmpty())
	{
		log(QStringLiteral("[%1][%2] %3").arg(m_site->url(), m_api->getName(), ret.error), Logger::Warning);
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	m_tags.clear();
	m_tags.append(ret.tags);

	emit finishedLoading(this, LoadResult::Ok);
}

const QList<Tag> &TagApi::tags() const
{
	return m_tags;
}
