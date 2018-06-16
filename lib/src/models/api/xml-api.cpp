#include "models/api/xml-api.h"
#include <QDomDocument>
#include "models/api/api.h"
#include "models/site.h"
#include "tags/tag-database.h"


XmlApi::XmlApi(const QMap<QString, QString> &data)
	: Api("Xml", data)
{}

ParsedPage XmlApi::parsePage(Page *parentPage, const QString &source, int first, int limit) const
{
	Q_UNUSED(limit);

	ParsedPage ret;

	// Parsing
	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
	{
		ret.error = QStringLiteral("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg).arg(errorLine).arg(errorColumn);
		return ret;
	}
	QDomElement docElem = doc.documentElement();

	// Getting last page
	int count = docElem.attributes().namedItem("count").nodeValue().toInt();
	const QString database = docElem.attributes().namedItem("type").nodeValue();
	if (count == 0 && database == QLatin1String("array"))
	{ count = docElem.elementsByTagName("total-count").at(0).toElement().text().toInt(); }
	if (count > 0)
	{ ret.imageCount = count; }

	// Reading posts
	QDomNodeList nodeList = docElem.elementsByTagName("post");
	for (int id = 0; id < nodeList.count(); id++)
	{
		QDomNode node = nodeList.at(id + first);
		QMap<QString, QString> d;
		QList<Tag> tags;
		if (database == QLatin1String("array"))
		{
			if (node.namedItem("md5").isNull())
				continue;

			QStringList infos, assoc;
			infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score" << "tags_artist" << "tags_character" << "tags_copyright" << "tags_general" << "tags_meta" << "ext";
			assoc << "created-at" << "status" << "source" << "has_comments" << "file-url" << "large-file-url" << "change" << "sample_width" << "has-children" << "preview-file-url" << "image-width" << "md5" << "preview_width" << "sample_height" << "parent-id" << "image-height" << "has_notes" << "uploader-id" << "file_size" << "id" << "preview_height" << "rating" << "tag-string" << "uploader-name" << "score" << "tag-string-artist" << "tag-string-character" << "tag-string-copyright" << "tag-string-general" << "tag-string-meta" << "file-ext";

			if (node.namedItem("preview-file-url").isNull())
			{
				// New syntax with old keys
				for (int i = 0; i < infos.count(); i++)
				{
					QDomNode item = node.namedItem(infos.at(i));
					if (!item.isNull())
					{ d[infos.at(i)] = item.toElement().text(); }
				}
			}
			else
			{
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = node.namedItem(assoc.at(i)).toElement().text(); }
			}

			// Typed tags
			QDomNodeList tagTypes = node.namedItem("tags").childNodes();
			if (!tagTypes.isEmpty())
			{
				for (int typeId = 0; typeId < tagTypes.count(); ++typeId)
				{
					QDomNode tagType = tagTypes.at(typeId);
					const TagType tType(tagType.nodeName());
					QDomNodeList tagList = tagType.childNodes();
					for (int iTag = 0; iTag < tagList.count(); ++iTag)
					{ tags.append(Tag(tagList.at(iTag).toElement().text(), tType)); }
				}
			}
		}
		else
		{
			QStringList infos;
			infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
			for (int i = 0; i < infos.count(); i++)
			{
				d[infos.at(i)] = node.attributes().isEmpty()
								 ? node.namedItem(infos.at(i)).toElement().text()
								 : node.attributes().namedItem(infos.at(i)).nodeValue().trimmed();
			}
		}

		QSharedPointer<Image> img = parseImage(parentPage, d, id + first, tags);
		if (!img.isNull())
		{ ret.images.append(img); }
	}

	return ret;
}

ParsedTags XmlApi::parseTags(const QString &source, Site *site) const
{
	ParsedTags ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	// Tag variables definitions
	int id;
	QString name;
	int count;
	int typeId;
	QString ttype;

	// Read source
	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
	{
		ret.error = QStringLiteral("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg).arg(errorLine).arg(errorColumn);
		return ret;
	}
	QDomElement docElem = doc.documentElement();

	// Read tags
	QDomNodeList nodeList = docElem.elementsByTagName("tag");
	for (int i = 0; i < nodeList.count(); i++)
	{
		QDomNode node = nodeList.at(i);
		QDomNamedNodeMap attr = node.attributes();

		ttype.clear();
		if (!node.namedItem("post-count").isNull())
		{
			id = node.namedItem("id").toElement().text().toInt();
			name = node.namedItem("name").toElement().text();
			count = node.namedItem("post-count").toElement().text().toInt();
			typeId = node.namedItem("category").toElement().text().toInt();
		}
		else if (attr.contains("name"))
		{
			id = attr.namedItem("id").toAttr().value().toInt();
			name = attr.namedItem("name").toAttr().value();
			count = attr.namedItem("count").toAttr().value().toInt();
			typeId = attr.namedItem("type").toAttr().value().toInt();
		}
		else
		{
			id = node.namedItem("id").toElement().text().toInt();
			name = node.namedItem("name").toElement().text();
			count = node.namedItem("count").toElement().text().toInt();
			typeId = node.namedItem("type").toElement().text().toInt();
		}

		const TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
		ret.tags.append(Tag(id, name, tagType, count));
	}

	return ret;
}
