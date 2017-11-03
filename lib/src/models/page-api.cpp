#include "page-api.h"
#include <QDomDocument>
#include <QRegularExpression>
#include <cmath>
#include "page.h"
#include "site.h"
#include "api.h"
#include "image.h"
#include "vendor/json.h"
#include "logger.h"
#include "functions.h"


PageApi::PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, QStringList tags, int page, int limit, QStringList postFiltering, bool smart, QObject *parent, int pool, int lastPage, int lastPageMinId, int lastPageMaxId)
	: QObject(parent), m_parentPage(parentPage), m_profile(profile), m_site(site), m_api(api), m_search(tags), m_postFiltering(postFiltering), m_errors(QStringList()), m_imagesPerPage(limit), m_lastPage(lastPage), m_lastPageMinId(lastPageMinId), m_lastPageMaxId(lastPageMaxId), m_smart(smart), m_reply(nullptr), m_replyTags(nullptr)
{
	m_imagesCount = -1;
	m_pagesCount = -1;
	m_imagesCountSafe = false;
	m_pagesCountSafe = false;

	m_search = tags;
	m_page = page;
	m_pool = pool;
	m_format = m_api->getName();

	updateUrls();
}

QUrl PageApi::parseUrl(QString url, int pid, int p, QString t, QString pseudo, QString password)
{
	if (pid < 0)
		pid = (this->page() - 1) * this->imagesPerPage();
	if (p < 0)
		p = this->page();
	if (t.isEmpty())
		t = m_search.join(" ");
	if (pseudo.isEmpty())
		pseudo = m_site->username();
	if (password.isEmpty())
		password = m_site->password();

	url.replace("{tags}", QUrl::toPercentEncoding(t));
	url.replace("{limit}", QString::number(m_imagesPerPage));

	int maxPage = -1;
	if (m_site->isLoggedIn() && m_api->contains("Urls/MaxPageLoggedIn"))
	{ maxPage = m_api->value("Urls/MaxPageLoggedIn").toInt(); }
	else if (m_api->contains("Urls/MaxPage"))
	{ maxPage = m_api->value("Urls/MaxPage").toInt(); }

	m_isAltPage = maxPage >= 0 && p > maxPage && m_page - 1 <= m_lastPage && m_lastPage <= m_page + 1;
	if (m_api->contains("Urls/NormalPage"))
	{ url.replace("{cpage}", m_isAltPage ? "{altpage}" : m_api->value("Urls/NormalPage")); }
	if (m_isAltPage)
	{
		url.replace("{altpage}", m_api->value("Urls/AltPage" + QString(m_lastPage > m_page ? "Prev" : "Next")));
		url.replace("{pagepart}", "");
	}
	else
	{
		if (m_api->contains("Urls/PagePart"))
		{ url.replace("{pagepart}", m_api->value("Urls/PagePart")); }
		url.replace("{altpage}", "");
	}
	url.replace("{min}", QString::number(m_lastPageMinId));
	url.replace("{max}", QString::number(m_lastPageMaxId));
	url.replace("{min-1}", QString::number(m_lastPageMinId-1));
	url.replace("{max-1}", QString::number(m_lastPageMaxId-1));
	url.replace("{min+1}", QString::number(m_lastPageMinId+1));
	url.replace("{max+1}", QString::number(m_lastPageMaxId+1));
	url.replace("{pid}", QString::number(pid));
	url.replace("{page}", QString::number(p));

	bool hasLoginString = m_api->contains("Urls/Login") && (!pseudo.isEmpty() || !password.isEmpty());
	url.replace("{login}", hasLoginString ? m_api->value("Urls/Login") : "");
	url.replace("{pseudo}", pseudo);
	url.replace("{password}", password);
	if (url.contains("{appkey}"))
	{
		QString appkey = m_site->value("AppkeySalt");
		appkey.replace("%password%", password);
		appkey.replace("%username%", pseudo.toLower());
		url.replace("{appkey}", QCryptographicHash::hash(appkey.toUtf8(), QCryptographicHash::Sha1).toHex());
	}

	return m_site->fixUrl(url);
}

void PageApi::setLastPage(Page *page)
{
	m_lastPage = page->page();
	m_lastPageMaxId = page->maxId();
	m_lastPageMinId = page->minId();

	if (!page->nextPage().isEmpty())
	{ m_url = page->nextPage(); }
	else
	{ /*fallback(false);*/ }

	updateUrls();
}

void PageApi::updateUrls()
{
	// Default tag is none is given
	QString t = m_search.join(" ").trimmed();
	if (m_api->contains("DefaultTag") && t.isEmpty())
		t = m_api->value("DefaultTag");

	// Find page number
	bool limited = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit");
	int p = m_page;
	m_blim = limited ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;
	if (m_smart)
	{
		if (m_imagesPerPage > m_blim || limited)
		{ m_imagesPerPage = m_blim; }
		p = (int)floor(((m_page - 1.) * m_imagesPerPage) / m_blim) + 1;
	}
	p = p - 1 + m_api->value("FirstPage").toInt();

	// GET login information
	QString pseudo = m_site->username();
	QString password = m_site->password();

	int pid = m_api->contains("Urls/Limit") ? m_api->value("Urls/Limit").toInt() * (m_page - 1) : m_imagesPerPage * (m_page - 1);

	// URL searches
	if (m_search.count() == 1 && !t.isEmpty() && isUrl(t))
	{
		m_originalUrl = QString(t);
		m_url = parseUrl(t, pid, p, t, pseudo, password).toString();
		m_urlRegex = parseUrl(t, pid, p, t, pseudo, password).toString();
		return;
	}

	// Check if we are looking for a pool
	QRegularExpression poolRx("pool:(\\d+)");
	auto match = poolRx.match(t);
	QString url;
	int pl = -1;
	if (match.hasMatch())
	{
		for (Api *api : m_site->getApis())
		{
			if (api->contains("Urls/Pools"))
			{
				url = api->value("Urls/Pools");
				url.replace("{pool}", match.captured(1));
				pl = match.captured(1).toInt();
				m_api = api;
				t = t.remove(match.capturedStart(0), match.captured(0).length()).trimmed();
				break;
			}
		}
		if (url.isEmpty())
		{
			log(QString("[%1][%2] No source of this site is compatible with pools.").arg(m_site->url()).arg(m_format), Logger::Warning);
			m_errors.append(tr("No source of this site is compatible with pools."));
			m_search.removeAll("pool:"+match.captured(1));
			t.remove(m_pool);
			t = t.trimmed();
		}
	}
	if (url.isEmpty())
	{
		if (t.isEmpty() && m_api->contains("Urls/Home"))
		{ url = m_api->value("Urls/Home"); }
		else
		{ url = m_api->value("Urls/Tags"); }
	}

	// Global replace tokens
	m_originalUrl = QString(url);
	m_url = parseUrl(url, pid, p, t, pseudo, password).toString();

	auto plMatch = poolRx.match(t);
	if ((pl > 0 || plMatch.hasMatch()) && m_api->contains("Urls/Html/Pools"))
	{
		url = m_site->value("Urls/Html/Pools");
		url = parseUrl(url, pid, p, t, pseudo, password).toString();
		url.replace("{pool}", pl > 0 ? QString::number(pl) : plMatch.captured(1));
		m_urlRegex = QUrl(url);
	}
	else if (m_api->contains("Urls/Html/Tags"))
	{
		url = m_site->value("Urls/Html/"+QString(t.isEmpty() && m_site->contains("Urls/Html/Home") ? "Home" : "Tags"));
		m_urlRegex = parseUrl(url, pid, p, t, pseudo, password);
	}
	else
	{ m_urlRegex = ""; }
}

void PageApi::load(bool rateLimit)
{
	// Reading reply and resetting vars
	m_images.clear();
	m_tags.clear();
	m_pageImageCount = 0;
	/*m_imagesCount = -1;
	m_pagesCount = -1;*/

	m_site->getAsync(rateLimit ? Site::QueryType::Retry : Site::QueryType::List, m_url, [this](QNetworkReply *reply) {
		log(QString("[%1][%2] Loading page <a href=\"%3\">%3</a>").arg(m_site->url()).arg(m_format).arg(m_url.toString().toHtmlEscaped()), Logger::Info);
		m_reply = reply;
		connect(m_reply, SIGNAL(finished()), this, SLOT(parse()));
	});
}
void PageApi::abort()
{
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

void PageApi::loadTags()
{
	if (!m_urlRegex.isEmpty())
	{
		log(QString("[%1][%2] Loading tags from page <a href=\"%3\">%3</a>").arg(m_site->url()).arg(m_format).arg(m_urlRegex.toString().toHtmlEscaped()), Logger::Info);
		m_replyTags = m_site->get(m_urlRegex);
		connect(m_replyTags, &QNetworkReply::finished, this, &PageApi::parseTags);
	}
}
void PageApi::abortTags()
{
	if (m_replyTags != nullptr && m_replyTags->isRunning())
		m_replyTags->abort();
}

QString _parseSetImageUrl(Site *site, Api* api, QString settingUrl, QString settingReplaces, QString ret, QMap<QString,QString> *d, bool replaces = true, QString def = QString())
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


void PageApi::parseImage(QMap<QString,QString> d, int position, QList<Tag> tags)
{
	// Set default values
	if (!d.contains("file_url"))
	{ d["file_url"] = ""; }
	if (!d.contains("sample_url"))
	{ d["sample_url"] = ""; }

	// Remove dot before extension
	if (d.contains("ext") && d["ext"][0] == '.')
	{ d["ext"] = d["ext"].mid(1); }

	// Fix urls
	d["file_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Image", "Urls/ImageReplaces", d["file_url"], &d, true, d["preview_url"]);
	d["sample_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Sample", "Urls/SampleReplaces", d["sample_url"], &d, true, d["preview_url"]);
	d["preview_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Preview", "Urls/PreviewReplaces", d["preview_url"], &d);

	if (d["file_url"].isEmpty())
	{ d["file_url"] = d["preview_url"]; }
	if (d["sample_url"].isEmpty())
	{ d["sample_url"] = d["preview_url"]; }

	QStringList errors;

	// If the file path is wrong (ends with "/.jpg")
	if (errors.isEmpty() && d["file_url"].endsWith("/." + d["ext"]))
	{ errors.append("file url"); }

	if (errors.isEmpty())
	{ m_pageImageCount++; }

	// Generate image
	QSharedPointer<Image> img(new Image(m_site, d, m_profile, m_parentPage));
	errors.append(img->filter(m_postFiltering));

	// Add if everything is ok
	if (errors.isEmpty())
	{
		// If we could get detailed tags information
		if (!tags.isEmpty())
			img->setTags(tags);

		m_images.append(QSharedPointer<Image>(img));
	}
	else
	{
		img->deleteLater();
		log(QString("[%1][%2] Image #%3 ignored. Reason: %4.").arg(m_site->url()).arg(m_format).arg(QString::number(position + 1), errors.join(", ")), Logger::Info);
	}
}

void PageApi::parse()
{
	log(QString("[%1][%2] Receiving page <a href=\"%3\">%3</a>").arg(m_site->url()).arg(m_format).arg(m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redir.toString(), m_url);
		log(QString("[%1][%2] Redirecting page <a href=\"%3\">%3</a> to <a href=\"%4\">%4</a>").arg(m_site->url()).arg(m_format).arg(m_url.toString().toHtmlEscaped()).arg(newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_url = newUrl;
		load();
		return;
	}

	int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429)
	{
		log(QString("[%1][%2] Limit reached (429). New try.").arg(m_site->url()).arg(m_format), Logger::Warning);
		load(true);
		return;
	}

	m_source = m_reply->readAll();

	if (m_source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QString("[%1][%2] Loading error: %3 (%4)").arg(m_site->url()).arg(m_format).arg(m_reply->errorString()).arg(m_reply->error())); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	int first = m_smart ? ((m_page - 1) * m_imagesPerPage) % m_blim : 0;

	// XML
	if (m_format == "Xml")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(QString("[%1][%2] Error parsing XML file: %3 (%4 - %5).").arg(m_site->url()).arg(m_format).arg(errorMsg).arg(errorLine).arg(errorColumn), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}
		QDomElement docElem = doc.documentElement();

		// Getting last page
		int count = docElem.attributes().namedItem("count").nodeValue().toFloat();
		QString database = docElem.attributes().namedItem("type").nodeValue();
		if (count == 0 && database == "array")
		{ count = docElem.elementsByTagName("total-count").at(0).toElement().text().toInt(); }
		if (count > 0)
		{ setImageCount(count, true); }

		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int id = 0; id < nodeList.count(); id++)
			{
				QDomNode node = nodeList.at(id + first);
				QMap<QString,QString> d;
				QList<Tag> tags;
				if (database == "array")
				{
					if (node.namedItem("md5").isNull())
						continue;

					QStringList infos, assoc;
					infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score" << "tags_artist" << "tags_character" << "tags_copyright" << "tags_general" << "ext";
					assoc << "created-at" << "status" << "source" << "has_comments" << "file-url" << "large-file-url" << "change" << "sample_width" << "has-children" << "preview-file-url" << "image-width" << "md5" << "preview_width" << "sample_height" << "parent-id" << "image-height" << "has_notes" << "uploader-id" << "file_size" << "id" << "preview_height" << "rating" << "tag-string" << "uploader-name" << "score" << "tag-string-artist" << "tag-string-character" << "tag-string-copyright" << "tag-string-general" << "file-ext";

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
							TagType tType(tagType.nodeName());
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
				this->parseImage(d, id + first, tags);
			}
		}
	}

	// RSS
	else if (m_format == "Rss")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(QString("[%1][%2] Error parsing RSS file: %3 (%4 - %5).").arg(m_site->url()).arg(m_format).arg(errorMsg).arg(errorLine).arg(errorColumn), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}
		QDomElement docElem = doc.documentElement();

		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("item");
		if (nodeList.count() > 0)
		{
			for (int id = 0; id < nodeList.count(); id++)
			{
				QDomNodeList children = nodeList.at(id + first).childNodes();
				QMap<QString,QString> d, dat;
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

				this->parseImage(d, id + first);
			}
		}
	}

	// Regexes
	else if (m_format == "Html")
	{
		// Getting tags
		if (m_site->contains("Regex/Tags"))
		{
			QRegularExpression rxtags(m_site->value("Regex/Tags"), QRegularExpression::DotMatchesEverythingOption);
			QStringList tags = QStringList();
			auto matches = rxtags.globalMatch(m_source);
			while (matches.hasNext())
			{
				auto match = matches.next();
				if (!tags.contains(match.captured(2)))
				{
					m_tags.append(Tag(match.captured(2), match.captured(1), match.captured(3).toInt()));
					tags.append(match.captured(2));
				}
			}
		}

		// Getting images
		QRegularExpression rx(m_site->value("Regex/Image"), QRegularExpression::DotMatchesEverythingOption);
		auto matches = rx.globalMatch(m_source);
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
					group.mid(underscorePos + 1).toInt(&ok);
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
					for (int i = 0; i < map.size(); i++)
					{ d[map.keys().at(i)] = map.values().at(i).toString(); }
				}
			}
			this->parseImage(d, id + first);
			id++;
		}
	}

	// JSON
	else if (m_format == "Json")
	{
		QVariant src = Json::parse(m_source);
		if (!src.isNull())
		{
			// Check JSON error message
			QMap<QString, QVariant> data = src.toMap();
			if (data.contains("success") && !data["success"].toBool())
			{
				log(QString("[%1][%2] JSON error reply: \"%3\"").arg(m_site->url()).arg(m_format).arg(data["reason"].toString()), Logger::Warning);
				emit finishedLoading(this, LoadResult::Error);
				return;
			}

			if (data.contains("total"))
			{ setImageCount(data.value("total").toInt(), true); }

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
				QMap<QString,QString> d;
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
				else
				{
					QStringList infos;
					infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
					for (int i = 0; i < infos.count(); i++)
					{ d[infos.at(i)] = sc.value(infos.at(i)).toString().trimmed(); }
				}

				// Tags as objects (Sankaku)
				if (sc.contains("tags") && m_api->contains("Regex/TagTypes"))
				{
					QList<QVariant> tgs = sc["tags"].toList();
					if (!tgs.isEmpty())
					{
						QStringList tagTypes = m_api->value("Regex/TagTypes").split(',');
						for (const QVariant &tagData : tgs)
						{
							QMap<QString, QVariant> tag = tagData.toMap();
							if (tag.contains("name"))
								tags.append(Tag(tag["name"].toString(), Tag::GetType(tag["type"].toString(), tagTypes), tag["count"].toInt()));
						}
					}
				}

				// Typed tags (e621)
				if (sc.contains("tags"))
				{
					QMap<QString, QVariant> tagTypes = sc["tags"].toMap();
					if (!tagTypes.isEmpty())
					{
						for (const QString &tagType : tagTypes.keys())
						{
							TagType tType(tagType);
							QList<QVariant> tagList = tagTypes.value(tagType).toList();
							for (const QVariant &iTag : tagList)
							{ tags.append(Tag(iTag.toString(), tType)); }

						}
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

				this->parseImage(d, id + first, tags);
			}
		}
		else
		{
			log(QString("[%1][%2] Error parsing JSON file: \"%3\"").arg(m_site->url()).arg(m_format).arg(m_source.left(500)), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}
	}

	// If tags have not been retrieved yet
	if (m_tags.isEmpty())
	{
		QStringList tagsGot;
		for (int i = 0; i < m_images.count(); i++)
		{
			QList<Tag> tags = m_images.at(i)->tags();
			for (int t = 0; t < tags.count(); t++)
			{
				if (tagsGot.contains(tags[t].text()))
				{ m_tags[tagsGot.indexOf(tags[t].text())].setCount(m_tags[tagsGot.indexOf(tags[t].text())].count()+1); }
				else
				{
					m_tags.append(tags[t]);
					tagsGot.append(tags[t].text());
				}
			}
		}
	}

	// Try to get navigation info on HTML pages
	if (m_format == "Html")
	{
		parseNavigation(m_source);
	}

	// Remove first n images (according to site settings)
	int skip = m_site->setting("ignore/always", 0).toInt();
	if (m_isAltPage)
		skip = m_site->setting("ignore/alt", 0).toInt();
	if (m_page == m_site->value("FirstPage").toInt())
		skip = m_site->setting("ignore/1", 0).toInt();
	if (m_images.size() > m_imagesPerPage && m_images.size() > skip)
		for (int i = 0; i < skip; ++i)
			m_images.removeFirst();

	// Virtual paging
	int firstImage = 0;
	int lastImage = m_smart ? m_imagesPerPage : m_images.size();
	if (!m_originalUrl.contains("{page}") && !m_originalUrl.contains("{cpage}") && !m_originalUrl.contains("{pagepart}") && !m_originalUrl.contains("{pid}"))
	{
		firstImage = m_imagesPerPage * (m_page - 1);
		lastImage = m_imagesPerPage;
	}
	while (firstImage > 0 && !m_images.isEmpty())
	{
		m_images.removeFirst();
		firstImage--;
	}
	while (m_images.size() > lastImage)
	{ m_images.removeLast(); }

	log(QString("[%1][%2] Parsed page <a href=\"%3\">%3</a>: %4 images, %5 total (%6), %7 pages (%8)").arg(m_site->url()).arg(m_format).arg(m_reply->url().toString().toHtmlEscaped()).arg(m_images.count()).arg(imagesCount(false)).arg(imagesCount(true)).arg(pagesCount(false)).arg(pagesCount(true)), Logger::Info);

	m_reply->deleteLater();
	m_reply = nullptr;

	QString t = m_search.join(" ");
	if (m_site->contains("DefaultTag") && t.isEmpty())
	{ t = m_site->value("DefaultTag"); }
	if (!m_search.isEmpty() && !m_api->value("Urls/" + QString(t.isEmpty() && !m_api->contains("Urls/Home") ? "Home" : "Tags")).contains("{tags}"))
	{ m_errors.append(tr("Tag search is impossible with the chosen source (%1).").arg(m_format)); }

	emit finishedLoading(this, LoadResult::Ok);
}
void PageApi::parseTags()
{
	log(QString("[%1][%2] Receiving tags page <a href=\"%3\">%3</a>").arg(m_site->url()).arg(m_format).arg(m_replyTags->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redir = m_replyTags->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redir.toString(), m_urlRegex);
		log(QString("[%1][%2] Redirecting tags page <a href=\"%3\">%3</a> to <a href=\"%4\">%4</a>").arg(m_site->url()).arg(m_format).arg(m_urlRegex.toString().toHtmlEscaped()).arg(newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_urlRegex = newUrl;
		loadTags();
		return;
	}

	QString source = QString::fromUtf8(m_replyTags->readAll());

	if (m_site->contains("Regex/Tags"))
	{
		QList<Tag> tgs = Tag::FromRegexp(m_site->value("Regex/Tags"), source);
		if (!tgs.isEmpty())
		{ m_tags = tgs; }
	}

	parseNavigation(source);

	// Wiki
	m_wiki.clear();
	if (m_site->contains("Regex/Wiki"))
	{
		QRegularExpression rxwiki(m_site->value("Regex/Wiki"));
		auto match = rxwiki.match(source);
		if (match.hasMatch())
		{
			m_wiki = match.captured(1);
			m_wiki.remove("/wiki/show?title=").remove(QRegularExpression("<p><a href=\"([^\"]+)\">Full entry &raquo;</a></p>")).replace("<h6>", "<span class=\"title\">").replace("</h6>", "</span>");
		}
	}

	m_replyTags->deleteLater();
	m_replyTags = nullptr;

	emit finishedLoadingTags(this);
}

void PageApi::parseNavigation(const QString &source)
{
	// Navigation
	if (m_site->contains("Regex/NextPage") && m_urlNextPage.isEmpty())
	{
		QRegularExpression rx(m_site->value("Regex/NextPage"));
		auto match = rx.match(source);
		if (match.hasMatch())
		{ m_urlNextPage = QUrl(match.captured(1)); }
	}
	if (m_site->contains("Regex/PrevPage") && m_urlPrevPage.isEmpty())
	{
		QRegularExpression rx(m_site->value("Regex/PrevPage"));
		auto match = rx.match(source);
		if (match.hasMatch())
		{ m_urlPrevPage = QUrl(match.captured(1)); }
	}

	// Last page
	if (m_site->contains("LastPage") && m_pagesCount < 1)
	{ setPageCount(m_site->value("LastPage").toInt(), true); }
	if (m_site->contains("Regex/LastPage") && m_pagesCount < 1)
	{
		QRegularExpression rxlast(m_site->value("Regex/LastPage"));
		auto match = rxlast.match(source);
		int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{
			int pagesCount = cnt;
			if (m_originalUrl.contains("{pid}") || (m_api->contains("Urls/PagePart") && m_api->value("Urls/PagePart").contains("{pid}")))
			{
				int ppid = m_api->contains("Urls/Limit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;
				pagesCount = floor((float)pagesCount / (float)ppid) + 1;
			}
			setPageCount(pagesCount, true);
		}
	}

	// Count images
	if (m_site->contains("Regex/Count") && m_imagesCount < 1)
	{
		QRegularExpression rxlast(m_site->value("Regex/Count"));
		auto match = rxlast.match(source);
		int cnt = match.hasMatch() ? match.captured(1).remove(",").toInt() : 0;
		if (cnt > 0)
		{ setImageCount(cnt, true); }
	}
	if (m_imagesCount < 1)
	{
		for (const Tag &tag : m_tags)
		{
			if (tag.text() == m_search.join(" "))
			{ setImageCount(tag.count(), false); }
		}
	}
}

void PageApi::clear()
{
	m_images.clear();
	m_pageImageCount = 0;
}

QList<QSharedPointer<Image>>	PageApi::images()		{ return m_images;		}
QUrl			PageApi::url()			{ return m_url;			}
QString			PageApi::source()		{ return m_source;		}
QString			PageApi::wiki()		{ return m_wiki;		}
QList<Tag>		PageApi::tags()		{ return m_tags;		}
QStringList		PageApi::search()		{ return m_search;		}
QStringList		PageApi::errors()		{ return m_errors;		}
QUrl			PageApi::nextPage()	{ return m_urlNextPage;	}
QUrl			PageApi::prevPage()	{ return m_urlPrevPage;	}

int PageApi::imagesPerPage()
{ return m_imagesPerPage;	}
int PageApi::page()
{ return m_page;			}
int PageApi::pageImageCount()
{ return m_pageImageCount;	}
int PageApi::highLimit()
{
	if (m_api->contains("Urls/Limit"))
		return m_api->value("Urls/Limit").toInt();
	if (m_api->contains("Urls/MaxLimit"))
		return m_api->value("Urls/MaxLimit").toInt();
	return 0;
}

bool PageApi::isImageCountSure() { return m_imagesCountSafe; }
int PageApi::imagesCount(bool guess)
{
	int perPage = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;

	if (m_imagesCount < 0 && guess && m_pagesCount >= 0)
		return m_pagesCount * perPage;

	return m_imagesCount;
}
bool PageApi::isPageCountSure() { return m_pagesCountSafe; }
int PageApi::pagesCount(bool guess)
{
	int perPage = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;

	if (m_pagesCount < 0 && guess && m_imagesCount >= 0)
		return (int)ceil(((float)m_imagesCount) / perPage);

	return m_pagesCount;
}

int PageApi::maxId()
{
	int maxId = 0;
	for (const QSharedPointer<Image> &img : m_images)
		if (img->id() > maxId || maxId == 0)
			maxId = img->id();
	return maxId;
}
int PageApi::minId()
{
	int minId = 0;
	for (const QSharedPointer<Image> &img : m_images)
		if (img->id() < minId || minId == 0)
			minId = img->id();
	return minId;
}

void PageApi::setUrl(const QUrl &url)
{
	m_url = url;
}

void PageApi::setImageCount(int count, bool sure)
{
	if (m_imagesCount <= 0 || (!m_imagesCountSafe && sure))
	{
		m_imagesCount = count;
		m_imagesCountSafe = sure;

		if (sure)
		{ setPageCount(ceil(((float)count) / m_imagesPerPage), true); }
	}
}

void PageApi::setPageCount(int count, bool sure)
{
	if (m_pagesCount <= 0 || (!m_pagesCountSafe && sure))
	{
		m_pagesCount = count;
		m_pagesCountSafe = sure;

		if (sure)
		{ setImageCount(count * m_imagesPerPage, false); }
	}
}
