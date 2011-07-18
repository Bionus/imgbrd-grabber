#include "page.h"
#include "json.h"



Page::Page(QMap<QString,QMap<QString,QString> > *sites, QString site, QStringList tags, int page, int limit, QStringList postFiltering, QObject *parent) : QObject(parent), m_postFiltering(postFiltering), m_imagesPerPage(limit)
{
	// Some definitions from parameters
	m_site = sites->value(site);
	m_format = m_site["Selected"];
	QString url = m_site["Urls/Selected/Tags"];
	url.replace("{page}", QString::number(page-1+m_site["FirstPage"].toInt()));
	url.replace("{tags}", tags.join(" ").replace("&", "%26"));
	url.replace("{limit}", QString::number(limit));
	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	url.replace("{pseudo}", settings->value("Login/pseudo").toString());
	url.replace("{password}", settings->value("Login/password").toString());
	m_url = QUrl(url);
}
Page::~Page()
{ }

void Page::load()
{
	// Load page, and when finished parse result
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(parse(QNetworkReply*)));
	manager->get(QNetworkRequest(m_url));
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

	// Reading reply
	m_source = r->readAll();

	// XML
	if (m_site["Selected"] == "xml")
	{
		// Initializations
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(m_source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
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
				QStringMap d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = nodeList.at(id).attributes().namedItem(infos.at(i)).nodeValue(); }
				if (!d["preview_url"].startsWith("http://"))
				{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://"))
				{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				d["page_url"] = m_site["Urls/Html/Post"];
				d["page_url"].replace("{id}", d["id"]);
				Image *img = new Image(d, 0, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			}
		}
	}

	// JSON
	else if (m_site["Selected"] == "json")
	{
		QVariant src = Json::parse(m_source);
		if (!src.isNull())
		{
			QMap<QString, QVariant> sc;
			QList<QVariant> sourc = src.toList();
			for (int id = 0; id < sourc.count(); id++)
			{
				sc = sourc.at(id).toMap();
				QStringMap d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = sc.value(infos.at(i)).toString(); }
				if (!d["preview_url"].startsWith("http://"))
				{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
				if (!d["file_url"].startsWith("http://"))
				{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
				d["page_url"] = m_site["Urls/Html/Post"];
				d["page_url"].replace("{id}", d["id"]);
				Image *img = new Image(d, 0, this);
				QString error = img->filter(m_postFiltering);
				if (error.isEmpty())
				{ m_images.append(img); }
				else
				{ log(tr("Image #%1 ignored. Reason: %2.").arg(QString::number(id+1), error)); }
			}
		}
	}

	// Regexes
	else if (m_site["Selected"] == "regex")
	{
		// Getting last page
		QRegExp rxlast(m_site["Regex/LastPage"]);
		rxlast.setMinimal(true);
		rxlast.indexIn(m_source, 0);
		m_imagesCount = rxlast.cap(1).toInt()*m_imagesPerPage;

		// Getting tags
		qDebug() << m_site["Regex/Tags"];
		QRegExp rxtags(m_site["Regex/Tags"]);
		rxtags.setMinimal(true);
		int p = 0;
		while (((p = rxtags.indexIn(m_source, p)) != -1))
		{
			p += rxtags.matchedLength();
			QMap<QString,QString> tg;
			tg["tag"] = rxtags.cap(2);
			tg["type"] = rxtags.cap(1);
			tg["count"] = rxtags.cap(3);
			m_tags.append(tg);
		}

		// Getting images
		QRegExp rx(m_site["Regex/Image"]);
		QStringList order = m_site["Regex/Order"].split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while (((pos = rx.indexIn(m_source, pos)) != -1))// && id < ui->spinImagesPerPage->value()
		{
			pos += rx.matchedLength();
			QStringMap d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			if (!d["preview_url"].startsWith("http://"))
			{ d["preview_url"] = "http://"+m_site["Url"]+QString(d["preview_url"].startsWith("/") ? "" : "/")+d["preview_url"]; }
			if (!d["file_url"].startsWith("http://"))
			{ d["file_url"] = "http://"+m_site["Url"]+QString(d["file_url"].startsWith("/") ? "" : "/")+d["file_url"]; }
			if (!m_site.contains("Urls/Html/Image"))
			{
				d["file_url"] = d["preview_url"];
				d["file_url"].remove("preview/");
			}
			else
			{
				d["file_url"] = m_site["Urls/Html/Image"];
				d["file_url"].replace("{id}", d["id"])
				.replace("{md5}", d["md5"])
				.replace("{ext}", "jpg");
			}
			d["page_url"] = m_site["Urls/Html/Post"];
			d["page_url"].replace("{id}", d["id"]);
			Image *img = new Image(d, 0, this);
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
			QStringList tags = m_images.at(i)->tags();
			for (int t = 0; t < tags.count(); t++)
			{
				QMap<QString,QString> tg;
				tg["tag"] = tags.at(t);
				tg["type"] = "unknown";
				tg["count"] = "1";
				if (tagsGot.contains(tags.at(t)))
				{ m_tags[tagsGot.indexOf(tags.at(t))]["count"] = QString::number(m_tags[tagsGot.indexOf(tags.at(t))]["count"].toInt()+1); }
				else
				{
					m_tags.append(tg);
					tagsGot.append(tags.at(t));
				}
			}
		}
	}

	emit finishedLoading(this);
}

QList<Image*>					Page::images()		{ return m_images;		}
QMap<QString,QString>			Page::site()		{ return m_site;		}
QUrl							Page::url()			{ return m_url;			}
int								Page::imagesCount()	{ return m_imagesCount;	}
QString							Page::source()		{ return m_source;		}
QList<QMap<QString,QString> >	Page::tags()		{ return m_tags;		}
