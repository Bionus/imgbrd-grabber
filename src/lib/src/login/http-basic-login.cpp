#include "login/http-basic-login.h"
#include "auth/http-basic-auth.h"
#include "mixed-settings.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "network/network-reply.h"


HttpBasicLogin::HttpBasicLogin(HttpBasicAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_auth(auth), m_site(site), m_manager(manager), m_settings(settings)
{}

bool HttpBasicLogin::isTestable() const
{
	return m_auth->maxPage() > 0;
}

void HttpBasicLogin::login()
{
	if (m_page != nullptr) {
		if (!m_page->isLoaded()) {
			return;
		}

		m_page->abort();
		m_page->deleteLater();
	}

	const int maxPageAnonymous = m_auth->maxPage();
	m_page = new Page(m_site->getSource()->getProfile(), m_site, QList<Site*>() << m_site, QStringList(), maxPageAnonymous);
	connect(m_page, &Page::finishedLoading, this, &HttpBasicLogin::loginFinished);
	connect(m_page, &Page::failedLoading, this, &HttpBasicLogin::loginFinished);
	m_page->load();
}

void HttpBasicLogin::loginFinished()
{
	if (!m_page->images().isEmpty()) {
		emit loggedIn(Result::Success);
		return;
	}

	emit loggedIn(Result::Failure);
}

void HttpBasicLogin::complementRequest(QNetworkRequest *request) const
{
	const QString username = m_settings->value("auth/pseudo").toString();
	const QString password = m_settings->value("auth/" + m_auth->passwordType()).toString();

	const QString credentials = username + ":" + password;
	const QByteArray base64Credentials = credentials.toLocal8Bit().toBase64();
	request->setRawHeader("Authorization", "Basic " + base64Credentials);
}
