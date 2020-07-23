#include "login/url-login.h"
#include "auth/auth-field.h"
#include "auth/url-auth.h"
#include "mixed-settings.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


UrlLogin::UrlLogin(UrlAuth *auth, Site *site, NetworkManager *manager, MixedSettings *settings)
	: m_auth(auth), m_site(site), m_manager(manager), m_settings(settings), m_page(nullptr)
{}

bool UrlLogin::isTestable() const
{
	return m_auth->maxPage() > 0;
}

void UrlLogin::login()
{
	if (m_page != nullptr) {
		if (!m_page->isLoaded()) {
			return;
		}

		m_page->abort();
		m_page->deleteLater();
	}

	const int maxPageAnonymous = m_auth->maxPage();
	m_page = new Page(m_site->getSource()->getProfile(), m_site, { m_site }, QStringList(), maxPageAnonymous);
	connect(m_page, &Page::finishedLoading, this, &UrlLogin::loginFinished);
	connect(m_page, &Page::failedLoading, this, &UrlLogin::loginFinished);
	m_page->load();
}

void UrlLogin::loginFinished()
{
	if (!m_page->images().isEmpty()) {
		emit loggedIn(Result::Success);
		return;
	}

	emit loggedIn(Result::Failure);
}

QString UrlLogin::complementUrl(QString url) const
{
	bool hasQuery = url.contains('?');

	int i = 0;
	for (AuthField *field : m_auth->fields()) {
		if (!field->key().isEmpty()) {
			const QString &val = field->value(m_settings);
			if (!val.isEmpty()) {
				url.append((i == 0 && !hasQuery ? '?' : '&') + field->key() + "=" + val);
				++i;
			}
		}
	}

	return url;
}
