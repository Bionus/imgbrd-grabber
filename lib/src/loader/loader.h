#ifndef LOADER_H
#define LOADER_H

#include <QMap>
#include <QString>
#include <QVariant>


class Site;
class LoaderQuery;

class Loader
{
	public:
		explicit Loader(Site *site);
		LoaderQuery search(QMap<QString, QVariant> options) const;

	private:
		Site *m_site;
};

#endif // LOADER_H
