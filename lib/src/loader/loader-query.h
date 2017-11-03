#ifndef LOADER_QUERY_H
#define LOADER_QUERY_H

#include <QMap>
#include <QString>
#include <QVariant>
#include "loader-data.h"


class Loader;
class Site;

class LoaderQuery
{
	public:
		explicit LoaderQuery(Site *site, QMap<QString, QVariant> options);
		LoaderData next();
		bool hasNext() const;

	private:
		Site *m_site;
		QMap<QString, QVariant> m_options;
		bool m_finished;
		int m_offset;
};

#endif // LOADER_QUERY_H
