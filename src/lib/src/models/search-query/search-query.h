#ifndef SEARCH_QUERY_H
#define SEARCH_QUERY_H

#include <QMap>
#include <QMetaType>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <utility>


class Image;
class Profile;
class QJsonObject;

class SearchQuery
{
	public:
		// Constructors
		SearchQuery() = default;
		SearchQuery(QStringList tags_)
			: tags(std::move(tags_))
		{}
		SearchQuery(QSharedPointer<Image> gallery_)
			: gallery(std::move(gallery_))
		{}
		SearchQuery(QString endpoint_, QMap<QString, QVariant> input_)
			: endpoint(std::move(endpoint_)), input(std::move(input_))
		{}
		QString toString() const;

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, Profile *profile);

		// Public members
		QMap<QString, QString> urls;
		QStringList tags;
		QSharedPointer<Image> gallery;
		QString endpoint;
		QMap<QString, QVariant> input;
};

bool operator==(const SearchQuery &lhs, const SearchQuery &rhs);
bool operator!=(const SearchQuery &lhs, const SearchQuery &rhs);

Q_DECLARE_METATYPE(SearchQuery)

#endif // SEARCH_QUERY_H
