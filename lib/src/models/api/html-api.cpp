#include "models/api/html-api.h"
#include <QRegularExpression>
#include "vendor/json.h"


HtmlApi::HtmlApi(const QMap<QString, QString> &data)
	: Api("Html", data)
{}

ParsedPage HtmlApi::parsePage(Page *parentPage, const QString &source, int first) const
{
	ParsedPage ret;

	// Getting tags
	if (contains("Regex/Tags"))
	{
		QRegularExpression rxtags(value("Regex/Tags"), QRegularExpression::DotMatchesEverythingOption);
		QStringList tags = QStringList();
		auto matches = rxtags.globalMatch(source);
		while (matches.hasNext())
		{
			auto match = matches.next();
			if (!tags.contains(match.captured(2)))
			{
				ret.tags.append(Tag(match.captured(2), match.captured(1), match.captured(3).toInt()));
				tags.append(match.captured(2));
			}
		}
	}

	// Getting images
	QRegularExpression rx(value("Regex/Image"), QRegularExpression::DotMatchesEverythingOption);
	auto matches = rx.globalMatch(source);
	int id = 0;
	while (matches.hasNext())
	{
		auto match = matches.next();
		QMap<QString, QString> d;
		for (QString group : rx.namedCaptureGroups())
		{
			if (group.isEmpty())
				continue;

			QString val = match.captured(group);
			if (!val.isEmpty())
			{
				int underscorePos = group.lastIndexOf('_');
				bool ok;
				group.midRef(underscorePos + 1).toInt(&ok);
				if (underscorePos != -1 && ok)
				{ group = group.left(underscorePos); }
				d[group] = val;
			}
		}

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
		QRegularExpression rx(value("Regex/NextPage"));
		auto match = rx.match(source);
		if (match.hasMatch())
		{ ret.urlNextPage = QUrl(match.captured(1)); }
	}
	if (contains("Regex/PrevPage"))
	{
		QRegularExpression rx(value("Regex/PrevPage"));
		auto match = rx.match(source);
		if (match.hasMatch())
		{ ret.urlPrevPage = QUrl(match.captured(1)); }
	}

	// Count images
	if (contains("Regex/Count"))
	{
		QRegularExpression rxlast(value("Regex/Count"));
		auto match = rxlast.match(source);
		int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{ ret.imageCount = cnt; }
	}

	return ret;
}
