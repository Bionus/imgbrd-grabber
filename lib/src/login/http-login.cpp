#include "login/http-login.h"
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QUrlQuery>
#include "custom-network-access-manager.h"
#include "mixed-settings.h"


HttpLogin::HttpLogin(QString type, Site *site, CustomNetworkAccessManager *manager, MixedSettings *settings)
	: m_type(std::move(type)), m_site(site), m_loginReply(nullptr), m_manager(manager), m_settings(settings)
{}

bool HttpLogin::isTestable() const
{
	return !m_settings->value("login/" + m_type + "/url").toString().isEmpty();
}

void HttpLogin::login()
{
	const QString username = m_settings->value("auth/pseudo").toString();
	const QString password = m_settings->value("auth/password").toString();

	QUrlQuery query;
	query.addQueryItem(m_settings->value("login/" + m_type + "/pseudo").toString(), username);
	query.addQueryItem(m_settings->value("login/" + m_type + "/password").toString(), password);

	m_settings->beginGroup("login/fields");
		QStringList keys = m_settings->childKeys();
		for (const QString &key : keys)
		{ query.addQueryItem(key, m_settings->value(key).toString()); }
	m_settings->endGroup();

	if (m_loginReply != nullptr)
	{
		m_loginReply->abort();
		m_loginReply->deleteLater();
	}

	const QString loginUrl = m_settings->value("login/" + m_type + "/url").toString();
	m_loginReply = getReply(loginUrl, query);

	connect(m_loginReply, &QNetworkReply::finished, this, &HttpLogin::loginFinished);
}

void HttpLogin::loginFinished()
{
	const QString cookieName = m_settings->value("login/" + m_type + "/cookie").toString();

	QNetworkCookieJar *cookieJar = m_manager->cookieJar();
	QList<QNetworkCookie> cookies = cookieJar->cookiesForUrl(m_loginReply->url());

	for (const QNetworkCookie &cookie : cookies)
	{
		if (cookie.name() == cookieName && !cookie.value().isEmpty())
		{
			emit loggedIn(Result::Success);
			return;
		}
	}

	emit loggedIn(Result::Failure);
}
