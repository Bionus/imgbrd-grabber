#include "login/oauth2-login.h"
#include <QByteArray>
#include <QCoreApplication>
#include <QDataStream>
#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QRandomGenerator>
#include <QUrlQuery>
#include "auth/oauth2-auth.h"
#include "logger.h"
#include "login/site-network-access-manager.h"
#include "mixed-settings.h"
#include "models/site.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#ifdef Q_OS_WIN
	#include "windows-url-protocol.h"
#endif


OAuth2Login::OAuth2Login(OAuth2Auth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_auth(auth), m_site(site), m_manager(manager), m_settings(settings)
{
	m_accessToken = m_settings->value("auth/accessToken").toString();
	m_refreshToken = m_settings->value("auth/refreshToken").toString();
	m_expires = m_settings->value("auth/accessTokenExpiration").toDateTime();
}

bool OAuth2Login::isTestable() const
{
	return !m_auth->tokenUrl().isEmpty()
		&& ((m_auth->authType() != "authorization_code" && m_auth->authType() != "pkce") || !m_auth->authorizationUrl().isEmpty());
}

QString toUrlBase64(const QByteArray &data)
{
	QString ret = data.toBase64();
	ret.replace('+', '-').replace('/', '_').remove(QRegularExpression("=+$"));
	return ret;
}

// https://www.oauth.com/oauth2-servers/access-tokens/client-credentials/
void OAuth2Login::loginClientCredentials()
{
	QList<QStrP> body;
	body << QStrP("grant_type", "client_credentials");

	postRequest(body);
}

// https://www.oauth.com/oauth2-servers/access-tokens/password-grant/
void OAuth2Login::loginPassword()
{
	const QString pseudo = m_settings->value("auth/pseudo").toString();
	const QString password = m_settings->value("auth/password").toString();

	// Fix for Pixiv (issue #1765)
	// TODO(Bionus): do this correctly in the JS file
	QMap<QString, QByteArray> headers;
	QString time = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
	QString hash = time + "28c1fdd170a5204386cb1313c7077b34f83e4aaf4aa829ce78c231e05b0bae2c";
	headers.insert("X-Client-Time", time.toLatin1());
	headers.insert("X-Client-Hash", QCryptographicHash::hash(hash.toLatin1(), QCryptographicHash::Md5).toHex());

	QList<QStrP> body;
	body << QStrP("grant_type", "password");
	body << QStrP("username", pseudo);
	body << QStrP("password", password);

	postRequest(body, headers);
}

// Similar to "password" but using a JSON body instead
void OAuth2Login::loginPasswordJson()
{
	QNetworkRequest request(m_site->fixUrl(m_auth->tokenUrl()));

	// Build headers
	m_site->setRequestHeaders(request);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	// Build body
	QJsonObject bodyJson;
	bodyJson["login"] = m_settings->value("auth/pseudo").toString();
	bodyJson["password"] = m_settings->value("auth/password").toString();
	QJsonDocument jsonDoc(bodyJson);
	const QByteArray data = jsonDoc.toJson();

	// Post request and wait for a reply
	m_tokenReply = m_manager->post(request, data);
	connect(m_tokenReply, &NetworkReply::finished, this, &OAuth2Login::loginFinished);
}

void OAuth2Login::postRequest(QList<QStrP> body, QMap<QString, QByteArray> headers)
{
	QNetworkRequest request(m_site->fixUrl(m_auth->tokenUrl()));

	// Client authentication
	const QString consumerKey = m_settings->value("auth/consumerKey").toString();
	const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();
	if (!consumerKey.isEmpty() && !consumerSecret.isEmpty()) {
		const QString clientAuthentication = m_auth->clientAuthentication();
		if (clientAuthentication == "header") {
			const QByteArray bearerCredentials = QUrl::toPercentEncoding(consumerKey) + ":" + QUrl::toPercentEncoding(consumerSecret);
			const QByteArray base64BearerCredentials = bearerCredentials.toBase64();
			headers.insert("Authorization", "Basic " + base64BearerCredentials);
		} else {
			body << QStrP("client_id", consumerKey);
			body << QStrP("client_secret", consumerSecret);
		}
	}

	// Scopes
	const QStringList scopes = m_auth->scopes();
	if (!scopes.isEmpty()) {
		body << QStrP("scope", scopes.join(' '));
	}

	// Build headers
	m_site->setRequestHeaders(request);
	for (const QString &key : headers.keys()) {
		request.setRawHeader(key.toLatin1(), headers[key]);
	}

	// Build body
	QString bodyStr;
	for (const QStrP &pair : body) {
		bodyStr += (!bodyStr.isEmpty() ? "&" : "") + pair.first + "=" + QUrl::toPercentEncoding(pair.second);
	}
	const QByteArray data = bodyStr.toUtf8();
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");

	// Post request and wait for a reply
	m_tokenReply = m_manager->post(request, data);
	connect(m_tokenReply, &NetworkReply::finished, this, &OAuth2Login::loginFinished);
}

// https://www.oauth.com/oauth2-servers/access-tokens/authorization-code-request/
void OAuth2Login::loginAuthorizationCode()
{
	const QString consumerKey = m_settings->value("auth/consumerKey").toString();
	const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();

	if (consumerKey.isEmpty()) {
		log(QStringLiteral("[%1] You need a client ID to go through the OAuth authorization code flow").arg(m_site->url()), Logger::Warning);
		emit loggedIn(Result::Failure);
		return;
	}

	const QString urlProtocol = m_auth->urlProtocol();
	if (!urlProtocol.isEmpty()) {
		#ifdef Q_OS_WIN
			if (protocolExists(urlProtocol)) {
				protocolUninstall(urlProtocol);
			}
			protocolInstall(urlProtocol, QStringLiteral(R"("%1" --url-protocol "%2")").arg(QDir::toNativeSeparators(qApp->applicationFilePath()), "%1"));
		#else
			log(QStringLiteral("[%1] This OAuth 2 login requires a custom URL protocol, which is only supported on Windows").arg(m_site->url()), Logger::Error);
		#endif
	}

	auto *manager = new SiteNetworkAccessManager(m_site, this);
	auto *flow = new QOAuth2AuthorizationCodeFlow(manager, this);
	flow->setClientIdentifier(consumerKey);
	flow->setClientIdentifierSharedKey(consumerSecret);
	flow->setAuthorizationUrl(m_site->fixUrl(m_auth->authorizationUrl()));
	flow->setAccessTokenUrl(m_site->fixUrl(m_auth->tokenUrl()));

	const QStringList scopes = m_auth->scopes();
	if (!scopes.isEmpty()) {
		flow->setScope(scopes.join(' '));
	}

	auto *replyHandler = new QOAuthHttpServerReplyHandler(58923, this);
	flow->setReplyHandler(replyHandler);

	// Ensure the response handler HTTP server could be started properly
	if (!replyHandler->isListening()) {
		log(QStringLiteral("[%1] OAuth2 HTTP handler not started").arg(m_site->url()), Logger::Error);
		emit loggedIn(Result::Failure);

		flow->deleteLater();
		manager->deleteLater();
		replyHandler->deleteLater();
		return;
	}
	log(QStringLiteral("[%1] OAuth2 HTTP handler listening on port %2").arg(m_site->url(), QString::number(replyHandler->port())), Logger::Info);

	// Detect when the connection succeeded
	QObject::connect(flow, &QOAuth2AuthorizationCodeFlow::statusChanged, [=](QAbstractOAuth::Status status) {
		if (status == QAbstractOAuth::Status::Granted) {
			#ifdef Q_OS_WIN
				if (!urlProtocol.isEmpty()) {
					protocolUninstall(urlProtocol);
				}
			#endif

			m_accessToken = flow->token();
			m_refreshToken = flow->refreshToken();
			m_expires = flow->expirationAt();
			m_settings->setValue("auth/accessToken", m_accessToken);
			m_settings->setValue("auth/refreshToken", m_refreshToken);
			m_settings->setValue("auth/accessTokenExpiration", m_expires);

			emit loggedIn(Result::Success);

			flow->deleteLater();
			manager->deleteLater();
			replyHandler->deleteLater();
		}
	});

	// Generate a base64 from 32 random bytes
	QByteArray byteVerifier;
	QDataStream stream(&byteVerifier, QIODevice::WriteOnly);
	for (int i = 0; i < 8; ++i) {
		stream << QRandomGenerator::global()->generate();
	}
	const QString codeVerifier = toUrlBase64(byteVerifier);

	// PKCE challenge
	if (m_auth->authType() == "pkce") {
		flow->setModifyParametersFunction([=](QAbstractOAuth::Stage stage, QMultiMap<QString, QVariant> *parameters) {
			if (stage == QAbstractOAuth::Stage::RequestingAuthorization) {
				const QString codeChallenge = toUrlBase64(QCryptographicHash::hash(codeVerifier.toLatin1(), QCryptographicHash::Sha256));

				parameters->insert("code_challenge", codeChallenge);
				parameters->insert("code_challenge_method", "S256");

				// TODO(Bionus): do this correctly in the JS file
				parameters->insert("client", "pixiv-android");
			}
			if (stage == QAbstractOAuth::Stage::RequestingAccessToken) {
				parameters->insert("code_verifier", codeVerifier);
				parameters->insert("include_policy", true);

				const QString redirectUrl = m_auth->redirectUrl();
				if (!redirectUrl.isEmpty()) {
					parameters->insert("redirect_uri", m_site->fixUrl(redirectUrl).toString(QUrl::FullyEncoded));
				}
			}
		});
	}

	// Open browser when necessary
	connect(flow, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, [=](const QUrl &url) {
		log(QStringLiteral("[%1] Login with OAuth2 via browser `%2`").arg(m_site->url(), url.toString()), Logger::Info);

		// Override OAuth 2 "state" security for sites that don't support it
		disconnect(replyHandler, &QAbstractOAuthReplyHandler::callbackReceived, flow, &QOAuth2AuthorizationCodeFlow::authorizationCallbackReceived);
		connect(replyHandler, &QAbstractOAuthReplyHandler::callbackReceived, [=](QVariantMap values) {
			values.insert("state", QUrlQuery(url).queryItemValue("state"));
			flow->authorizationCallbackReceived(values);
		});

		QDesktopServices::openUrl(url);
	});

	flow->grant();
}

void OAuth2Login::login()
{
	const QDateTime now = QDateTime::currentDateTime();
	if (!m_refreshToken.isEmpty() && (!m_expires.isValid() || m_expires < now)) {
		refresh(true);
		return;
	}

	if (!m_accessToken.isEmpty()) {
		emit loggedIn(Result::Success);
		return;
	}

	const QString type = m_auth->authType();
	if (type == "client_credentials") {
		loginClientCredentials();
	} else if (type == "password") {
		loginPassword();
	} else if (type == "password_json") {
		loginPasswordJson();
	} else if (type == "refresh_token") {
		refresh(true);
	} else if (type == "authorization_code" || type == "pkce") {
		loginAuthorizationCode();
	}
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
	// Don't try to refresh while a refresh is already in progress
	if (m_refreshing) {
		if (login) {
			m_refreshForLogin = true;
		}
		return;
	}

	log(QStringLiteral("[%1] Refreshing OAuth2 token...").arg(m_site->url()), Logger::Info);

	// Without a refresh token, there's nothing to do
	if (m_refreshToken.isEmpty()) {
		log(QStringLiteral("[%1] Cannot refresh OAuth2 token without a refresh token").arg(m_site->url()), Logger::Warning);
		if (login) {
			emit loggedIn(Result::Failure);
		}
		m_refreshing = false;
		return;
	}

	// Set the refresh status and block other refresh requests until this one is completed
	m_refreshing = true;
	m_refreshForLogin = login;

	QNetworkRequest request(m_site->fixUrl(m_auth->tokenUrl()));
	m_site->setRequestHeaders(request);

	// Build body
	QByteArray data;
	if (m_auth->authType() == "password_json") {
		QJsonObject bodyJson;
		bodyJson["refresh_token"] = m_refreshToken;
		QJsonDocument jsonDoc(bodyJson);

		data = jsonDoc.toJson();
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	} else {
		const QString consumerKey = m_settings->value("auth/consumerKey").toString();
		const QString consumerSecret = m_settings->value("auth/consumerSecret").toString();
		const QList<QStrP> body {
			{ "grant_type", "refresh_token" },
			{ "client_id", consumerKey },
			{ "client_secret", consumerSecret },
			{ "refresh_token", m_refreshToken }
		};

		QString bodyStr;
		for (const QStrP &pair : body) {
			bodyStr += (!bodyStr.isEmpty() ? "&" : "") + pair.first + "=" + pair.second;
		}

		data = bodyStr.toUtf8();
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded;charset=UTF-8");
	}

	// Post request and wait for a reply
	m_refreshReply = m_manager->post(request, data);
	connect(m_refreshReply, &NetworkReply::finished, this, &OAuth2Login::refreshFinished);
}

void OAuth2Login::refreshFinished()
{
	const bool ok = readResponse(m_refreshReply);
	m_refreshing = false;
	if (!m_refreshForLogin) {
		return;
	}
	if (!ok) {
		if (m_auth->authType() == "refresh_token") {
			log(QStringLiteral("[%1] Refresh failed").arg(m_site->url()), Logger::Warning);
			emit loggedIn(Result::Failure);
			return;
		}

		log(QStringLiteral("[%1] Refresh failed, clearing tokens and re-trying login...").arg(m_site->url()), Logger::Warning);
		m_accessToken.clear();
		m_settings->remove("auth/accessToken");
		m_refreshToken.clear();
		m_settings->remove("auth/refreshToken");
		m_expires = QDateTime();
		m_settings->remove("auth/accessTokenExpiration");
		login();
	} else {
		emit loggedIn(Result::Success);
	}
}

bool OAuth2Login::readResponse(NetworkReply *reply)
{
	const QString result = reply->readAll();
	QJsonParseError parseError;
	const QJsonDocument jsonDocument = QJsonDocument::fromJson(result.toUtf8(), &parseError);

	// Ensure we got a proper JSON
	if (jsonDocument.isNull()) {
		const QString extract = (parseError.offset < 100 ? "" : "...") + result.mid(parseError.offset - 100, 200) + (parseError.offset > result.length() - 100 ? "" : "...");
		log(QStringLiteral("[%1] Error parsing JSON response: %2 at position %3 - %4").arg(m_site->url(), parseError.errorString()).arg(parseError.offset).arg(extract), Logger::Warning);
		return false;
	}

	// Some OAuth2 API wrap their responses in 'response' JSON objects
	QJsonObject jsonObject = jsonDocument.object();
	if (!jsonObject.contains("token_type") && jsonObject.contains("response")) {
		jsonObject = jsonObject.value("response").toObject();
	}

	const QJsonValue tokenType = jsonObject.value("token_type");
	if (tokenType.isUndefined()) {
		const QJsonValue error = jsonObject.value("error");
		if (!error.isUndefined()) {
			log(QStringLiteral("[%1] Error during OAuth2 login: %2").arg(m_site->url(), error.toString()), Logger::Warning);
		} else {
			log(QStringLiteral("[%1] No OAuth2 token type received: %2").arg(m_site->url(), result), Logger::Warning);
		}
		return false;
	}
	if (tokenType.toString().toLower() != QLatin1String("bearer")) {
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
		}
		if (m_refreshToken.count('.') == 2) {
			const QStringList parts = m_refreshToken.split('.');
			const QJsonDocument jsonPayloadDoc = QJsonDocument::fromJson(QByteArray::fromBase64(parts[1].toUtf8()));
			if (!jsonPayloadDoc.isNull()) {
				const QJsonObject jsonPayload = jsonPayloadDoc.object();
				const QJsonValue jsonExp = jsonPayload.value("exp");
				if (!jsonExp.isUndefined()) {
					m_expires = QDateTime::fromSecsSinceEpoch(jsonExp.toInt(), Qt::UTC);
				}
			}
		}

		if (!m_expires.isNull()) {
			const int expiresSecond = QDateTime::currentDateTime().secsTo(m_expires);
			QTimer::singleShot((expiresSecond / 2) * 1000, this, SIGNAL(basicRefresh()));
			log(QStringLiteral("[%1] Token will expire at '%2'").arg(m_site->url(), m_expires.toString("yyyy-MM-dd HH:mm:ss")), Logger::Debug);
			m_settings->setValue("auth/accessTokenExpiration", m_expires);
		}
	}

	return true;
}

void OAuth2Login::complementRequest(QNetworkRequest *request) const
{
	// Trigger a token refresh in the background if the token is expired
	if (!m_refreshToken.isEmpty() && (!m_expires.isValid() || m_expires < QDateTime::currentDateTime())) {
		const_cast<OAuth2Login*>(this)->refresh(false);
	}

	if (!m_accessToken.isEmpty()) {
		request->setRawHeader("Authorization", "Bearer " + m_accessToken.toUtf8());
	}
}
