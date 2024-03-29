#include "login/http-login.h"
#include <QEventLoop>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QUrlQuery>
#include <utility>
#include "auth/auth-field.h"
#include "auth/http-auth.h"
#include "mixed-settings.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#include "models/site.h"
#include "logger.h"
#include "utils/html-node.h"


HttpLogin::HttpLogin(QString type, HttpAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_type(std::move(type)), m_auth(auth), m_site(site), m_loginReply(nullptr), m_manager(manager), m_settings(settings)
{}

bool HttpLogin::isTestable() const
{
	return !m_auth->url().isEmpty();
}

void HttpLogin::login()
{
	const QUrl url = m_site->fixUrl(m_auth->url());

	if (hasCookie(url)) {
		emit loggedIn(Result::Success);
		return;
	}

	if (m_loginReply != nullptr) {
		m_loginReply->abort();
		m_loginReply->deleteLater();
	}

	QUrlQuery query;
	for (AuthField *field : m_auth->fields()) {
		if (!field->key().isEmpty()) {
			query.addQueryItem(field->key(), field->value(m_settings));
		}
	}

	if (!m_auth->csrfUrl().isEmpty()) {
		QEventLoop loop;
		NetworkReply *reply = m_site->get(m_site->fixUrl(m_auth->csrfUrl()), Site::QueryType::UnknownType);
		QObject::connect(reply, &NetworkReply::finished, &loop, &QEventLoop::quit);
		loop.exec();

		// Loading error
		if (reply->error() != NetworkReply::NetworkError::NoError) {
			log(QStringLiteral("Error loading CSRF information (%1)").arg(reply->errorString()), Logger::Error);
			emit loggedIn(Result::Failure);
			return;
		}

		const QString src = reply->readAll();
		const QStringList fields = m_auth->csrfFields();
		HtmlNode *document = HtmlNode::fromString(src);
		for (const QString &field : fields) {
			const QList<HtmlNode> input = document->find("input[name=" + field + "]");
			if (input.isEmpty()) {
				log(QStringLiteral("Could not find HTML field '%1'").arg(field), Logger::Warning);
				continue;
			}
			query.addQueryItem(field, input.first().attr("value"));
		}
		delete document;
	}

	NetworkReply *reply = getReply(url, query);
	if (reply == nullptr) {
		emit loggedIn(Result::Failure);
		return;
	}

	m_loginReply = reply;
	connect(m_loginReply, &NetworkReply::finished, this, &HttpLogin::loginFinished);
}

void HttpLogin::loginFinished()
{
	// "cookie" check
	bool success = hasCookie(m_loginReply->url());

	// "redirect" check
	const QString redirectCheck = m_auth->redirectUrl();
	if (!redirectCheck.isEmpty()) {
		const QUrl redirection = m_loginReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		success = redirection.toString().contains(redirectCheck);
	}

	emit loggedIn(success ? Result::Success : Result::Failure);
}

bool HttpLogin::hasCookie(const QUrl &url) const
{
	const QString cookieName = m_auth->cookie();
	if (cookieName.isEmpty()) {
		return false;
	}

	const QNetworkCookieJar *cookieJar = m_manager->cookieJar();
	const QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(url);

	for (const QNetworkCookie &cookie : cookies) {
		if (cookie.name() == cookieName && !cookie.value().isEmpty() && cookie.value() != "0") {
			return true;
		}
	}

	return false;
}
