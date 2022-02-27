#include "login/oauth1-login.h"
#include <QDesktopServices>
#include <QEventLoop>
#include <QOAuthHttpServerReplyHandler>
#include "auth/oauth1-auth.h"
#include "logger.h"
#include "login/qoauth1-setup.h"
#include "mixed-settings.h"
#include "models/site.h"
#include "network/network-manager.h"


using QStrP = QPair<QString, QString>;

OAuth1Login::OAuth1Login(OAuth1Auth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_auth(auth), m_site(site), m_manager(manager), m_settings(settings)
{
	m_oauth1 = new QOAuth1Setup(new QNetworkAccessManager(), this);

	// Signature method
	const QString signatureMethod = m_auth->signatureMethod();
	if (signatureMethod == "hmac-sha1") {
		m_oauth1->setSignatureMethod(QOAuth1::SignatureMethod::Hmac_Sha1);
	} else if (signatureMethod == "plaintext") {
		m_oauth1->setSignatureMethod(QOAuth1::SignatureMethod::PlainText);
	}

	// Create OAuth 1 client with proper API keys
	const QString apiKey = m_settings->value("auth/consumerKey").toString();
	const QString apiSecretKey = m_settings->value("auth/consumerSecret").toString();
	m_oauth1->setClientCredentials(apiKey, apiSecretKey);

	// Pass existing access token if present
	const QString accessToken = m_settings->value("auth/accessToken").toString();
	const QString accessTokenSecret = m_settings->value("auth/accessTokenSecret").toString();
	m_oauth1->setTokenCredentials(accessToken, accessTokenSecret);

	// Setup URLs
	m_oauth1->setTemporaryCredentialsUrl(m_site->fixUrl(m_auth->temporaryCredentialsUrl()));
	m_oauth1->setAuthorizationUrl(m_site->fixUrl(m_auth->authorizationUrl()));
	m_oauth1->setTokenCredentialsUrl(m_site->fixUrl(m_auth->tokenCredentialsUrl()));

	// Automatically open browser when necessary
	QObject::connect(m_oauth1, &QOAuth1::authorizeWithBrowser, [=](QUrl url) {
		log(QStringLiteral("[%1] Login with OAuth1 via browser `%2`").arg(m_site->url(), url.toString()), Logger::Info);
		QDesktopServices::openUrl(url);
	});
}

bool OAuth1Login::isTestable() const
{
	return !m_auth->temporaryCredentialsUrl().isEmpty()
		&& !m_auth->authorizationUrl().isEmpty()
		&& !m_auth->tokenCredentialsUrl().isEmpty();
}

void OAuth1Login::login()
{
	// Start a response handler
	auto *replyHandler = dynamic_cast<QOAuthHttpServerReplyHandler *>(m_oauth1->replyHandler());
	if (replyHandler == nullptr) {
		replyHandler = new QOAuthHttpServerReplyHandler(58923, this);
		replyHandler->setCallbackPath("callback");
		m_oauth1->setReplyHandler(replyHandler);
	}

	// Ensure the response handler HTTP server could be started properly
	if (!replyHandler->isListening()) {
		log(QStringLiteral("[%1] OAuth1 HTTP handler not started").arg(m_site->url()), Logger::Error);
		emit loggedIn(Result::Failure);
		return;
	}
	log(QStringLiteral("[%1] OAuth1 HTTP handler listening on port %2, with callback `%3`").arg(m_site->url(), QString::number(replyHandler->port()), replyHandler->callback()), Logger::Info);

	// Actual login call
	connect(m_oauth1, &QOAuth1::granted, [this, replyHandler]() {
		QPair<QString, QString> credentials = m_oauth1->tokenCredentials();
		m_settings->setValue("auth/accessToken", credentials.first);
		m_settings->setValue("auth/accessTokenSecret", credentials.second);

		emit loggedIn(m_oauth1->status() == QOAuth1::Status::Granted ? Result::Success : Result::Failure);
		replyHandler->deleteLater();
	});
	m_oauth1->grant();
}

void OAuth1Login::complementRequest(QNetworkRequest *request) const
{
	m_oauth1->publicSetup(request, {}, QNetworkAccessManager::GetOperation);
}
