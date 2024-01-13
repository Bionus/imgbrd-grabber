#ifndef SITE_FACTORY_H
#define SITE_FACTORY_H

#include <QString>


class Profile;
class Site;
class Source;

class SiteFactory
{
	public:
		static Site *fromUrl(const QString &url, Source *source, Profile *profile);

		static QString getDomain(QString url, bool *ssl = nullptr);

};

#endif // SITE_FACTORY_H
