#include "models/api/html-api.h"
#include <QRegularExpression>
#include <QtMath>
#include "functions.h"
#include "models/api/api.h"
#include "models/site.h"
#include "tags/tag-database.h"
#include "vendor/json.h"


HtmlApi::HtmlApi(const QMap<QString, QString> &data)
	: Api("Html", data)
{}

ParsedPage HtmlApi::parsePage(Page *parentPage, const QString &source, int first, int limit) const
{
	ParsedPage ret;

	// Getting tags
	if (contains("Regex/Tags"))
	{
		QList<Tag> tgs = Tag::FromRegexp(value("Regex/Tags"), source);
		if (!tgs.isEmpty())
		{ ret.tags = tgs; }
	}

	// Getting images
	QRegularExpression rxImages(value("Regex/Image"), QRegularExpression::DotMatchesEverythingOption);
	auto matches = rxImages.globalMatch(source);
	int id = 0;
	while (matches.hasNext())
	{
		const auto &match = matches.next();
		QMap<QString, QString> d = multiMatchToMap(match, rxImages.namedCaptureGroups());

		// JSON elements
		if (d.contains("json") && !d["json"].isEmpty())
		{
			QVariant src = Json::parse(d["json"]);
			if (!src.isNull())
			{
				QMap<QString, QVariant> map = src.toMap();
				for (auto it = map.begin(); it != map.end(); ++it)
				{ d[it.key()] = it.value().toString(); }
			}
		}

		QSharedPointer<Image> img = parseImage(parentPage, d, id + first);
		if (!img.isNull())
		{ ret.images.append(img); }

		id++;
	}

	// Navigation
	if (contains("Regex/NextPage"))
	{
		QRegularExpression rxNextPage(value("Regex/NextPage"));
		auto match = rxNextPage.match(source);
		if (match.hasMatch())
		{ ret.urlNextPage = QUrl(match.captured(1)); }
	}
	if (contains("Regex/PrevPage"))
	{
		QRegularExpression rxPrevPage(value("Regex/PrevPage"));
		auto match = rxPrevPage.match(source);
		if (match.hasMatch())
		{ ret.urlPrevPage = QUrl(match.captured(1)); }
	}

	// Last page
	if (contains("LastPage"))
	{ ret.pageCount = value("LastPage").toInt(); }
	else if (contains("Regex/LastPage"))
	{
		QRegularExpression rxlast(value("Regex/LastPage"));
		auto match = rxlast.match(source);
		const int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{
			int pagesCount = cnt;
			if (value("Urls/Tags").contains("{pid}") || (contains("Urls/PagePart") && value("Urls/PagePart").contains("{pid}")))
			{
				const int forced = forcedLimit();
				const int ppid = forced > 0 ? forced : limit;
				pagesCount = qFloor(static_cast<qreal>(pagesCount) / static_cast<qreal>(ppid)) + 1;
			}
			ret.pageCount = pagesCount;
		}
	}

	// Count images
	if (contains("Regex/Count"))
	{
		QRegularExpression rxlast(value("Regex/Count"));
		auto match = rxlast.match(source);
		const int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{ ret.imageCount = cnt; }
	}

	// Wiki
	if (contains("Regex/Wiki"))
	{
		QRegularExpression rxwiki(value("Regex/Wiki"), QRegularExpression::DotMatchesEverythingOption);
		auto match = rxwiki.match(source);
		if (match.hasMatch())
		{
			QString wiki = match.captured(1);
			wiki.remove("/wiki/show?title=");
			wiki.remove(QRegularExpression("<p><a href=\"([^\"]+)\">Full entry &raquo;</a></p>"));
			wiki.replace("<h6>", "<span class=\"title\">").replace("</h6>", "</span>");
			ret.wiki = wiki;
		}
	}

	return ret;
}

ParsedTags HtmlApi::parseTags(const QString &source, Site *site) const
{
	ParsedTags ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	// Read tags
	QRegularExpression rx(value("Regex/TagApi"), QRegularExpression::DotMatchesEverythingOption);
	auto matches = rx.globalMatch(source);
	while (matches.hasNext())
	{
		auto match = matches.next();

		// Parse result using the regex
		QMap<QString, QString> d;
		for (const QString &group : rx.namedCaptureGroups())
		{
			if (group.isEmpty())
				continue;

			QString val = match.captured(group);
			if (!val.isEmpty())
			{ d[group] = val.trimmed(); }
		}

		// Map variables
		int id = d.contains("id") ? d["id"].toInt() : 0;
		QString name = d["tag"];
		int count = d.contains("count") ? d["count"].toInt() : 0;
		int typeId = d.contains("typeId") ? d["typeId"].toInt() : -1;
		QString ttype = d["type"];

		TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
		ret.tags.append(Tag(id, name, tagType, count));
	}

	return ret;
}

ParsedDetails HtmlApi::parseDetails(const QString &source, Site *site) const
{
	Q_UNUSED(site);
	ParsedDetails ret;

	// Pools
	if (contains("Regex/Pools"))
	{
		QRegularExpression rx(value("Regex/Pools"));
		auto matches = rx.globalMatch(source);
		while (matches.hasNext())
		{
			auto match = matches.next();
			QString previous = match.captured(1), id = match.captured(2), name = match.captured(3), next = match.captured(4);
			ret.pools.append(Pool(id.toInt(), name, 0, next.toInt(), previous.toInt()));
		}
	}

	// Tags
	QString rxtags;
	if (contains("Regex/ImageTags"))
	{ rxtags = value("Regex/ImageTags"); }
	else if (contains("Regex/Tags"))
	{ rxtags = value("Regex/Tags"); }
	if (!rxtags.isEmpty())
	{ ret.tags = Tag::FromRegexp(rxtags, source); }

	// Image url
	if (contains("Regex/ImageUrl"))
	{
		QRegularExpression rx(value("Regex/ImageUrl"));
		auto matches = rx.globalMatch(source);
		while (matches.hasNext())
		{
			auto match = matches.next();
			ret.imageUrl = match.captured(1).replace("&amp;", "&");
		}
	}

	// Image date
	if (contains("Regex/ImageDate"))
	{
		QRegularExpression rx(value("Regex/ImageDate"));
		auto matches = rx.globalMatch(source);
		while (matches.hasNext())
		{
			auto match = matches.next();
			ret.createdAt = qDateTimeFromString(match.captured(1));
		}
	}

	return ret;
}
