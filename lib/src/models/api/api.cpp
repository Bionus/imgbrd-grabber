#include "models/api/api.h"
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/site.h"
#include "models/source.h"


Api::Api(QString name)
	: m_name(std::move(name))
{}


QString Api::getName() const { return m_name; }


QSharedPointer<Image> Api::parseImage(Page *parentPage, QMap<QString, QString> d, int position, const QList<Tag> &tags) const
{
	Site *site = parentPage->site();

	d["position"] = QString::number(position + 1);

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
