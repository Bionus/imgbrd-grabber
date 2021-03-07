#include "image-factory.h"
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QPair>
#include <QRegularExpression>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include "functions.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-type.h"


QSharedPointer<Image> ImageFactory::build(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent)
{
	return ImageFactory::build(site, details, QVariantMap(), profile, parent);
}

QSharedPointer<Image> ImageFactory::build(Site *site, QMap<QString, QString> details, QVariantMap data, Profile *profile, Page *parent)
{
	static QList<QPair<QString, vTransformToken>> transforms
	{
		{ "parentid", ImageFactory::parseInt("parent_id") },
		{ "authorid", ImageFactory::parseInt("creator_id") },
		{ "author", ImageFactory::parseString("author") },
		{ "status", ImageFactory::parseString("status") },
		{ "score", ImageFactory::parseString("score") },
		{ "rating", &ImageFactory::parseRating },
		{ "has_children", ImageFactory::parseBool("has_children") },
		{ "has_note", ImageFactory::parseBool("has_note") },
		{ "has_comments", ImageFactory::parseBool("has_comments") },
		{ "tags_general", ImageFactory::parseTypedTags("general") },
		{ "tags_artist", ImageFactory::parseTypedTags("artist") },
		{ "tags_character", ImageFactory::parseTypedTags("character") },
		{ "tags_copyright", ImageFactory::parseTypedTags("copyright") },
		{ "tags_model", ImageFactory::parseTypedTags("model") },
		{ "tags_species", ImageFactory::parseTypedTags("species") },
		{ "tags_meta", ImageFactory::parseTypedTags("meta") },
		{ "tags", &ImageFactory::parseTags },
		{ "created_at", &ImageFactory::parseCreatedAt },
		{ "date", &ImageFactory::parseDate },
	};

	for (const auto &transform : transforms) {
		const auto &key = transform.first;
		if (details.contains(key) && !data.contains(key) && !details[key].isEmpty()) {
			const auto &f = transform.second;
			f(details[key], data);
			details.remove(key);
		}
	}

	return QSharedPointer<Image>(new Image(site, details, data, profile, parent));
}


vTransformToken ImageFactory::parseString(const QString &key)
{
	return [key](const QString &val, QVariantMap &data) {
		data[key] = val;
	};
}

vTransformToken ImageFactory::parseInt(const QString &key)
{
	return [key](const QString &val, QVariantMap &data) {
		data[key] = val.toInt();
	};
}

vTransformToken ImageFactory::parseBool(const QString &key)
{
	return [key](const QString &val, QVariantMap &data) {
		data[key] = val == "true";
	};
}


void ImageFactory::parseCreatedAt(const QString &val, QVariantMap &data)
{
	data["date"] = qDateTimeFromString(val);
	data["date_raw"] = val;
}

void ImageFactory::parseDate(const QString &val, QVariantMap &data)
{
	data["date"] = QDateTime::fromString(val, Qt::ISODate);
	data["date_raw"] = val;
}

void ImageFactory::parseRating(const QString &val, QVariantMap &data)
{
	static const QMap<QString, QString> assoc
	{
		{ "s", "safe" },
		{ "q", "questionable" },
		{ "e", "explicit" }
	};

	data["rating"] = assoc.contains(val)
		? assoc[val]
		: val.toLower();
}

vTransformToken ImageFactory::parseTypedTags(const QString &type)
{
	return [type](const QString &val, QVariantMap &data) {
		if (!data.contains("tags")) {
			data.insert("tags", QVariant::fromValue(QList<Tag>()));
		}
		QList<Tag> tagList = data["tags"].value<QList<Tag>>();

		const TagType tagType(type);
		QStringList tags = val.split(' ', Qt::SkipEmptyParts);
		for (QString tag : tags) {
			tag.replace("&amp;", "&");
			tagList.append(Tag(tag, tagType));
		}

		data.insert("tags", QVariant::fromValue(tagList));
	};
}

void ImageFactory::parseTags(const QString &val, QVariantMap &data)
{
	if (!data.contains("tags")) {
		data.insert("tags", QVariant::fromValue(QList<Tag>()));
	}
	QList<Tag> tagList = data["tags"].value<QList<Tag>>();

	if (!tagList.isEmpty()) {
		return;
	}

	QString raw = val;
	raw.replace(QRegularExpression("[\r\n\t]+"), " ");

	// Automatically find tag separator and split the list
	const int commas = raw.count(", ");
	const int spaces = raw.count(" ");
	const QStringList &tags = commas >= 10 || (commas > 0 && (spaces - commas) / commas < 2)
		? raw.split(", ", Qt::SkipEmptyParts)
		: raw.split(" ", Qt::SkipEmptyParts);

	for (QString tg : tags) {
		tg.replace("&amp;", "&");

		const int colon = tg.indexOf(':');
		if (colon != -1) {
			const QString tp = tg.left(colon).toLower();
			if (tp == "user") {
				data["author"] = tg.mid(colon + 1);
			} else if (tp == "score") {
				data["score"] = tg.mid(colon + 1).toInt();
			} else if (tp == "size") {
				/*QStringList size = tg.mid(colon + 1).split('x');
				if (size.size() == 2) {
					setSize(QSize(size[0].toInt(), size[1].toInt()), Size::Full);
				}*/
			} else if (tp == "rating") {
				parseRating(tg.mid(colon + 1), data);
			} else {
				tagList.append(Tag(tg));
			}
		} else {
			tagList.append(Tag(tg));
		}
	}

	data.insert("tags", QVariant::fromValue(tagList));
}
