#include "page-api.h"
#include <QDomDocument>
#include <math.h>
#include "page.h"
#include "site.h"
#include "api.h"
#include "image.h"
#include "vendor/json.h"
#include "logger.h"


PageApi::PageApi(Page *parentPage, Profile *profile, Site *site, Api *api, QStringList tags, int page, int limit, QStringList postFiltering, bool smart, QObject *parent, int pool, int lastPage, int lastPageMinId, int lastPageMaxId)
	: QObject(parent), m_parentPage(parentPage), m_profile(profile), m_site(site), m_api(api), m_search(tags), m_postFiltering(postFiltering), m_errors(QStringList()), m_imagesPerPage(limit), m_currentSource(0), m_lastPage(lastPage), m_lastPageMinId(lastPageMinId), m_lastPageMaxId(lastPageMaxId), m_smart(smart), m_reply(nullptr), m_replyTags(nullptr)
{
	m_imagesCount = -1;
	m_pagesCount = -1;
	m_search = tags;
	m_page = page;
	m_pool = pool;

	updateUrls();
}
PageApi::~PageApi()
{ }

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

	m_currentSource--;
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
	QRegExp regexUrl("^https?://[^\\s/$.?#].[^\\s]*$");
	if (m_search.count() == 1 && !t.isEmpty() && regexUrl.exactMatch(t))
	{
		m_originalUrl = QString(t);
		m_url = parseUrl(t, pid, p, t, pseudo, password).toString();
		m_urlRegex = parseUrl(t, pid, p, t, pseudo, password).toString();
		return;
	}

	// Check if we are looking for a pool
	QRegExp poolRx("pool:(\\d+)");
	QString url;
	int pl = -1;
	int pos = -1;
	if ((pos = poolRx.indexIn(t)) != -1)
	{
		for (int i = 1; i <= m_site->getApis().count() + 1; i++)
		{
			Api *api = m_site->getApis().at(i - 1);
			if (api->contains("Urls/Pools"))
			{
				url = api->value("Urls/Pools");
				url.replace("{pool}", poolRx.cap(1));
				pl = poolRx.cap(1).toInt();
				m_currentSource = i;
				m_api = api;
				t = t.remove(pos, poolRx.cap(0).length()).trimmed();
				break;
			}
		}
		if (url.isEmpty())
		{
			log("No source of this site is compatible with pools.");
			m_errors.append(tr("No source of this site is compatible with pools."));
			m_search.removeAll("pool:"+poolRx.cap(1));
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

	if ((pl >= 0 || poolRx.indexIn(t) != -1) && m_api->contains("Urls/Html/Pools"))
	{
		url = m_site->value("Urls/Html/Pools");
		url = parseUrl(url, pid, p, t, pseudo, password).toString();
		url.replace("{pool}", poolRx.cap(1));
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
	/*m_imagesCount = -1;
	m_pagesCount = -1;*/

	m_site->getAsync(rateLimit ? Site::QueryType::Retry : Site::QueryType::List, m_url, [this](QNetworkReply *reply) {
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
			opt.replace("{id}", d->value("id"))
			.replace("{md5}", d->value("md5"))
			.replace("{ext}", d->value("ext", "jpg"));

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

		QStringList replaces = api->value(settingReplaces).split('&');
		for (QString rep : replaces)
		{
			QRegExp rgx(rep.left(rep.indexOf("->")));
			ret.replace(rgx, rep.right(rep.size() - rep.indexOf("->") - 2));
		}
	}
	return site->fixUrl(ret).toString();
}


void PageApi::parseImage(QMap<QString,QString> d, int position, QList<Tag> tags)
{
	// Set default values
	if (!d.contains("file_url"))
	{ d["file_url"] = ""; }
	if (!d.contains("sample_url"))
	{ d["sample_url"] = ""; }

	// Fix urls
	d["file_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Image", "Urls/ImageReplaces", d["file_url"], &d, true, d["preview_url"]);
	d["sample_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Sample", "Urls/SampleReplaces", d["sample_url"], &d, true, d["preview_url"]);
	d["preview_url"] = _parseSetImageUrl(m_site, m_api, "Urls/Preview", "Urls/PreviewReplaces", d["preview_url"], &d);

	if (d["file_url"].isEmpty())
	{ d["file_url"] = d["preview_url"]; }
	if (d["sample_url"].isEmpty())
	{ d["sample_url"] = d["preview_url"]; }

	// Generate image
	QSharedPointer<Image> img = QSharedPointer<Image>(new Image(m_site, d, m_profile, m_parentPage));
	QStringList errors = img->filter(m_postFiltering);

	// If the file path is wrong (ends with "/.jpg")
	if (errors.isEmpty() && d["file_url"].endsWith("/." + d["ext"]))
	{ errors.append("file url"); }

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
		log(QString("Image #%1 ignored. Reason: %2.").arg(QString::number(position + 1), errors.join(", ")));
	}
}

void PageApi::parse()
{
	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = m_site->fixUrl(redir.toString(), m_url);
		load();
		return;
	}

	int statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode == 429)
	{
		log("Limit reached (429). New try.");
		load(true);
		return;
	}

	m_source = m_reply->readAll();

	if (m_source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QString("Loading error: %1").arg(m_reply->errorString())); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	int first = m_smart ? ((m_page - 1) * m_imagesPerPage) % m_blim : 0;
	m_format = m_api->getName();

	// XML
	if (m_format == "Xml")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(QString("Error parsing XML file: %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
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
		{
			m_imagesCount = count;
			m_pagesCount = ceil(((float)count) / m_imagesPerPage);
		}

		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int id = 0; id < nodeList.count(); id++)
			{
				QDomNode node = nodeList.at(id + first);
				QMap<QString,QString> d;
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
				this->parseImage(d, id + first);
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
			log(QString("Error parsing RSS file: %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
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
					QRegExp rx("/(\\d+)");
					rx.indexIn(d["page_url"]);
					d.insert("id", rx.cap(1));
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
			QRegExp rxtags(m_site->value("Regex/Tags"));
			rxtags.setMinimal(true);
			QStringList tags = QStringList();
			int p = 0;
			while (((p = rxtags.indexIn(m_source, p)) != -1))
			{
				if (!tags.contains(rxtags.cap(2)))
				{
					m_tags.append(Tag(rxtags.cap(2), rxtags.cap(1), rxtags.cap(3).toInt()));
					tags.append(rxtags.cap(2));
				}
				p += rxtags.matchedLength();
			}
		}

		// Getting images
		QRegExp rx(m_site->value("Regex/Image"));
		QStringList order = m_site->value("Regex/Order").split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while ((pos = rx.indexIn(m_source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QMap<QString,QString> d;
			for (int i = 0; i < order.size(); i++)
			{
				QString ord = order.at(i);
				if (!d.contains(ord) || d[ord].isEmpty())
				{ d[ord] = rx.cap(i + 1); }
			}

			// JSON elements
			if (order.contains("json") && !d["json"].isEmpty())
			{
				QVariant src = Json::parse(d["json"]);
				if (!src.isNull())
				{
					QMap<QString,QVariant> map = src.toMap();
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
			if (data.contains("success") && data["success"].toBool() == false)
			{
				log(QString("JSON error reply: \"%1\"").arg(data["reason"].toString()));
				emit finishedLoading(this, LoadResult::Error);
				return;
			}

			if (data.contains("total"))
			{ m_imagesCount = data.value("total").toInt(); }

			QMap<QString, QVariant> sc;
			QList<QVariant> sourc = src.toList();
			if (sourc.isEmpty())
			{ sourc = data.value("images").toList(); }
			if (sourc.isEmpty())
			{ sourc = data.value("search").toList(); }

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
					QStringList infos, assoc;
					infos << "created_at" << "source" << "file_url" << "preview_url" << "width" << "md5" << "height" << "id" << "tags" << "author" << "score";
					assoc << "created_at" << "source_url" << "image" << "image" << "width" << "sha512_hash" << "height" << "id" << "tags" << "uploader" << "score";
					for (int i = 0; i < infos.count(); i++)
					{ d[infos.at(i)] = sc.value(assoc.at(i)).toString().trimmed(); }
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
						for (QVariant tagData : tgs)
						{
							QMap<QString, QVariant> tag = tagData.toMap();
							if (tag.contains("name"))
								tags.append(Tag(tag["name"].toString(), Tag::GetType(tag["type"].toString(), tagTypes), tag["count"].toInt()));
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
			log(QString("Error parsing JSON file: \"%1\"").arg(m_source.left(500)));
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
	if (!m_originalUrl.contains("{page}") && !m_originalUrl.contains("{pagepart}") && !m_originalUrl.contains("{pid}"))
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
	// Check redirection
	QUrl redir = m_replyTags->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_urlRegex = m_site->fixUrl(redir.toString(), m_urlRegex);
		loadTags();
		return;
	}

	QString source = QString::fromUtf8(m_replyTags->readAll());

	if (m_site->contains("Regex/Tags"))
	{
		QStringList order = m_site->value("Regex/TagsOrder").split('|', QString::SkipEmptyParts);
		QList<Tag> tgs = Tag::FromRegexp(m_site->value("Regex/Tags"), order, source);
		if (!tgs.isEmpty())
		{ m_tags = tgs; }
	}

	parseNavigation(source);

	// Wiki
	m_wiki.clear();
	if (m_site->contains("Regex/Wiki"))
	{
		QRegExp rxwiki(m_site->value("Regex/Wiki"));
		rxwiki.setMinimal(true);
		if (rxwiki.indexIn(source) != -1)
		{
			m_wiki = rxwiki.cap(1);
			m_wiki.remove("/wiki/show?title=").remove(QRegExp("<p><a href=\"([^\"]+)\">Full entry &raquo;</a></p>")).replace("<h6>", "<span class=\"title\">").replace("</h6>", "</span>");
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
		QRegExp rx(m_site->value("Regex/NextPage"));
		if (rx.indexIn(source, 0) >= 0)
		{ m_urlNextPage = QUrl(rx.cap(1)); }
	}
	if (m_site->contains("Regex/PrevPage") && m_urlPrevPage.isEmpty())
	{
		QRegExp rx(m_site->value("Regex/PrevPage"));
		if (rx.indexIn(source, 0) >= 0)
		{ m_urlPrevPage = QUrl(rx.cap(1)); }
	}

	// Last page
	if (m_site->contains("LastPage") && m_pagesCount < 1)
	{ m_pagesCount = m_site->value("LastPage").toInt(); }
	if (m_site->contains("Regex/LastPage") && m_pagesCount < 1)
	{
		QRegExp rxlast(m_site->value("Regex/LastPage"));
		rxlast.indexIn(source, 0);
		int cnt = rxlast.cap(1).remove(",").toInt();
		if (cnt > 0)
		{
			m_pagesCount = cnt;
			if (m_originalUrl.contains("{pid}"))
			{
				int ppid = m_api->contains("Urls/Limit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;
				m_pagesCount = floor((float)m_pagesCount / (float)ppid) + 1;
			}
		}
	}

	// Count images
	if (m_site->contains("Regex/Count") && m_imagesCount < 1)
	{
		QRegExp rxlast(m_site->value("Regex/Count"));
		rxlast.indexIn(source, 0);
		m_imagesCount = rxlast.cap(1).remove(",").toInt();
	}
	if (m_imagesCount < 1)
	{
		for (Tag tag : m_tags)
		{
			if (tag.text() == m_search.join(" "))
			{
				m_imagesCount = tag.count();
			}
		}
	}
}

void PageApi::clear()
{
	m_images.clear();
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
int PageApi::highLimit()
{
	if (m_api->contains("Urls/Limit"))
		return m_api->value("Urls/Limit").toInt();
	return 0;
}

int PageApi::imagesCount(bool guess)
{
	int perPage = m_api->contains("Urls/Limit") && !m_api->contains("Urls/MaxLimit") ? m_api->value("Urls/Limit").toInt() : m_imagesPerPage;

	if (m_imagesCount < 0 && guess && m_pagesCount >= 0)
		return m_pagesCount * perPage;

	return m_imagesCount;
}
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
	for (QSharedPointer<Image> img : m_images)
		if (img->id() > maxId || maxId == 0)
			maxId = img->id();
	return maxId;
}
int PageApi::minId()
{
	int minId = 0;
	for (QSharedPointer<Image> img : m_images)
		if (img->id() < minId || minId == 0)
			minId = img->id();
	return minId;
}

void PageApi::setUrl(QUrl url)
{
	m_url = url;
}
