#include "models/api/json-api.h"
#include "models/page.h"
#include "models/site.h"
#include "tags/tag-database.h"
#include "vendor/json.h"


JsonApi::JsonApi(const QMap<QString, QString> &data)
	: Api("Json", data)
{}

ParsedPage JsonApi::parsePage(Page *parentPage, const QString &source, int first, int limit) const
{
	Q_UNUSED(limit);

	ParsedPage ret;

	// Parsing
	QVariant src = Json::parse(source);
	if (src.isNull())
	{
		ret.error = QString("Error parsing JSON file: \"%1\"").arg(source.left(500));
		return ret;
	}

	// Check JSON error message
	QMap<QString, QVariant> data = src.toMap();
	if (data.contains("success") && !data["success"].toBool())
	{
		ret.error = QString("JSON error reply: \"%1\"").arg(data["reason"].toString());
		return ret;
	}

	if (data.contains("total"))
	{ ret.imageCount = data.value("total").toInt(); }

	// Get the list of posts
	QList<QVariant> sourc = src.toList();
	QStringList postsKey = QStringList() << "images" << "search" << "posts";
	for (int i = 0; i < postsKey.count() && sourc.isEmpty(); ++i)
	{ sourc = data.value(postsKey[i]).toList(); }

	QMap<QString, QVariant> sc;
	for (int id = 0; id < sourc.count(); id++)
	{
		QList<Tag> tags;

		sc = sourc.at(id + first).toMap();
		QMap<QString, QString> d;
		if (sc.contains("tag_string"))
		{
			QStringList infos, assoc;
			infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score" << "tags_artist" << "tags_character" << "tags_copyright" << "tags_general";
			assoc << "created_at" << "status" << "source" << "has_comments" << "file_url" << "large_file_url" << "change" << "sample_width" << "has_children" << "preview_file_url" << "image_width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "image_height" << "has_notes" << "uploader_id" << "file_size" << "id" << "preview_height" << "rating" << "tag_string" << "uploader_name" << "score" << "tag_string_artist" << "tag_string_character" << "tag_string_copyright" << "tag_string_general";
			for (int i = 0; i < infos.count(); i++)
			{ d[infos.at(i)] = sc.value(assoc.at(i)).toString().trimmed(); }
		}
		else if (sc.contains("tag_ids"))
		{
			QStringList from, to;
			from << "created_at" << "source_url" << "image" << "image" << "width" << "sha512_hash" << "height" << "id" << "tags" << "uploader" << "score";
			to << "created_at" << "source" << "file_url" << "preview_url" << "width" << "md5" << "height" << "id" << "tags" << "author" << "score";
			for (int i = 0; i < from.count(); i++)
			{ d[to[i]] = sc.value(from[i]).toString().trimmed(); }
		}
		// 4chan format
		else if (sc.contains("resto"))
		{
			QStringList from, to;
			from << "now" << "w" << "md5" << "h" << "no" << "com" << "time" << "tim" << "name" << "fsize";
			to << "created_at" << "width" << "md5" << "height" << "id" << "comment" << "created_at" << "tim" << "author" << "file_size";
			for (int i = 0; i < from.count(); i++)
			{ d[to[i]] = sc.value(from[i]).toString().trimmed(); }
		}
		// Anime-pictures format
		else if (sc.contains("download_count"))
		{
			QStringList from, to;
			from << "pubtime" << "small_preview" << "width" << "md5" << "height" << "id" << "score_number" << "big_preview" << "ext" << "size";
			to << "created_at" << "preview_url" << "width" << "md5" << "height" << "id" << "score" << "sample_url" << "ext" << "filesize";
			for (int i = 0; i < from.count(); i++)
			{ d[to[i]] = sc.value(from[i]).toString().trimmed(); }
		}
		// Twitter format
		else if (sc.contains("retweet_count"))
		{
			if (!sc.contains("extended_entities"))
				continue;

			auto entities = sc.value("extended_entities").toMap();
			if (!entities.contains("media"))
				continue;

			auto medias = entities.value("media").toList();
			if (medias.isEmpty())
				continue;

			auto media = medias.first().toMap();
			auto sizes = media.value("sizes").toMap();

			d["id"] = sc.value("id_str").toString();
			d["created_at"] = sc.value("created_at").toString();
			d["preview_url"] = media.value("media_url_https").toString() + ":thumb";
			if (sizes.contains("medium"))
			{ d["sample_url"] = media.value("media_url_https").toString() + ":medium"; }

			auto size = sizes.value("large").toMap();
			d["width"] = QString::number(size.value("w").toInt());
			d["height"] = QString::number(size.value("h").toInt());

			if (media.contains("video_info"))
			{
				int maxBitrate = -1;
				auto videoInfo = media.value("video_info").toMap();
				auto variants = videoInfo.value("variants").toList();
				for (QVariant variant : variants)
				{
					auto variantInfo = variant.toMap();
					int bitrate = variantInfo.value("bitrate").toInt();
					if (bitrate > maxBitrate)
					{
						maxBitrate = bitrate;
						d["file_url"] = variantInfo.value("url").toString();
					}
				}
			}
			else
			{ d["file_url"] = media.value("media_url_https").toString() + ":large"; }
		}
		else
		{
			QStringList infos;
			infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
			for (int i = 0; i < infos.count(); i++)
			{ d[infos.at(i)] = sc.value(infos.at(i)).toString().trimmed(); }
		}

		// Tags as objects (Sankaku)
		QMap<int, TagType> tagTypes = parentPage->site()->tagDatabase()->tagTypes();
		if (sc.contains("tags") && sc["tags"].type() == QVariant::List && !tagTypes.isEmpty())
		{
			QList<QVariant> tgs = sc["tags"].toList();
			if (!tgs.isEmpty())
			{
				QMap<int, QString> tagTypesIds;
				for (auto it = tagTypes.begin(); it != tagTypes.end(); ++it)
					tagTypesIds.insert(it.key(), it.value().name());

				for (const QVariant &tagData : tgs)
				{
					QMap<QString, QVariant> tag = tagData.toMap();
					if (tag.contains("name"))
						tags.append(Tag(tag["name"].toString(), Tag::GetType(tag["type"].toString(), tagTypesIds), tag["count"].toInt()));
				}
			}
		}

		// Typed tags (e621)
		if (sc.contains("tags") && sc["tags"].type() == QVariant::Map)
		{
			QMap<QString, QVariant> scTypes = sc["tags"].toMap();
			for (auto it = scTypes.begin(); it != scTypes.end(); ++it)
			{
				TagType tType(it.key());
				QList<QVariant> tagList = it.value().toList();
				for (const QVariant &iTag : tagList)
				{ tags.append(Tag(iTag.toString(), tType)); }
			}
		}

		// Booru-on-rails sizes
		if (sc.contains("representations"))
		{
			QMap<QString, QVariant> sizes = sc.value("representations").toMap();
			if (sizes.contains("thumb"))
			{ d["preview_url"] = sizes["thumb"].toString(); }
			if (sizes.contains("large"))
			{ d["sample_url"] = sizes["large"].toString(); }
			if (sizes.contains("full"))
			{ d["file_url"] = sizes["full"].toString(); }
		}

		// Object creation date
		if (sc.contains("created_at"))
		{
			QMap<QString, QVariant> time = sc.value("created_at").toMap();
			if (!time.isEmpty() && time.contains("s"))
			{ d["created_at"] = time.value("s").toString(); }
		}

		QSharedPointer<Image> img = parseImage(parentPage, d, id + first, tags);
		if (!img.isNull())
		{ ret.images.append(img); }
	}

	return ret;
}

ParsedTags JsonApi::parseTags(const QString &source, Site *site) const
{
	ParsedTags ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	// Read source
	QVariant src = Json::parse(source);
	if (src.isNull())
	{
		ret.error = QString("Error parsing JSON file: \"%1\"").arg(source.left(500));
		return ret;
	}
	QMap<QString, QVariant> data = src.toMap();

	// Check for a JSON error message
	if (data.contains("success") && !data["success"].toBool())
	{
		ret.error = QString("JSON error reply: \"%1\"").arg(data["reason"].toString());
		return ret;
	}

	// Tag variables definitions
	int id;
	QString name;
	int count;
	int typeId;
	QString ttype;

	// Read tags
	QList<QVariant> sourc = src.toList();
	for (const QVariant &el : sourc)
	{
		QMap<QString, QVariant> sc = el.toMap();

		ttype = "";
		if (sc.contains("short_description"))
		{
			id = sc.value("id").toInt();
			name = sc.value("name").toString();
			count = sc.value("images").toInt();
			ttype = sc.value("category").toString();
		}
		else if (sc.contains("post_count"))
		{
			id = sc.value("id").toInt();
			name = sc.value("name").toString();
			count = sc.value("post_count").toInt();
			typeId = sc.value("category").toInt();
		}
		else
		{
			id = sc.value("id").toInt();
			name = sc.value("name").toString();
			count = sc.value("count").toInt();
			typeId = sc.value("type").toInt();
		}

		TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
		ret.tags.append(Tag(id, name, tagType, count));
	}

	return ret;
}
