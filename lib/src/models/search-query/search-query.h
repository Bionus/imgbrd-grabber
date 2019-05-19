#ifndef SEARCH_QUERY_H
#define SEARCH_QUERY_H

#include <QJsonObject>
#include <QMap>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QStringList>


class Image;
class Profile;

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
		QString toString() const;

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, Profile *profile);

		// Public members
		QString url;
		QStringList tags;
		QSharedPointer<Image> gallery;
};

bool operator==(const SearchQuery &lhs, const SearchQuery &rhs);
bool operator!=(const SearchQuery &lhs, const SearchQuery &rhs);

Q_DECLARE_METATYPE(SearchQuery)

#endif // SEARCH_QUERY_H
