#include "models/api/api.h"
#include <QRegularExpression>
#include "logger.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


Api::Api(const QString &name, const QMap<QString, QString> &data)
	: QObject(), m_name(name), m_data(data)
{
	QString prefix = "Urls/" + m_name;
	for (auto it = m_data.begin(); it != m_data.end(); ++it)
	{
		if (it.key().startsWith(prefix))
		{
			const QString k = it.key().right(it.key().length() - prefix.length() - 1);
			m_data["Urls/" + k] = it.value();
		}
	}
}


QString Api::getName() const	{ return m_name;	}
bool Api::needAuth() const	{ return contains("Urls/NeedAuth") && value("Urls/NeedAuth").toLower() == "true";	}

bool Api::contains(const QString &key) const	{ return m_data.contains(key);	}
QString Api::value(const QString &key) const	{ return m_data.value(key);		}


PageUrl Api::pageUrl(const QString &tt, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const
{
	PageUrl ret;
	QString url;
	QString search = tt;

	// Default tag is none is given
	if (m_data.contains("DefaultTag") && search.isEmpty())
	{ search = m_data.value("DefaultTag"); }

	// Find page number
	const int forced = forcedLimit();
	const int pidLimit = forced > 0 ? forced : limit;
	const int pid = pidLimit * (page - 1);
	page = page - 1 + m_data.value("FirstPage").toInt();

	// Custom URL for pool search
	if (m_data.contains("Urls/Pools"))
	{
		QRegularExpression poolRx("pool:(\\d+)");
		auto match = poolRx.match(search);
		if (match.hasMatch())
		{
			url = m_data.value("Urls/Pools");
			url.replace("{pool}", match.captured(1));
		}
	}

	// Home URL for empty searches
	if (url.isEmpty())
	{
		if (search.isEmpty() && m_data.contains("Urls/Home"))
		{ url = m_data.value("Urls/Home"); }
		else
		{ url = m_data.value("Urls/Tags"); }
	}

	// Return an error if we are trying to do a search on a non-compatible API
	if (!search.isEmpty() && !url.contains("{tags}"))
	{
		ret.error = tr("Tag search is impossible with the chosen source (%1).").arg(m_name);
		return ret;
	}

	int maxPage = -1;
	if (site->isLoggedIn() && m_data.contains("Urls/MaxPageLoggedIn"))
	{ maxPage = m_data.value("Urls/MaxPageLoggedIn").toInt(); }
	else if (m_data.contains("Urls/MaxPage"))
	{ maxPage = m_data.value("Urls/MaxPage").toInt(); }

	const bool isAltPage = maxPage >= 0 && page > maxPage && page - 1 <= lastPage && lastPage <= page + 1;
	if (m_data.contains("Urls/NormalPage"))
	{ url.replace("{cpage}", isAltPage ? "{altpage}" : m_data.value("Urls/NormalPage")); }
	if (isAltPage)
	{
		url.replace("{altpage}", m_data.value("Urls/AltPage" + QString(lastPage > page ? "Prev" : "Next")));
		url.replace("{pagepart}", "");
	}
	else
	{
		if (m_data.contains("Urls/PagePart"))
		{ url.replace("{pagepart}", m_data.value("Urls/PagePart")); }
		url.replace("{altpage}", "");
	}

	// Basic replaces
	url.replace("{tags}", QUrl::toPercentEncoding(search));
	url.replace("{limit}", QString::number(limit));

	// Previous page replaces
	url.replace("{min}", QString::number(lastPageMinId));
	url.replace("{max}", QString::number(lastPageMaxId));
	url.replace("{min-1}", QString::number(lastPageMinId - 1));
	url.replace("{max-1}", QString::number(lastPageMaxId - 1));
	url.replace("{min+1}", QString::number(lastPageMinId + 1));
	url.replace("{max+1}", QString::number(lastPageMaxId + 1));

	// Page replaces
	url.replace("{pid}", QString::number(pid));
	url.replace("{page}", QString::number(page));

	// Add login information
	url = site->fixLoginUrl(url, value("Urls/Login"));

	ret.url = url;
	return ret;
}

PageUrl Api::tagsUrl(int page, int limit, Site *site) const
{
	PageUrl ret;
	QString url = value("Urls/TagApi");

	// Basic information
	page = page - 1 + value("FirstPage").toInt();
	url.replace("{page}", QString::number(page));
	url.replace("{limit}", QString::number(limit));

	// Add login information
	url = site->fixLoginUrl(url, value("Urls/Login"));

	ret.url = url;
	return ret;
}

PageUrl Api::detailsUrl(qulonglong id, const QString &md5, Site *site) const
{
	PageUrl ret;

	QString url = value("Urls/Html/Post");
	url.replace("{id}", QString::number(id));
	url.replace("{md5}", md5);
	url = site->fixLoginUrl(url, value("Urls/Login"));

	ret.url = url;
	return ret;
}

ParsedDetails Api::parseDetails(const QString &source, Site *site) const
{
	Q_UNUSED(source);
	Q_UNUSED(site);

	ParsedDetails ret;
	ret.error = "Not implemented";
	return ret;
}

PageUrl Api::checkUrl() const
{
	PageUrl ret;
	ret.url = value("Check/Url");
	return ret;
}

ParsedCheck Api::parseCheck(const QString &source) const
{
	ParsedCheck ret;

	QRegularExpression rx(value("Check/Regex"));
	ret.ok = rx.match(source).hasMatch();

	return ret;
}


QString Api::parseSetImageUrl(Site *site, const QString &settingUrl, const QString &settingReplaces, QString ret, QMap<QString, QString> *d, bool replaces, const QString &def) const
{
	if (contains(settingUrl) && ret.length() < 5)
	{
		QStringList options = value(settingUrl).split('|');
		for (QString opt : options)
		{
			if (opt.contains("{tim}") && d->value("tim").isEmpty())
				return QString();

			opt.replace("{id}", d->value("id"))
			.replace("{md5}", d->value("md5"))
			.replace("{ext}", d->value("ext", "jpg"))
			.replace("{tim}", d->value("tim"))
			.replace("{website}", site->url());

			if (!opt.endsWith("/." + d->value("ext")) && !opt.contains('{'))
			{
				ret = opt;
				break;
			}
		}
	}
	else if (contains(settingReplaces) && replaces)
	{
		if (ret.isEmpty() && !def.isEmpty())
			ret = def;

		QStringList reps = value(settingReplaces).split('&');
		for (const QString &rep : reps)
		{
			const QRegularExpression rgx(rep.left(rep.indexOf("->")));
			ret.replace(rgx, rep.right(rep.size() - rep.indexOf("->") - 2));
		}
	}
	QString fixed = site->fixUrl(ret).toString();

	// Clean fake webp files
	if (fixed.endsWith(QLatin1String(".jpg.webp")))
		fixed = fixed.left(fixed.length() - 5);

	return fixed;
}

QSharedPointer<Image> Api::parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags, bool replaces) const
{
	Site *site = parentPage->site();

	// Remove dot before extension
	if (d.contains("ext") && d["ext"][0] == '.')
	{ d["ext"] = d["ext"].mid(1); }

	// Set default values
	if (!d.contains("file_url"))
	{ d["file_url"] = ""; }
	if (!d.contains("sample_url"))
	{ d["sample_url"] = ""; }
	if (!d.contains("preview_url"))
	{ d["preview_url"] = ""; }

	if (replaces)
	{
		// Fix urls
		d["file_url"] = parseSetImageUrl(site, "Urls/Image", "Urls/ImageReplaces", d["file_url"], &d, true, d["preview_url"]);
		d["sample_url"] = parseSetImageUrl(site, "Urls/Sample", "Urls/SampleReplaces", d["sample_url"], &d, true, d["preview_url"]);
		d["preview_url"] = parseSetImageUrl(site, "Urls/Preview", "Urls/PreviewReplaces", d["preview_url"], &d);
	}

	if (d["file_url"].isEmpty())
	{ d["file_url"] = d["preview_url"]; }
	if (d["sample_url"].isEmpty())
	{ d["sample_url"] = d["preview_url"]; }

	QStringList errors;

	// If the file path is wrong (ends with "/.jpg")
	if (errors.isEmpty() && d["file_url"].endsWith("/." + d["ext"]))
	{ errors.append(QStringLiteral("file url")); }

	if (!errors.isEmpty())
	{
		log(QStringLiteral("[%1][%2] Image #%3 ignored. Reason: %4.").arg(site->url(), m_name, QString::number(position + 1), errors.join(", ")), Logger::Info);
		return QSharedPointer<Image>();
	}

	// Generate image
	QSharedPointer<Image> img(new Image(site, d, site->getSource()->getProfile(), parentPage));
	img->moveToThread(this->thread());
	if (!tags.isEmpty())
		img->setTags(tags);

	return img;
}

bool Api::canLoadTags() const
{ return contains("Urls/TagApi"); }
bool Api::canLoadDetails() const
{ return contains("Urls/Post"); }
bool Api::canLoadCheck() const
{ return contains("Check/Url") && contains("Check/Regex"); }
int Api::forcedLimit() const
{ return contains("Urls/Limit") ? value("Urls/Limit").toInt() : 0; }
int Api::maxLimit() const
{
	const int forced = forcedLimit();
	if (forced > 0)
		return forced;

	return contains("Urls/MaxLimit") ? value("Urls/MaxLimit").toInt() : 0;
}

QStringList Api::modifiers() const
{ return value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts); }
QStringList Api::forcedTokens() const
{
	QStringList ret;
	if (contains("Regex/NeedLoad"))
		ret.append("*");
	if (contains("Regex/ForceImageUrl"))
		ret.append("file_url");
	if ((m_name == "Html" || m_name == "Rss") && contains("Regex/ImageDate"))
		ret.append("date");
	return ret;
}
