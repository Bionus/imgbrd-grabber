#include "models/api/api.h"
#include <QRegularExpression>
#include "logger.h"
#include "models/page.h"
#include "models/post-filter.h"
#include "models/site.h"


Api::Api(const QString &name, const QMap<QString, QString> &data)
	: QObject(), m_name(name), m_data(data)
{
	QString prefix = "Urls/" + m_name;
	for (auto it = m_data.begin(); it != m_data.end(); ++it)
	{
		if (it.key().startsWith(prefix))
		{
			QString k = it.key().right(it.key().length() - prefix.length() - 1);
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
	int forced = forcedLimit();
	int pidLimit = forced > 0 ? forced : limit;
	int pid = pidLimit * (page - 1);
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

	bool isAltPage = maxPage >= 0 && page > maxPage && page - 1 <= lastPage && lastPage <= page + 1;
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


QString Api::parseSetImageUrl(Site *site, const QString &settingUrl, const QString &settingReplaces, QString ret, QMap<QString, QString> *d, bool replaces, const QString &def) const
{
	if (contains(settingUrl) && ret.length() < 5)
	{
		QStringList options = value(settingUrl).split('|');
		for (QString opt : options)
		{
			if (opt.contains("{tim}") && d->value("tim").isEmpty())
				return "";

			opt.replace("{id}", d->value("id"))
			.replace("{md5}", d->value("md5"))
			.replace("{ext}", d->value("ext", "jpg"))
			.replace("{tim}", d->value("tim"))
			.replace("{website}", site->url())
			.replace("{cdn}", QString(site->url()).replace("boards.4chan", "4cdn"));

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
			QRegularExpression rgx(rep.left(rep.indexOf("->")));
			ret.replace(rgx, rep.right(rep.size() - rep.indexOf("->") - 2));
		}
	}
	QString fixed = site->fixUrl(ret).toString();

	// Clean fake webp files
	if (fixed.endsWith(".jpg.webp"))
		fixed = fixed.left(fixed.length() - 5);

	return fixed;
}

QSharedPointer<Image> Api::parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags) const
{
	Site *site = parentPage->site();

	// Set default values
	if (!d.contains("file_url"))
	{ d["file_url"] = ""; }
	if (!d.contains("sample_url"))
	{ d["sample_url"] = ""; }

	// Remove dot before extension
	if (d.contains("ext") && d["ext"][0] == '.')
	{ d["ext"] = d["ext"].mid(1); }

	// Fix urls
	d["file_url"] = parseSetImageUrl(site, "Urls/Image", "Urls/ImageReplaces", d["file_url"], &d, true, d["preview_url"]);
	d["sample_url"] = parseSetImageUrl(site, "Urls/Sample", "Urls/SampleReplaces", d["sample_url"], &d, true, d["preview_url"]);
	d["preview_url"] = parseSetImageUrl(site, "Urls/Preview", "Urls/PreviewReplaces", d["preview_url"], &d);

	if (d["file_url"].isEmpty())
	{ d["file_url"] = d["preview_url"]; }
	if (d["sample_url"].isEmpty())
	{ d["sample_url"] = d["preview_url"]; }

	QStringList errors;

	// If the file path is wrong (ends with "/.jpg")
	if (errors.isEmpty() && d["file_url"].endsWith("/." + d["ext"]))
	{ errors.append("file url"); }

	if (!errors.isEmpty())
	{
		log(QString("[%1][%2] Image #%3 ignored. Reason: %4.").arg(site->url(), m_name, QString::number(position + 1), errors.join(", ")), Logger::Info);
		return QSharedPointer<Image>();
	}

	// Generate image
	QSharedPointer<Image> img(new Image(site, d, site->getSource()->getProfile(), parentPage));
	if (!tags.isEmpty())
		img->setTags(tags);

	return img;
}

bool Api::canLoadTags() const
{ return contains("Urls/TagApi"); }
int Api::forcedLimit() const
{ return contains("Urls/Limit") ? value("Urls/Limit").toInt() : 0; }
int Api::maxLimit() const
{
	int forced = forcedLimit();
	if (forced > 0)
		return forced;

	return contains("Urls/MaxLimit") ? value("Urls/MaxLimit").toInt() : 0;
}
QStringList Api::modifiers() const
{ return value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts); }
