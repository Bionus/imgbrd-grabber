#include "models/search-query/search-query.h"
#include <QJsonArray>
#include <QJsonObject>
#include "functions.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"


QString SearchQuery::toString() const
{
	if (!gallery.isNull()) {
		return "Gallery: " + gallery->name();
	}

	return tags.join(' ');
}


void SearchQuery::write(QJsonObject &json) const
{
	if (!endpoint.isEmpty()) {
		json["endpoint"] = endpoint;
	}

	if (!tags.isEmpty()) {
		json["tags"] = QJsonArray::fromStringList(tags);
	}

	if (!gallery.isNull()) {
		QJsonObject jsonGallery;
		gallery->write(jsonGallery);
		json["gallery"] = jsonGallery;
	}

	if (!input.isEmpty()) {
		json["input"] = QJsonObject::fromVariantMap(input);
	}
}

bool SearchQuery::read(const QJsonObject &json, Profile *profile)
{
	// Endpoint
	if (json.contains("endpoint")) {
		endpoint = json["endpoint"].toString();
	}

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
		const QMap<QString, Site*> &sites = profile->getSites();
		auto image = new Image(profile);
		if (image->read(json["gallery"].toObject(), sites)) {
			gallery = QSharedPointer<Image>(image);
		} else {
			image->deleteLater();
		}
	}

	// Input
	if (json.contains("input")) {
		input = json["input"].toObject().toVariantMap();
	}

	return true;
}


bool operator==(const SearchQuery &lhs, const SearchQuery &rhs)
{
	return lhs.tags == rhs.tags
		&& lhs.gallery == rhs.gallery
		&& lhs.endpoint == rhs.endpoint
		&& lhs.input == rhs.input;
}

bool operator!=(const SearchQuery &lhs, const SearchQuery &rhs)
{
	return !(lhs == rhs);
}
