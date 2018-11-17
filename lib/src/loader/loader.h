#ifndef LOADER_H
#define LOADER_H

#include <QVariant>


class LoaderQuery;
class Site;

class Loader
{
	public:
		explicit Loader(Site *site);
		LoaderQuery search(const QMap<QString, QVariant> &options) const;

	private:
		Site *m_site;
};

#endif // LOADER_H
