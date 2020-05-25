#include "tags/tag-api-base.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/site.h"
#include "network/network-reply.h"


TagApiBase::TagApiBase(Profile *profile, Site *site, Api *api, QObject *parent)
	: QObject(parent), m_profile(profile), m_site(site), m_api(api), m_reply(nullptr)
{}

TagApiBase::~TagApiBase()
{
	if (m_reply != nullptr) {
		m_reply->deleteLater();
	}
}

void TagApiBase::setUrl(QUrl url)
{
	m_url = std::move(url);
}

void TagApiBase::load(bool rateLimit)
{
	log(QStringLiteral("[%1] Loading tags page `%2`").arg(m_site->url(), m_url.toString().toHtmlEscaped()), Logger::Info);

	if (m_reply != nullptr) {
		if (m_reply->isRunning()) {
			m_reply->abort();
		}

		m_reply->deleteLater();
	}

	Site::QueryType type = rateLimit ? Site::QueryType::Retry : Site::QueryType::List;
	m_reply = m_site->get(m_url, type);
	connect(m_reply, &NetworkReply::finished, this, &TagApiBase::parseInternal);
}

void TagApiBase::abort()
{
	if (m_reply != nullptr && m_reply->isRunning()) {
		m_reply->abort();
	}
}

void TagApiBase::parseInternal()
{
	log(QStringLiteral("[%1] Receiving tags page `%2`").arg(m_site->url(), m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty()) {
		QUrl newUrl = m_site->fixUrl(redirection.toString(), m_url);
		log(QStringLiteral("[%1] Redirecting tags page `%2` to `%3`").arg(m_site->url(), m_url.toString().toHtmlEscaped(), newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_url = newUrl;
		load();
		return;
	}

	// Try to read the reply
	QString source = m_reply->readAll();
	if (source.isEmpty()) {
		if (m_reply->error() != NetworkReply::NetworkError::OperationCanceledError) {
			log(QStringLiteral("[%1][%2] Loading error: %3 (%4)").arg(m_site->url(), m_api->getName(), m_reply->errorString()).arg(m_reply->error()), Logger::Error);
		}
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Parse source
	const int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	parse(source, statusCode, m_site);
}
