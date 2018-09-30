#include "login/oauth2-login.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "logger.h"
#include "mixed-settings.h"
#include "models/site.h"


typedef QPair<QString, QString> QStrP;

OAuth2Login::OAuth2Login(Site *site, QNetworkAccessManager *manager, MixedSettings *settings)
	: m_site(site), m_manager(manager), m_settings(settings), m_tokenReply(nullptr)
{}

bool OAuth2Login::isTestable() const
{
	return !m_settings->value("login/oauth2/tokenUrl").toString().isEmpty();
}

void OAuth2Login::login()
{
	const QString type = m_settings->value("login/oauth2/type", "password").toString();
	const QString consumerKey = m_settings->value("auth/consumerKey").toString();
	const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();

	QNetworkRequest request(m_site->fixUrl(m_settings->value("login/oauth2/tokenUrl").toString()));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");

	QList<QStrP> body;

	if (type == "header_basic")
	{
		body << QStrP("grant_type", "client_credentials");

		const QByteArray bearerCredentials = QUrl::toPercentEncoding(consumerKey) + ":" + QUrl::toPercentEncoding(consumerSecret);
		const QByteArray base64BearerCredentials = bearerCredentials.toBase64();
		request.setRawHeader("Authorization", "Basic " + base64BearerCredentials);
	}
	else if (type == "client_credentials")
	{
		body << QStrP("grant_type", "client_credentials")
			 << QStrP("client_id", consumerKey)
			 << QStrP("client_secret", consumerSecret);
	}
	else if (type == "password")
	{
		const QString pseudo = m_settings->value("auth/pseudo").toString();
		const QString password = m_settings->value("auth/password").toString();

		body << QStrP("grant_type", "password")
			 << QStrP("username", pseudo)
			 << QStrP("password", password);

		if (!consumerKey.isEmpty())
		{
			body << QStrP("client_id", consumerKey);
			if (!consumerSecret.isEmpty())
			{ body << QStrP("client_secret", consumerSecret); }
		}
	}

	// Post request and wait for a reply
	QString bodyStr;
	for (const QStrP &pair : body)
	{ bodyStr += (!bodyStr.isEmpty() ? "&" : "") + pair.first + "=" + pair.second; }
	m_tokenReply = m_manager->post(request, bodyStr.toUtf8());
	connect(m_tokenReply, &QNetworkReply::finished, this, &OAuth2Login::loginFinished);
}

void OAuth2Login::loginFinished()
{
	const QString result = m_tokenReply->readAll();
	const QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toUtf8());

	// Some OAuth2 API wrap their responses in 'response' JSON objects
	QJsonObject jsonObject = jsonDocument.object();
	if (!jsonObject.contains("token_type") && jsonObject.contains("response"))
	{ jsonObject = jsonObject.value("response").toObject(); }

	const QJsonValue tokenType = jsonObject.value("token_type");
	if (tokenType.isUndefined())
	{
		log(QStringLiteral("[%1] No OAuth2 token type received: %2").arg(m_site->url(), result), Logger::Warning);
		emit loggedIn(Result::Failure);
		return;
	}
	if (tokenType.toString() != QLatin1String("bearer"))
	{
		log(QStringLiteral("[%1] Wrong OAuth2 token type received (%2).").arg(m_site->url(), tokenType.toString()), Logger::Warning);
		emit loggedIn(Result::Failure);
		return;
	}

	m_token = jsonObject.value("access_token").toString();
	emit loggedIn(Result::Success);
}

void OAuth2Login::complementRequest(QNetworkRequest *request) const
{
	if (!m_token.isEmpty())
		request->setRawHeader("Authorization", "Bearer " + m_token.toUtf8());
}
