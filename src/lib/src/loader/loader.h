#ifndef LOADER_H
#define LOADER_H

#include <QMap>
#include <QString>


class LoaderQuery;
class Profile;
class QVariant;
class Site;

class Loader
{
	public:
		explicit Loader(Profile *profile, Site *site);
		LoaderQuery search(const QMap<QString, QVariant> &options) const;

	private:
		Profile *m_profile;
		Site *m_site;
};

#endif // LOADER_H
