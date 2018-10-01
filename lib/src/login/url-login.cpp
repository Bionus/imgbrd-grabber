#include "login/url-login.h"
#include "auth/auth-field.h"
#include "mixed-settings.h"
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

QString UrlLogin::complementUrl(QString url, QList<AuthField*> fields) const
{
	for (AuthField *field : fields)
	{
		url.append("&" + field->key() + "=" + field->value(m_settings));
	}

	return url;
}
