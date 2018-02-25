#include "models/api/rss-api.h"
#include <QDomDocument>
#include <QRegularExpression>


RssApi::RssApi(const QMap<QString, QString> &data)
	: Api("Rss", data)
{}

ParsedPage RssApi::parsePage(Page *parentPage, const QString &source, int first, int limit) const
{
	Q_UNUSED(limit);

	ParsedPage ret;

	// Parsing
	QDomDocument doc;
	QString errorMsg;
	int errorLine, errorColumn;
	if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
	{
		ret.error = QString("Error parsing RSS file: %1 (%2 - %3).").arg(errorMsg).arg(errorLine).arg(errorColumn);
		return ret;
	}
	QDomElement docElem = doc.documentElement();

	// Reading posts
	QDomNodeList nodeList = docElem.elementsByTagName("item");
	for (int id = 0; id < nodeList.count(); id++)
	{
		QDomNodeList children = nodeList.at(id + first).childNodes();
		QMap<QString, QString> d, dat;
		for (int i = 0; i < children.size(); i++)
		{
			QString content = children.at(i).childNodes().at(0).nodeValue();
			if (!content.isEmpty())
			{ dat.insert(children.at(i).nodeName(), content.trimmed()); }
			else
			{ dat.insert(children.at(i).nodeName(), children.at(i).attributes().namedItem("url").nodeValue().trimmed()); }
		}

		d.insert("page_url", dat["link"]);
		d.insert("tags", dat["media:keywords"]);
		d.insert("preview_url", dat["media:thumbnail"]);
		d.insert("file_url", dat["media:content"]);

		// Shimmie
		if (dat.contains("dc:creator"))
		{ d.insert("author", dat["dc:creator"]); }
		if (dat.contains("enclosure"))
		{ d.insert("file_url", dat["enclosure"]); }
		if (dat.contains("pubDate"))
		{ d.insert("created_at", QString::number(QDateTime::fromString(dat["pubDate"], "ddd, dd MMM yyyy hh:mm:ss +0000").toTime_t())); }

		if (!d.contains("id"))
		{
			QRegularExpression rx("/(\\d+)");
			auto match = rx.match(d["page_url"]);
			if (match.hasMatch())
			{ d.insert("id", match.captured(1)); }
		}

		QSharedPointer<Image> img = parseImage(parentPage, d, id + first);
		if (!img.isNull())
		{ ret.images.append(img); }
	}

	return ret;
}

ParsedTags RssApi::parseTags(const QString &source, Site *site) const
{
	Q_UNUSED(source);
	Q_UNUSED(site);

	ParsedTags ret;
	ret.error = "Not implemented";
	return ret;
}
