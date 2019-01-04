#include "models/search-query/search-query.h"
#include <QJsonArray>
#include "functions.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"


void SearchQuery::write(QJsonObject &json) const
{
	json["tags"] = QJsonArray::fromStringList(tags);

	if (!gallery.isNull()) {
		QJsonObject jsonGallery;
		gallery->write(jsonGallery);
		json["gallery"] = jsonGallery;
	}
}

bool SearchQuery::read(const QJsonObject &json, const QMap<QString, Site*> &sites)
{
	// Tags
	if (json.contains("tags")) {
		QJsonArray jsonTags = json["tags"].toArray();
		tags.reserve(jsonTags.count());
		for (auto tag : jsonTags) {
			tags.append(tag.toString());
		}
	}

	// Gallery
	if (json.contains("gallery")) {
		auto image = new Image();
		if (image->read(json["gallery"].toObject(), sites)) {
			gallery = QSharedPointer<Image>(image);
		} else {
			image->deleteLater();
		}
	}

	return true;
}


bool operator==(const SearchQuery &lhs, const SearchQuery &rhs)
{
	return lhs.tags == rhs.tags
		&& lhs.gallery == rhs.gallery;
}

bool operator!=(const SearchQuery &lhs, const SearchQuery &rhs)
{
	return !(lhs == rhs);
}
