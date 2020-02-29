#include "image-factory.h"
#include <QDateTime>
#include <QList>
#include <QMap>
#include <QPair>
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


Image *ImageFactory::build(Site *site, QMap<QString, QString> details, QVariantMap data, Profile *profile, Page *parent)
{
	static QList<QPair<QString, vTransformToken>> transforms
	{
		{ "rating", &ImageFactory::parseRating },
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

	return new Image(site, details, data, profile, parent);
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
}

void ImageFactory::parseDate(const QString &val, QVariantMap &data)
{
	data["date"] = QDateTime::fromString(val, Qt::ISODate);
}

void ImageFactory::parseRating(const QString &val, QVariantMap &data)
{
	static QMap<QString, QString> assoc =
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
		QStringList tags = val.split(' ', QString::SkipEmptyParts);
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

	if (!tagList.isEmpty())
		return;

	QString raw = val;
	raw.replace(QRegularExpression("[\r\n\t]+"), " ");

	// Automatically find tag separator and split the list
	const int commas = raw.count(", ");
	const int spaces = raw.count(" ");
	const QStringList &tags = commas >= 10 || (commas > 0 && (spaces - commas) / commas < 2)
		? raw.split(", ", QString::SkipEmptyParts)
		: raw.split(" ", QString::SkipEmptyParts);

	for (QString tg : tags) {
		tg.replace("&amp;", "&");

		const int colon = tg.indexOf(':');
		if (colon != -1) {
			const QString tp = tg.left(colon).toLower();
			if (tp == "user") {
				//m_author = tg.mid(colon + 1);
			} else if (tp == "score") {
				//m_score = tg.mid(colon + 1);
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
