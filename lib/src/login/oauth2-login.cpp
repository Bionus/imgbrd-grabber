#include "login/oauth2-login.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "auth/oauth2-auth.h"
#include "logger.h"
#include "mixed-settings.h"
#include "models/site.h"
#include "network/network-manager.h"
#include "network/network-reply.h"


using QStrP = QPair<QString, QString>;

OAuth2Login::OAuth2Login(OAuth2Auth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_auth(auth), m_site(site), m_manager(manager), m_settings(settings)
{
	m_accessToken = m_settings->value("auth/accessToken").toString();
	m_refreshToken = m_settings->value("auth/refreshToken").toString();
}

bool OAuth2Login::isTestable() const
{
	return !m_auth->tokenUrl().isEmpty();
}

void OAuth2Login::login()
{
	QDateTime now = QDateTime::currentDateTime();
	if (!m_refreshToken.isEmpty() && (!m_expires.isValid() || m_expires < now)) {
		refresh(true);
		return;
	}

	if (!m_accessToken.isEmpty()) {
		emit loggedIn(Result::Success);
		return;
	}

	const QString type = m_auth->authType();
	const QString consumerKey = m_settings->value("auth/consumerKey").toString();
	const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();

	QNetworkRequest request(m_site->fixUrl(m_auth->tokenUrl()));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");

	QList<QStrP> body;

	if (type == "header_basic") {
		body << QStrP("grant_type", "client_credentials");

		const QByteArray bearerCredentials = QUrl::toPercentEncoding(consumerKey) + ":" + QUrl::toPercentEncoding(consumerSecret);
		const QByteArray base64BearerCredentials = bearerCredentials.toBase64();
		request.setRawHeader("Authorization", "Basic " + base64BearerCredentials);
	} else if (type == "client_credentials") {
		body << QStrP("grant_type", "client_credentials")
			 << QStrP("client_id", consumerKey)
			 << QStrP("client_secret", consumerSecret);
	} else if (type == "password") {
		const QString pseudo = m_settings->value("auth/pseudo").toString();
		const QString password = m_settings->value("auth/password").toString();

		body << QStrP("grant_type", "password")
			 << QStrP("username", pseudo)
			 << QStrP("password", password);

		if (!consumerKey.isEmpty()) {
			body << QStrP("client_id", consumerKey);
			if (!consumerSecret.isEmpty()) {
				body << QStrP("client_secret", consumerSecret);
			}
		}
	}

	// Post request and wait for a reply
	QString bodyStr;
	for (const QStrP &pair : body) {
		bodyStr += (!bodyStr.isEmpty() ? "&" : "") + pair.first + "=" + pair.second;
	}
	m_tokenReply = m_manager->post(request, bodyStr.toUtf8());
	connect(m_tokenReply, &NetworkReply::finished, this, &OAuth2Login::loginFinished);
}

void OAuth2Login::loginFinished()
{
	const auto result = readResponse(m_tokenReply) ? Result::Success : Result::Failure;
	emit loggedIn(result);
}

void OAuth2Login::basicRefresh()
{
	refresh(false);
}

void OAuth2Login::refresh(bool login)
{
	log(QStringLiteral("[%1] Refreshing OAuth2 token...").arg(m_site->url()), Logger::Info);

	const QString type = m_auth->authType();
	const QString consumerKey = m_settings->value("auth/consumerKey").toString();
	const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();

	QNetworkRequest request(m_site->fixUrl(m_auth->tokenUrl()));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");

	QList<QStrP> body;
	body << QStrP("grant_type", "refresh_token")
		 << QStrP("client_id", consumerKey)
		 << QStrP("client_secret", consumerSecret)
		 << QStrP("refresh_token", m_refreshToken);

	// Post request and wait for a reply
	QString bodyStr;
	for (const QStrP &pair : body) {
		bodyStr += (!bodyStr.isEmpty() ? "&" : "") + pair.first + "=" + pair.second;
	}
	m_refreshReply = m_manager->post(request, bodyStr.toUtf8());

	if (login) {
		connect(m_refreshReply, &NetworkReply::finished, this, &OAuth2Login::refreshLoginFinished);
	} else {
		connect(m_refreshReply, &NetworkReply::finished, this, &OAuth2Login::refreshFinished);
	}
}

void OAuth2Login::refreshLoginFinished()
{
	const auto result = readResponse(m_refreshReply) ? Result::Success : Result::Failure;
	emit loggedIn(result);
}
void OAuth2Login::refreshFinished()
{
	readResponse(m_refreshReply);
}

bool OAuth2Login::readResponse(NetworkReply *reply)
{
	const QString result = reply->readAll();
	const QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toUtf8());

	// Some OAuth2 API wrap their responses in 'response' JSON objects
	QJsonObject jsonObject = jsonDocument.object();
	if (!jsonObject.contains("token_type") && jsonObject.contains("response")) {
		jsonObject = jsonObject.value("response").toObject();
	}

	const QJsonValue tokenType = jsonObject.value("token_type");
	if (tokenType.isUndefined()) {
		log(QStringLiteral("[%1] No OAuth2 token type received: %2").arg(m_site->url(), result), Logger::Warning);
		return false;
	}
	if (tokenType.toString() != QLatin1String("bearer")) {
		log(QStringLiteral("[%1] Wrong OAuth2 token type received (%2).").arg(m_site->url(), tokenType.toString()), Logger::Warning);
		return false;
	}

	m_accessToken = jsonObject.value("access_token").toString();
	m_settings->setValue("auth/accessToken", m_accessToken);
	log(QStringLiteral("[%1] Successfully received OAuth2 access token '%2'").arg(m_site->url(), m_accessToken), Logger::Debug);

	if (jsonObject.contains("refresh_token")) {
		m_refreshToken = jsonObject.value("refresh_token").toString();
		m_settings->setValue("auth/refreshToken", m_refreshToken);
		log(QStringLiteral("[%1] Successfully received OAuth2 refresh token '%2'").arg(m_site->url(), m_refreshToken), Logger::Debug);

		bool expires = jsonObject.contains("expires");
		bool expires_in = jsonObject.contains("expires_in");
		if (expires || expires_in) {
			int expiresSecond = jsonObject.value(expires ? "expires" : "expires_in").toInt();
			m_expires = QDateTime::currentDateTime().addSecs(expiresSecond);
			QTimer::singleShot((expiresSecond / 2) * 1000, this, &OAuth2Login::basicRefresh);
			log(QStringLiteral("[%1] Token will expire at '%2'").arg(m_site->url(), m_expires.toString("yyyy-MM-dd HH:mm:ss")), Logger::Debug);
		}
	}

	return true;
}

void OAuth2Login::complementRequest(QNetworkRequest *request) const
{
	if (!m_accessToken.isEmpty()) {
		request->setRawHeader("Authorization", "Bearer " + m_accessToken.toUtf8());
	}
}
