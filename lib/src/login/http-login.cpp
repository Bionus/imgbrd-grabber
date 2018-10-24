#include "login/http-login.h"
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QUrlQuery>
#include "auth/auth-field.h"
#include "auth/http-auth.h"
#include "custom-network-access-manager.h"
#include "mixed-settings.h"
#include "models/site.h"


HttpLogin::HttpLogin(QString type, HttpAuth *auth, Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings)
	: m_type(std::move(type)), m_auth(auth), m_site(site), m_loginReply(nullptr), m_manager(manager), m_settings(settings)
{}

bool HttpLogin::isTestable() const
{
	return !m_auth->url().isEmpty();
}

void HttpLogin::login()
{
	QUrlQuery query;
	for (AuthField *field : m_auth->fields())
	{ query.addQueryItem(field->key(), field->value(m_settings)); }

	if (m_loginReply != nullptr)
	{
		m_loginReply->abort();
		m_loginReply->deleteLater();
	}

	QNetworkReply *reply = getReply(m_auth->url(), query);
	if (reply == nullptr)
	{
		emit loggedIn(Result::Failure);
		return;
	}

	m_loginReply = reply;
	connect(m_loginReply, &QNetworkReply::finished, this, &HttpLogin::loginFinished);
}

void HttpLogin::loginFinished()
{
	const QString cookieName = m_settings->value("login/" + m_type + "/cookie").toString();

	QNetworkCookieJar *cookieJar = m_manager->cookieJar();
	QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(m_loginReply->url());

	for (const QNetworkCookie &cookie : cookies)
	{
		if (cookie.name() == cookieName && !cookie.value().isEmpty() && cookie.value() != "0")
		{
			emit loggedIn(Result::Success);
			return;
		}
	}

	emit loggedIn(Result::Failure);
}
