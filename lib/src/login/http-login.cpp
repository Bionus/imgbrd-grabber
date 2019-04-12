#include "login/http-login.h"
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QUrlQuery>
#include "auth/auth-field.h"
#include "auth/http-auth.h"
#include "mixed-settings.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#include "models/site.h"


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

	QUrlQuery query;
	for (AuthField *field : m_auth->fields()) {
		if (!field->key().isEmpty()) {
			query.addQueryItem(field->key(), field->value(m_settings));
		}
	}

	if (m_loginReply != nullptr) {
		m_loginReply->abort();
		m_loginReply->deleteLater();
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
	const auto status = hasCookie(m_loginReply->url())
		? Result::Success
		: Result::Failure;

	emit loggedIn(status);
}

bool HttpLogin::hasCookie(const QUrl &url) const
{
	const QString cookieName = m_auth->cookie();

	QNetworkCookieJar *cookieJar = m_manager->cookieJar();
	QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(url);

	for (const QNetworkCookie &cookie : cookies) {
		if (cookie.name() == cookieName && !cookie.value().isEmpty() && cookie.value() != "0") {
			return true;
		}
	}

	return false;
}
