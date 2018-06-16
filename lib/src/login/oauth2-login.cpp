#include "login/oauth2-login.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include "logger.h"
#include "mixed-settings.h"
#include "models/site.h"


OAuth2Login::OAuth2Login(Site *site, QNetworkAccessManager *manager, MixedSettings *settings)
	: m_site(site), m_manager(manager), m_settings(settings)
{}

bool OAuth2Login::isTestable() const
{
	return !m_settings->value("login/oauth2/tokenUrl").toString().isEmpty();
}

void OAuth2Login::login()
{
	// Get user application credentials
	QString consumerKey = m_settings->value("auth/consumerKey").toString();
	QString consumerSecret = m_settings->value("auth/consumerSecret").toString();
	QByteArray bearerCredentials = QUrl::toPercentEncoding(consumerKey) + ":" + QUrl::toPercentEncoding(consumerSecret);
	QByteArray base64BearerCredentials = bearerCredentials.toBase64();

	// Create request
	QNetworkRequest request(m_site->fixUrl(m_settings->value("login/oauth2/tokenUrl").toString()));
	request.setRawHeader("Authorization", "Basic " + base64BearerCredentials);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
	QString body = QStringLiteral("grant_type=client_credentials");

	// Post request and wait for a reply
	m_tokenReply = m_manager->post(request, body.toUtf8());
	connect(m_tokenReply, &QNetworkReply::finished, this, &OAuth2Login::loginFinished);
}

void OAuth2Login::loginFinished()
{
	QString result = m_tokenReply->readAll();
	QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toUtf8());
	QJsonObject jsonObject = jsonDocument.object();

	if (jsonObject.value("token_type").toString() == QLatin1String("bearer"))
	{
		m_token = jsonObject.value("access_token").toString();
		emit loggedIn(Result::Success);
		return;
	}
	else
	{ log(QStringLiteral("[%1] Wrong OAuth2 token type received.").arg(m_site->url())); }

	emit loggedIn(Result::Failure);
}

void OAuth2Login::complementRequest(QNetworkRequest *request) const
{
	if (!m_token.isEmpty())
		request->setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
}
