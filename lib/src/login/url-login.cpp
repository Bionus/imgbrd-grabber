#include "login/url-login.h"
#include <QCryptographicHash>
#include "mixed-settings.h"
#include "models/api/api.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


UrlLogin::UrlLogin(Site *site, QNetworkAccessManager *manager, MixedSettings *settings)
	: m_site(site), m_manager(manager), m_settings(settings), m_page(nullptr)
{}

bool UrlLogin::isTestable() const
{
	return m_settings->value("login/maxPage", 0).toInt() > 0;
}

void UrlLogin::login()
{
	if (m_page != nullptr)
	{
		m_page->abort();
		m_page->deleteLater();
	}

	const int maxPageAnonymous = m_settings->value("login/maxPage", 0).toInt();
	m_page = new Page(m_site->getSource()->getProfile(), m_site, QList<Site*>() << m_site, QStringList(), maxPageAnonymous);
	connect(m_page, &Page::finishedLoading, this, &UrlLogin::loginFinished);
	connect(m_page, &Page::failedLoading, this, &UrlLogin::loginFinished);
	m_page->load();
}

void UrlLogin::loginFinished()
{
	if (!m_page->images().isEmpty())
	{
		emit loggedIn(Result::Success);
		return;
	}

	emit loggedIn(Result::Failure);
}

QString UrlLogin::complementUrl(QString url, const QString &loginPart) const
{
	QString pseudo = m_settings->value("auth/pseudo").toString();
	QString password = m_settings->value("auth/password").toString();

	const bool hasLoginString = !loginPart.isEmpty() && (!pseudo.isEmpty() || !password.isEmpty());
	url.replace("{login}", hasLoginString ? loginPart : QString());

	// Basic GET auth
	url.replace("{pseudo}", pseudo);
	url.replace("{password}", password);

	// Appkey GET auth
	if (url.contains("{appkey}"))
	{
		QString appkey = m_site->getApis().first()->value("AppkeySalt");
		appkey.replace("%password%", password);
		appkey.replace("%username%", pseudo.toLower());
		url.replace("{appkey}", QCryptographicHash::hash(appkey.toUtf8(), QCryptographicHash::Sha1).toHex());
	}

	return url;
}
