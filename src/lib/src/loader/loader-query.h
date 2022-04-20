#ifndef LOADER_QUERY_H
#define LOADER_QUERY_H

#include <QMap>
#include <QString>
#include <QVariant>


struct LoaderData;
class Profile;
class Site;

class LoaderQuery
{
	public:
		explicit LoaderQuery(Profile *profile, Site *site, QMap<QString, QVariant> options);
		bool start();
		LoaderData next();
		bool hasNext() const;

	private:
		Profile *m_profile;
		Site *m_site;
		QMap<QString, QVariant> m_options;
		bool m_finished;
		int m_offset;
};

#endif // LOADER_QUERY_H
