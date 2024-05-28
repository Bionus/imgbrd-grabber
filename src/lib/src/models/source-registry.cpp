#include "models/source-registry.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <utility>
#include "js-helpers.h"
#include "logger.h"
#include "network/network-manager.h"
#include "network/network-reply.h"


SourceRegistry::SourceRegistry(QString jsonUrl, QObject *parent)
	: QObject(parent), m_jsonUrl(std::move(jsonUrl))
{
	m_manager = new NetworkManager(this);
}

void SourceRegistry::load()
{
	QNetworkRequest request(m_jsonUrl);
	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

	NetworkReply *reply = m_manager->get(request);
	connect(reply, &NetworkReply::finished, this, &SourceRegistry::jsonLoaded);
}

void SourceRegistry::jsonLoaded()
{
	auto *reply = qobject_cast<NetworkReply*>(sender());

	// Loading error
	if (reply->error() != NetworkReply::NetworkError::NoError) {
		log(QStringLiteral("Error loading source registry metadata (%1) from `%2`").arg(reply->errorString(), m_jsonUrl), Logger::Error);
		m_isValid = false;
		emit loaded(false);
		return;
	}

	// Parse JSON response
	QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
	QJsonObject object = jsonDoc.object();

	// General metadata
	m_name = object["name"].toString();
	m_home = object["home"].toString();
	m_url = object["url"].toString();

	// Supported sources
	QJsonArray sources = object["sources"].toArray();
	for (auto sourceJson : sources) {
		QJsonObject sourceObj = sourceJson.toObject();
		QJsonObject lastCommitObj = sourceObj["lastCommit"].toObject();

		SourceRegistrySource source;
		source.slug = sourceObj["slug"].toString();
		source.name = sourceObj["name"].toString();
		source.hash = sourceObj["hash"].toString();
		source.lastCommit.hash = lastCommitObj["hash"].toString();
		source.lastCommit.author = lastCommitObj["author"].toString();
		source.lastCommit.date = lastCommitObj["date"].toString();
		source.lastCommit.subject = lastCommitObj["subject"].toString();

		QJsonArray defaultSites = sourceObj["defaultSites"].toArray();
		source.defaultSites.reserve(defaultSites.count());
		for (auto site : defaultSites) {
			source.defaultSites.append(site.toString());
		}

		QJsonArray supportedSites = sourceObj["supportedSites"].toArray();
		source.supportedSites.reserve(supportedSites.count());
		for (auto site : supportedSites) {
			source.supportedSites.append(site.toString());
		}

		m_sources.insert(source.name, source);
	}

	m_isValid = true;
	emit loaded(true);
}
