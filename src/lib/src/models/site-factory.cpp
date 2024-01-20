#include "site-factory.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"


Site *SiteFactory::fromUrl(const QString &url, Source *source, Profile *profile)
{
	// Parse the given URL
	bool ssl = false;
	const QString domain = getDomain(url, &ssl);

	// Create the site instance
	Site *site = new Site(domain, source, profile);
	if (ssl) {
		site->setSetting("ssl", true, false);
	}

	return site;
}


QString SiteFactory::getDomain(QString url, bool *ssl)
{
	// Remove scheme and set the SSL variable
	if (url.startsWith("http://")) {
		url = url.mid(7);
		if (ssl != nullptr) {
			*ssl = false;
		}
	} else if (url.startsWith("https://")) {
		url = url.mid(8);
		if (ssl != nullptr) {
			*ssl = true;
		}
	}

	// Remove trailing slash (if any)
	if (url.endsWith('/')) {
		url = url.left(url.length() - 1);
	}

	return url;
}
