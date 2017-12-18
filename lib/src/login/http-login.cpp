#include "login/http-login.h"
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QUrlQuery>
#include "models/site.h"


HttpLogin::HttpLogin(const QString &type, Site *site, QNetworkAccessManager *manager, QNetworkCookieJar *cookieJar, MixedSettings *settings)
	: m_type(type), m_site(site), m_manager(manager), m_cookieJar(cookieJar), m_settings(settings)
{}

bool HttpLogin::isTestable() const
{
	return !m_settings->value("login/" + m_type + "/url", "").toString().isEmpty();
}

void HttpLogin::login()
{
	QString username = m_settings->value("auth/pseudo", "").toString();
	QString password = m_settings->value("auth/password", "").toString();

	QUrlQuery query;
	query.addQueryItem(m_settings->value("login/" + m_type + "/pseudo", "").toString(), username);
	query.addQueryItem(m_settings->value("login/" + m_type + "/password", "").toString(), password);

	m_settings->beginGroup("login/fields");
		QStringList keys = m_settings->childKeys();
		for (const QString &key : keys)
		{ query.addQueryItem(key, m_settings->value(key).toString()); }
	m_settings->endGroup();

	QString loginUrl = m_settings->value("login/" + m_type + "/url", "").toString();
	m_loginReply = getReply(loginUrl, query);

	connect(m_loginReply, &QNetworkReply::finished, this, &HttpLogin::loginFinished);
}

void HttpLogin::loginFinished()
{
	QString cookieName = m_settings->value("login/" + m_type + "/cookie", "").toString();

	QList<QNetworkCookie> cookies = m_cookieJar->cookiesForUrl(m_loginReply->url());
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
