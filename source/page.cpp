#include "page.h"
#include "json.h"



Page::Page(QMap<QString,QMap<QString,QString> > *sites, QString site, QStringList tags, int page, int limit, QStringList postFiltering, QObject *parent) : QObject(parent), m_postFiltering(postFiltering), m_search(tags), m_imagesPerPage(limit), m_currentSource(0)
{

	// Some definitions from parameters
	m_site = sites->value(site);
	m_website = site;

	m_page = page;
	fallback();

	m_replyExists = false;
	m_replyTagsExists = false;
	m_currentUrl = 0;
}
Page::~Page()
{ }

void Page::fallback()
{
	if (m_currentSource > 3)
	{
		log(tr("Aucune source valide du site n'a retourné de résultat."));
		return;
	}
	if (m_currentSource > 0)
	{ log(tr("Chargement en %1 échoué. Nouvel essai en %2.").arg(m_format).arg(m_site["Selected"].split('/').at(m_currentSource))); }

	m_currentSource++;

	QString t = m_search.join(" ").replace("&", "%26");
	if (m_site.contains("DefaultTag") && t.isEmpty())
	{ t = m_site["DefaultTag"]; }

	m_format = m_site["Selected"].split('/').at(m_currentSource-1);
	m_blim = m_site.contains("Urls/"+QString::number(m_currentSource)+"/Limit") ? m_site["Urls/"+QString::number(m_currentSource)+"/Limit"].toInt() : m_imagesPerPage;
	if (m_imagesPerPage > m_blim)
	{ m_imagesPerPage = m_blim; }
	int p = floor((m_page - 1) * m_imagesPerPage / m_blim) + m_site["FirstPage"].toInt();

	QString url = m_site["Urls/"+QString::number(m_currentSource)+"/Tags"];
	url.replace("{page}", QString::number(p));
	url.replace("{tags}", t);
	url.replace("{limit}", QString::number(m_imagesPerPage));
	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	url.replace("{pseudo}", settings->value("Login/pseudo").toString());
	url.replace("{password}", settings->value("Login/password").toString());
	m_url = QUrl::fromEncoded(url.toAscii());

	if (m_site.contains("Urls/Html/Tags"))
	{
		QString url = m_site["Urls/Html/Tags"];
		url.replace("{page}", QString::number(p));
		url.replace("{tags}", t);
		url.replace("{limit}", QString::number(m_imagesPerPage));
		QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
		url.replace("{pseudo}", settings->value("Login/pseudo").toString());
		url.replace("{password}", settings->value("Login/password").toString());
		m_urlRegex = QUrl(url);
	}
	else
	{ m_urlRegex = ""; }
}

void Page::load()
{
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parse(QNetworkReply*)));
	QNetworkRequest r(m_url);
		r.setRawHeader("Referer", m_url.toString().toAscii());
	m_reply = manager->get(r);
	m_replyExists = true;
}
void Page::abort()
{
	if (m_replyExists)
	{
		if (m_reply->isRunning())
		{ m_reply->abort(); }
	}
}

void Page::loadTags()
{
	if (!m_urlRegex.isEmpty())
	{
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseTags(QNetworkReply*)));
		m_replyTags = manager->get(QNetworkRequest(m_urlRegex));
		m_replyTagsExists = true;
	}
}
void Page::abortTags()
{
	if (m_replyTagsExists)
	{
		if (m_replyTags->isRunning())
		{ m_replyTags->abort(); }
	}
}

void Page::parse(QNetworkReply* r)
{
	// Check redirection
	QUrl redir = r->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = redir;
		load();
		return;
	}

	// Reading reply and resetting vars
	m_images.clear();
	m_tags.clear();
	m_imagesCount = -1;
	m_source = r->readAll();
	int first = ((m_page - 1) * m_imagesPerPage) % m_blim;

	// Getting last page
	if (m_site.contains("LastPage"))
	{ m_imagesCount = m_site["LastPage"].toInt()*m_imagesPerPage; }
	else if (m_site.contains("Regex/LastPage"))
	{
		QRegExp rxlast(m_site["Regex/LastPage"]);
		rxlast.setMinimal(true);
		rxlast.indexIn(m_source, 0);
		m_imagesCount = rxlast.cap(1).remove(",").toInt()*m_imagesPerPage;
	}
	else if (m_site.contains("Regex/Count"))
	{
		QRegExp rxlast(m_site["Regex/Count"]);
		rxlast.setMinimal(true);
		rxlast.indexIn(m_source, 0);
		m_imagesCount = rxlast.cap(1).remove(",").toInt();
	}

	if (m_source.isEmpty())
	{
		fallback();
		load();
		return;
	}

	// XML
	if (m_format == "xml")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
			fallback();
			load();
			return;
		}
		QDomElement docElem = doc.documentElement();

		// Getting last page
		m_imagesCount = docElem.attributes().namedItem("count").nodeValue().toFloat();

		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int id = first; id < nodeList.count() && id < m_imagesPerPage + first; id++)
			{
				QStringMap d;
				QStringList infos;
				infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = nodeList.at(id).attributes().namedItem(infos.at(i)).nodeValue(); }
				if (!d["preview_url"].startsWith("http://"))
				{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://"))
				{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				d["page_url"] = m_site["Urls/Html/Post"];
				QString t = m_search.join(" ").replace("&", "%26");
				if (m_site.contains("DefaultTag") && t.isEmpty())
				{ t = m_site["DefaultTag"]; }
				d["page_url"].replace("{tags}", t);
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			}
		}
	}

	// JSON
	else if (m_format == "json")
	{
		QVariant src = Json::parse(m_source);
		if (!src.isNull())
		{
			QMap<QString, QVariant> sc;
			QList<QVariant> sourc = src.toList();
			for (int id = first; id < sourc.count() && id < m_imagesPerPage + first; id++)
			{
				sc = sourc.at(id).toMap();
				QStringMap d;
				QStringList infos;
				infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = sc.value(infos.at(i)).toString(); }
				if (!d["preview_url"].startsWith("http://"))
				{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://"))
				{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				d["page_url"] = m_site["Urls/Html/Post"];
				QString t = m_search.join(" ").replace("&", "%26");
				if (m_site.contains("DefaultTag") && t.isEmpty())
				{ t = m_site["DefaultTag"]; }
				d["page_url"].replace("{tags}", t);
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			}
		}
		else
		{
			fallback();
			load();
			return;
		}
	}

	// RSS
	if (m_format == "rss")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(tr("Erreur lors de l'analyse du fichier RSS : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
			fallback();
			load();
			return;
		}
		QDomElement docElem = doc.documentElement();

		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("item");
		if (nodeList.count() > 0)
		{
			for (int id = first; id < nodeList.count() && id < m_imagesPerPage + first; id++)
			{
				QDomNodeList children = nodeList.at(id).childNodes();
				QStringMap d, dat;
				for (int i = 0; i < children.size(); i++)
				{
					QString content = children.at(i).childNodes().at(0).nodeValue();
					if (!content.isEmpty())
					{ dat.insert(children.at(i).nodeName(), content); }
					else
					{ dat.insert(children.at(i).nodeName(), children.at(i).attributes().namedItem("url").nodeValue()); }
				}
				// QDateTime::fromString(date, "ddd, dd MMM yyyy hh:mm:ss +0000"); // shimmie date format
				d.insert("page_url", dat["link"]);
				d.insert("tags", dat["media:keywords"]);
				d.insert("preview_url", dat["media:thumbnail"]);
				d.insert("sample_url", dat["media:content"]);
				d.insert("file_url", dat["media:content"]);
				if (!d.contains("id"))
				{
					QRegExp id("/(\\d+)");
					id.indexIn(d["page_url"]);
					d.insert("id", id.cap(1));
				}
				if (!d["preview_url"].startsWith("http://"))
				{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://"))
				{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				if (m_site.contains("Urls/Rss/Image"))
				{
					if (m_site["Urls/Rss/Image"].contains("->"))
					{ d["file_url"].replace(m_site["Urls/Rss/Image"].left(m_site["Urls/Rss/Image"].indexOf("->")), m_site["Urls/Rss/Image"].right(m_site["Urls/Rss/Image"].indexOf("->")+1)); }
					else
					{
						d["file_url"] = m_site["Urls/Rss/Image"];
						d["file_url"].replace("{id}", d["id"])
						.replace("{md5}", d["md5"])
						.replace("{ext}", "jpg");
					}
				}
				d["page_url"] = m_site["Urls/Html/Post"];
				QString t = m_search.join(" ").replace("&", "%26");
				if (m_site.contains("DefaultTag") && t.isEmpty())
				{ t = m_site["DefaultTag"]; }
				d["page_url"].replace("{tags}", t);
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			}
		}
	}

	// Regexes
	else if (m_format == "regex")
	{
		// Getting tags
		if (m_site.contains("Regex/Tags"))
		{
			QRegExp rxtags(m_site["Regex/Tags"]);
			rxtags.setMinimal(true);
			int p = 0;
			while (((p = rxtags.indexIn(m_source, p)) != -1))
			{
				p += rxtags.matchedLength();
				m_tags.append(new Tag(rxtags.cap(2), rxtags.cap(1), rxtags.cap(3).toInt()));
			}
		}

		// Getting images
		QRegExp rx(m_site["Regex/Image"]);
		QStringList order = m_site["Regex/Order"].split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while ((pos = rx.indexIn(m_source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QStringMap d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			if (!d["preview_url"].startsWith("http://"))
			{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
			if (!d["file_url"].startsWith("http://"))
			{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
			if (m_site.contains("Urls/Html/Image"))
			{
				if (m_site["Urls/Rss/Image"].contains("->"))
				{ d["file_url"].replace(m_site["Urls/Rss/Image"].left(m_site["Urls/Rss/Image"].indexOf("->")), m_site["Urls/Rss/Image"].right(m_site["Urls/Rss/Image"].indexOf("->")+1)); }
				else
				{
					d["file_url"] = m_site["Urls/Html/Image"];
					d["file_url"].replace("{id}", d["id"])
					.replace("{md5}", d["md5"])
					.replace("{ext}", "jpg");
				}
			}
			d["page_url"] = m_site["Urls/Html/Post"];
			QString t = m_search.join(" ").replace("&", "%26");
			if (m_site.contains("DefaultTag") && t.isEmpty())
			{ t = m_site["DefaultTag"]; }
			d["page_url"].replace("{tags}", t);
			d["page_url"].replace("{id}", d["id"]);
			int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
			Image *img = new Image(d, timezonedecay, this);
			QString error = img->filter(m_postFiltering);
			if (error.isEmpty())
			{ m_images.append(img); }
			else
			{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			id++;
		}
	}

	// If tags have not been retrieved yet
	if (m_tags.isEmpty())
	{
		QStringList tagsGot;
		for (int i = 0; i < m_images.count(); i++)
		{
			QList<Tag*> tags = m_images.at(i)->tags();
			for (int t = 0; t < tags.count(); t++)
			{
				if (tagsGot.contains(tags.at(t)->text()))
				{ m_tags[tagsGot.indexOf(tags.at(t)->text())]->setCount(m_tags[tagsGot.indexOf(tags.at(t)->text())]->count()+1); }
				else
				{
					m_tags.append(tags.at(t));
					tagsGot.append(tags.at(t)->text());
				}
			}
		}
	}

	emit finishedLoading(this);
}
void Page::parseTags(QNetworkReply *r)
{
	m_tags.clear();
	if (m_site.contains("Regex/Tags"))
	{
		QString source = r->readAll();
		QRegExp rxtags(m_site["Regex/Tags"]);
		rxtags.setMinimal(true);
		int p = 0;
		while (((p = rxtags.indexIn(source, p)) != -1))
		{
			p += rxtags.matchedLength();
			m_tags.append(new Tag(rxtags.cap(2), rxtags.cap(1), rxtags.cap(3).toInt()));
		}
	}
	emit finishedLoadingTags(this);
}

QList<Image*>			Page::images()		{ return m_images;		}
QMap<QString,QString>	Page::site()		{ return m_site;		}
QUrl					Page::url()			{ return m_url;			}
int						Page::imagesCount()	{ return m_imagesCount;	}
QString					Page::source()		{ return m_source;		}
QString					Page::website()		{ return m_website;		}
QList<Tag*>				Page::tags()		{ return m_tags;		}
QStringList				Page::search()		{ return m_search;		}
