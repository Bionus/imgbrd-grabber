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


QString _parseSetImageUrl(Site *site, const Api *api, const QString &settingUrl, const QString &settingReplaces, QString ret, QMap<QString, QString> *d, bool replaces = true, const QString &def = QString())
{
	if (api->contains(settingUrl) && ret.length() < 5)
	{
		QStringList options = api->value(settingUrl).split('|');
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
	else if (api->contains(settingReplaces) && replaces)
	{
		if (ret.isEmpty() && !def.isEmpty())
			ret = def;

		QStringList reps = api->value(settingReplaces).split('&');
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
	d["file_url"] = _parseSetImageUrl(site, this, "Urls/Image", "Urls/ImageReplaces", d["file_url"], &d, true, d["preview_url"]);
	d["sample_url"] = _parseSetImageUrl(site, this, "Urls/Sample", "Urls/SampleReplaces", d["sample_url"], &d, true, d["preview_url"]);
	d["preview_url"] = _parseSetImageUrl(site, this, "Urls/Preview", "Urls/PreviewReplaces", d["preview_url"], &d);

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
