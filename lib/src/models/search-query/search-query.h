#ifndef SEARCH_QUERY_H
#define SEARCH_QUERY_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QStringList>


class Image;
class Site;

class SearchQuery
{
	public:
		// Constructors
		SearchQuery() = default;
		SearchQuery(QStringList tags_)
			: tags(tags_)
		{}
		SearchQuery(QSharedPointer<Image> gallery_)
			: gallery(gallery_)
		{}

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites);

		// Public members
		QStringList tags;
		QSharedPointer<Image> gallery;
};

bool operator==(const SearchQuery &lhs, const SearchQuery &rhs);
bool operator!=(const SearchQuery &lhs, const SearchQuery &rhs);

Q_DECLARE_METATYPE(SearchQuery)

#endif // SEARCH_QUERY_H
