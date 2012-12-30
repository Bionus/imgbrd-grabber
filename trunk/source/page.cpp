#include <QSettings>
#include <QFile>
#include <QDebug>
#include "page.h"
#include "json.h"
#include "math.h"



Page::Page(Site *site, QMap<QString,Site*> *sites, QStringList tags, int page, int limit, QStringList postFiltering, bool smart, QObject *parent, int pool) : QObject(parent), m_site(site), m_postFiltering(postFiltering), m_errors(QStringList()), m_imagesPerPage(limit), m_currentSource(0), m_smart(smart)
{
	m_website = m_site->name();

	QStringList modifiers = QStringList();
	for (int i = 0; i < sites->size(); i++)
	{
		if (sites->value(sites->keys().at(i))->contains("Modifiers"))
		{ modifiers.append(sites->value(sites->keys().at(i))->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts)); }
	}
	if (m_site->contains("Modifiers"))
	{
		QStringList mods = m_site->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts);
		for (int j = 0; j < mods.size(); j++)
		{ modifiers.removeAll(mods[j]); }
	}
	for (int k = 0; k < modifiers.size(); k++)
	{ tags.removeAll(modifiers[k]); }
	m_search = tags;


	m_page = page;
	m_pool = pool;
	fallback();

	m_replyExists = false;
	m_replyTagsExists = false;
	m_currentUrl = 0;
}
Page::~Page()
{ }

void Page::fallback()
{
	if (m_currentSource > m_site->value("Selected").count('/'))
	{
		log(tr("Aucune source valide du site n'a retourné de résultat."));
		m_errors.append(tr("Aucune source valide du site n'a retourné de résultat."));
		m_currentSource++;
		//emit finishedLoading(this);
		return;
	}
	if (m_currentSource > 0)
	{ log(tr("Chargement en %1 échoué. Nouvel essai en %2.").arg(m_format).arg(m_site->value("Selected").split('/').at(m_currentSource))); }

	m_currentSource++;

	QString t = m_search.join(" ");
	if (m_site->contains("DefaultTag") && t.isEmpty())
	{ t = m_site->value("DefaultTag"); }

	m_format = m_site->value("Selected").split('/').at(m_currentSource-1);
	int p = m_page;
	m_blim = m_site->contains("Urls/"+QString::number(m_currentSource)+"/Limit") ? m_site->value("Urls/"+QString::number(m_currentSource)+"/Limit").toInt() : m_imagesPerPage;
	if (m_smart)
	{
		if (m_imagesPerPage > m_blim)
		{ m_imagesPerPage = m_blim; }
		p = (int)floor((m_page - 1.) * m_imagesPerPage / m_blim) + 1;
	}
	p = p - 1 + m_site->value("FirstPage").toInt();

	QRegExp pool("pool:(\\d+)");
	QString url;
	int pos = -1;
	if ((pos = pool.indexIn(t)) != -1)
	{
		for (int i = 1; i <= m_site->value("Selected").count('/') + 1; i++)
		{
			if (m_site->contains("Urls/"+QString::number(i)+"/Pools"))
			{
				url = m_site->value("Urls/"+QString::number(i)+"/Pools");
				url.replace("{pool}", pool.cap(1));
				m_currentSource = i;
				m_format = m_site->value("Selected").split('/').at(m_currentSource-1);
				t = t.remove(pos, pool.cap(0).length()).trimmed();
				break;
			}
		}
		if (url.isEmpty())
		{
			log(tr("Aucune source du site n'est compatible avec les pools."));
			m_errors.append(tr("Aucune source du site n'est compatible avec les pools."));
			m_search.removeAll("pool:"+pool.cap(1));
			t.remove(pool);
			t = t.trimmed();
		}
	}
	if (url.isEmpty())
	{
		if (t.isEmpty() && m_site->contains("Urls/"+QString::number(m_currentSource)+"/Home"))
		{ url = m_site->value("Urls/"+QString::number(m_currentSource)+"/Home"); }
		else
		{ url = m_site->value("Urls/"+QString::number(m_currentSource)+"/Tags"); }
	}

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QString pseudo = m_site->setting("auth/pseudo", settings.value("Login/pseudo", "").toString()).toString();
	QString password = m_site->setting("auth/password", settings.value("Login/password", "").toString()).toString();

	m_originalUrl = url;
	url.replace("{page}", QString::number(p));
	url.replace("{tags}", QUrl::toPercentEncoding(t));
	url.replace("{limit}", QString::number(m_imagesPerPage));
	url.replace("{pseudo}", pseudo);
	url.replace("{password}", password);
	m_url = QUrl::fromEncoded(url.toUtf8());

	if (pool.indexIn(t) != -1 && m_site->contains("Urls/Html/Pools"))
	{
		QString url = m_site->value("Urls/Html/Pools");
		url.replace("{page}", QString::number(p));
		url.replace("{pool}", pool.cap(1));
		url.replace("{tags}", QUrl::toPercentEncoding(t));
		url.replace("{limit}", QString::number(m_imagesPerPage));
		url.replace("{pseudo}", pseudo);
		url.replace("{password}", password);
		m_urlRegex = QUrl::fromEncoded(url.toUtf8());
	}
	else if (m_site->contains("Urls/Html/Tags"))
	{
		QString url = m_site->value("Urls/Html/"+QString(t.isEmpty() && m_site->contains("Urls/Html/Home") ? "Home" : "Tags"));
		url.replace("{page}", QString::number(p));
		url.replace("{tags}", QUrl::toPercentEncoding(t));
		url.replace("{limit}", QString::number(m_imagesPerPage));
		url.replace("{pseudo}", pseudo);
		url.replace("{password}", password);
		m_urlRegex = QUrl::fromEncoded(url.toUtf8());
	}
	else
	{ m_urlRegex = ""; }
}

void Page::load()
{
	if (m_currentSource <= m_site->value("Selected").count('/') + 1)
	{
		m_reply = m_site->get(m_url);
		connect(m_reply, SIGNAL(finished()), this, SLOT(parse()));
		m_replyExists = true;
	}
}
void Page::sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errlist)
{ qnr->ignoreSslErrors(); }
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
		m_replyTags = m_site->get(m_urlRegex);
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

void Page::parse()
{
	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = redir;
		load();
		return;
	}

	// Reading reply and resetting vars
	qDeleteAll(m_images);
	m_images.clear();
	m_tags.clear();
	m_imagesCount = -1;
	m_source = m_reply->readAll();

	if (m_source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{
			log("Loading error: "+m_reply->errorString());
			fallback();
			load();
		}
		return;
	}

	int first = m_smart ? ((m_page - 1) * m_imagesPerPage) % m_blim : 0;

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
			for (int id = 0; id < nodeList.count(); id++)
			{
				QMap<QString,QString> d;
				QStringList infos;
				infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = nodeList.at(id + first).attributes().namedItem(infos.at(i)).nodeValue().trimmed(); }
				if (!d["preview_url"].startsWith("http://") && !d["preview_url"].startsWith("https://"))
				{ d["preview_url"] = "http://"+m_site->value("Url")+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://") && !d["file_url"].startsWith("https://"))
				{ d["file_url"] = "http://"+m_site->value("Url")+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				d["page_url"] = m_site->value("Urls/Html/Post");
				QString t = m_search.join(" ");
				if (m_site->contains("DefaultTag") && t.isEmpty())
				{ t = m_site->value("DefaultTag"); }
				d["page_url"].replace("{tags}", QUrl::toPercentEncoding(t));
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{
					img->deleteLater();
					log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id + first + 1), error));
				}
			}
		}
	}

	// RSS
	else if (m_format == "rss")
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
				// QDateTime::fromString(date, "ddd, dd MMM yyyy hh:mm:ss +0000"); // shimmie date format
				d.insert("page_url", dat["link"]);
				d.insert("tags", dat["media:keywords"]);
				d.insert("preview_url", dat["media:thumbnail"]);
				d.insert("sample_url", dat["media:content"]);
				d.insert("file_url", dat["media:content"]);
				if (!d.contains("id"))
				{
					QRegExp rx("/(\\d+)");
					rx.indexIn(d["page_url"]);
					d.insert("id", rx.cap(1));
				}
				if (!d["preview_url"].startsWith("http://") && !d["preview_url"].startsWith("https://"))
				{ d["preview_url"] = "http://"+m_site->value("Url")+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["sample_url"].startsWith("http://") && !d["sample_url"].startsWith("https://"))
				{ d["sample_url"] = "http://"+m_site->value("Url")+QString(d["sample_url"].startsWith("/") ? "" : "/")+d["sample_url"]; }
				if (!d["file_url"].startsWith("http://") && !d["file_url"].startsWith("https://"))
				{ d["file_url"] = "http://"+m_site->value("Url")+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				if (m_site->contains("Urls/Rss/Image"))
				{
					if (m_site->value("Urls/Rss/Image").contains("->"))
					{ d["file_url"].replace(m_site->value("Urls/Rss/Image").left(m_site->value("Urls/Rss/Image").indexOf("->")), m_site->value("Urls/Rss/Image").right(m_site->value("Urls/Json/Image").size()-m_site->value("Urls/Json/Image").indexOf("->")-2)); }
					else
					{
						d["file_url"] = m_site->value("Urls/Rss/Image");
						d["file_url"].replace("{id}", d["id"])
						.replace("{md5}", d["md5"])
						.replace("{ext}", "jpg");
					}
				}
				d["page_url"] = m_site->value("Urls/Html/Post");
				QString t = m_search.join(" ");
				if (m_site->contains("DefaultTag") && t.isEmpty())
				{ t = m_site->value("DefaultTag"); }
				d["page_url"].replace("{tags}", QUrl::toPercentEncoding(t));
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{
					img->deleteLater();
					log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id + first + 1), error));
				}
			}
		}
	}

	// Regexes
	else if (m_format == "regex")
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
			{ d[order.at(i)] = rx.cap(i+1); }

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

			if (!d["preview_url"].startsWith("http://") && !d["preview_url"].startsWith("https://"))
			{ d["preview_url"] = "http://"+m_site->value("Url")+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
			if (!d.contains("file_url"))
			{
				if (!m_site->contains("Regex/ImageUrl"))
				{ d["file_url"] = d["preview_url"]; }
			}
			else if (!d["file_url"].startsWith("http://") && !d["file_url"].startsWith("https://"))
			{ d["file_url"] = "http://"+m_site->value("Url")+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
			if (m_site->contains("Urls/Html/Image"))
			{
				if (m_site->value("Urls/Html/Image").contains("->"))
				{
					QStringList replaces = m_site->value("Urls/Html/Image").split("&");
					foreach (QString rep, replaces)
					{
						QRegExp rgx(rep.left(rep.indexOf("->")));
						d["file_url"].replace(rgx, rep.right(rep.size() - rep.indexOf("->") - 2));
					}
				}
				else
				{
					d["file_url"] = m_site->value("Urls/Html/Image");
					d["file_url"].replace("{id}", d["id"])
					.replace("{md5}", d["md5"])
					.replace("{ext}", "jpg");
				}
			}
			qDebug() << 1 << d["preview_url"];
			qDebug() << 2 << d["file_url"];

			if (!d.contains("page_url"))
			{
				d["page_url"] = m_site->value("Urls/Html/Post");
				QString t = m_search.join(" ");
				if (m_site->contains("DefaultTag") && t.isEmpty())
				{ t = m_site->value("DefaultTag"); }
				d["page_url"].replace("{tags}", QUrl::toPercentEncoding(t));
				d["page_url"].replace("{id}", d["id"]);
			}

			int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
			Image *img = new Image(d, timezonedecay, this);
			QString error = img->filter(m_postFiltering);
			if (error.isEmpty())
			{ m_images.append(img); }
			else
			{
				img->deleteLater();
				log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error));
			}
			id++;
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
			for (int id = 0; id < sourc.count(); id++)
			{
				sc = sourc.at(id + first).toMap();
				QMap<QString,QString> d;
				QStringList infos;
				infos << "created_at" << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = sc.value(infos.at(i)).toString().trimmed(); }
				if (!d["preview_url"].startsWith("http://") && !d["preview_url"].startsWith("https://"))
				{ d["preview_url"] = "http://"+m_site->value("Url")+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d.contains("file_url") || d["file_url"].isEmpty())
				{
					if (!m_site->contains("Regex/ImageUrl"))
					{ d["file_url"] = d["preview_url"]; }
				}
				else if (!d["file_url"].startsWith("http://") && !d["file_url"].startsWith("https://"))
				{ d["file_url"] = "http://"+m_site->value("Url")+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				if (m_site->contains("Urls/Json/Image"))
				{
					if (m_site->value("Urls/Json/Image").contains("->"))
					{ d["file_url"].replace(m_site->value("Urls/Json/Image").left(m_site->value("Urls/Json/Image").indexOf("->")), m_site->value("Urls/Json/Image").right(m_site->value("Urls/Json/Image").size()-m_site->value("Urls/Json/Image").indexOf("->")-2)); }
					else
					{
						d["file_url"] = m_site->value("Urls/Json/Image");
						d["file_url"].replace("{id}", d["id"])
						.replace("{md5}", d["md5"])
						.replace("{ext}", "jpg");
					}
				}
				d["page_url"] = m_site->value("Urls/Html/Post");
				QString t = m_search.join(" ");
				if (m_site->contains("DefaultTag") && t.isEmpty())
				{ t = m_site->value("DefaultTag"); }
				d["page_url"].replace("{tags}", QUrl::toPercentEncoding(t));
				d["page_url"].replace("{id}", d["id"]);
				int timezonedecay = QDateTime::currentDateTime().time().hour()-QDateTime::currentDateTime().toUTC().addSecs(-60*60*4).time().hour();
				Image *img = new Image(d, timezonedecay, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{
					img->deleteLater();
					log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id + first + 1), error));
				}
			}
		}
		else
		{
			fallback();
			load();
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

	// Getting last page
	if (m_site->contains("LastPage") && m_imagesCount < 1)
	{ m_imagesCount = m_site->value("LastPage").toInt()*m_imagesPerPage; }
	if (m_site->contains("Regex/LastPage") && m_imagesCount < 1)
	{
		QRegExp rxlast(m_site->value("Regex/LastPage"));
		rxlast.indexIn(m_source, 0);
		m_imagesCount = rxlast.cap(1).remove(",").toInt() * m_imagesPerPage;
	}
	if (m_site->contains("Regex/Count") && m_imagesCount < 1)
	{
		QRegExp rxlast(m_site->value("Regex/Count"));
		rxlast.indexIn(m_source, 0);
		m_imagesCount = rxlast.cap(1).remove(",").toInt();
	}

	// Virtual paging
	int firstImage = 0;
	int lastImage = m_smart ? m_imagesPerPage : m_images.size();
	if (!m_originalUrl.contains("{page}"))
	{
		firstImage = m_imagesPerPage * (m_page - 1);
		lastImage = m_imagesPerPage;
	}
	while (firstImage > 0)
	{
		m_images.removeFirst();
		firstImage--;
	}
	while (m_images.size() > lastImage)
	{ m_images.removeLast(); }

	m_reply->deleteLater();
	m_replyExists = false;

	QString t = m_search.join(" ");
	if (m_site->contains("DefaultTag") && t.isEmpty())
	{ t = m_site->value("DefaultTag"); }
	if (!m_search.isEmpty() && !m_site->value("Urls/"+QString::number(m_currentSource)+"/"+(t.isEmpty() && !m_site->contains("Urls/"+QString::number(m_currentSource)+"/Home") ? "Home" : "Tags")).contains("{tags}"))
	{ m_errors.append(tr("La recherche par tags est impossible avec la source choisie (%1).").arg(m_format)); }

	emit finishedLoading(this);
}
void Page::parseTags()
{
	// Check redirection
	QUrl redir = m_replyTags->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_urlRegex = redir;
		loadTags();
		return;
	}

	QString source = QString::fromUtf8(m_replyTags->readAll());
	m_tags.clear();

	if (m_site->contains("Regex/Tags"))
	{
		QRegExp rxtags(m_site->value("Regex/Tags"));
		rxtags.setMinimal(true);
		int p = 0;
		while ((p = rxtags.indexIn(source, p)) != -1)
		{
			p += rxtags.matchedLength();
			if (rxtags.captureCount() == 4)
			{ m_tags.append(Tag(rxtags.cap(4), rxtags.cap(1), rxtags.cap(3).toInt())); }
			else
			{ m_tags.append(Tag(rxtags.cap(2), rxtags.cap(1), rxtags.cap(3).toInt())); }
		}
	}

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
	m_replyTagsExists = false;

	emit finishedLoadingTags(this);
}

QList<Image*>	Page::images()		{ return m_images;		}
Site			*Page::site()		{ return m_site;		}
QUrl			Page::url()			{ return m_url;			}
int				Page::imagesCount()	{ return m_imagesCount;	}
QString			Page::source()		{ return m_source;		}
QString			Page::website()		{ return m_website;		}
QString			Page::wiki()		{ return m_wiki;		}
QList<Tag>		Page::tags()		{ return m_tags;		}
QStringList		Page::search()		{ return m_search;		}
QStringList		Page::errors()		{ return m_errors;		}
