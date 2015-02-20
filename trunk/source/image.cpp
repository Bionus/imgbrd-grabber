#include <QtScript>
#include "image.h"
#include "functions.h"


Image::Image(QMap<QString, QString> details, Page* parent)
{
	// Parents
	m_site = parent != NULL ? parent->website() : (details.contains("website") ? details["website"] : "");
	m_parentSite = parent != NULL ? parent->site() : (details.contains("site") ? (Site*)details["site"].toInt() : NULL);
	if (m_parentSite == NULL)
	{
		log("Image has null parent, aborting creation.");
		return;
	}

	// Get file url and try to improve it to save bandwidth
	m_url = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]).toString() : "";
	QString ext = getExtension(m_url);
	if (details.contains("sample_url"))
	{
		QString sampleExt = getExtension(details["sample_url"]);
		if (sampleExt != "jpg" && sampleExt != ext)
			m_url = setExtension(m_url, sampleExt);
	}
	else if (details.contains("image") && details["image"].contains("MB // gif\" height=\"") && !m_url.endsWith(".gif", Qt::CaseInsensitive))
	{ m_url = setExtension(m_url, "gif"); }

	// Other details
	m_md5 = details.contains("md5") ? details["md5"] : "";
	m_author = details.contains("author") ? details["author"] : "";
	m_status = details.contains("status") ? details["status"] : "";
	m_filename = details.contains("filename") ? details["filename"] : "";
	m_folder = details.contains("folder") ? details["folder"] : "";
	m_search = parent != NULL ? parent->search() : QStringList();
	m_id = details.contains("id") ? details["id"].toInt() : 0;
	m_score = details.contains("score") ? details["score"].toInt() : 0;
	m_hasScore = details.contains("score");
	m_parentId = details.contains("parent_id") ? details["parent_id"].toInt() : 0;
	m_fileSize = details.contains("file_size") ? details["file_size"].toInt() : 0;
	m_authorId = details.contains("creator_id") ? details["creator_id"].toInt() : 0;
	m_hasChildren = details.contains("has_children") ? details["has_children"] == "true" : false;
	m_hasNote = details.contains("has_note") ? details["has_note"] == "true" : false;
	m_hasComments = details.contains("has_comments") ? details["has_comments"] == "true" : false;
	m_pageUrl = details.contains("page_url") ? m_parentSite->fixUrl(details["page_url"]) : QUrl();
	m_fileUrl = details.contains("file_url") ? m_parentSite->fixUrl(details["file_url"]) : QUrl();
	m_sampleUrl = details.contains("sample_url") ? m_parentSite->fixUrl(details["sample_url"]) : QUrl();
	m_previewUrl = details.contains("preview_url") ? m_parentSite->fixUrl(details["preview_url"]) : QUrl();
	m_size = QSize(details.contains("width") ? details["width"].toInt() : 0, details.contains("height") ? details["height"].toInt() : 0);
	m_source = details.contains("source") ? details["source"] : "";

	// Rating
	m_rating = details.contains("rating") ? details["rating"] : "";
	QMap<QString,QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	if (assoc.contains(m_rating))
	{ m_rating = assoc[m_rating]; }

	// Tags
	m_tags = QList<Tag>();
	if (details.contains("tags_general"))
	{
		QStringList t = details["tags_general"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "general"));
		}
		t = details["tags_artist"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "artist"));
		}
		t = details["tags_character"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "character"));
		}
		t = details["tags_copyright"].split(" ");
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg, "copyright"));
		}
	}
	else if (details.contains("tags"))
	{
		QStringList t;
		if (details["tags"].count(", ") != 0 && details["tags"].count(" ") / details["tags"].count(", ") < 2)
		{ t = details["tags"].split(", "); }
		else
		{ t = details["tags"].split(" "); }
		for (int i = 0; i < t.count(); ++i)
		{
			QString tg = t.at(i);
			tg.replace("&amp;", "&");
			m_tags.append(Tag(tg));
		}
	}

	// Creation date
	m_createdAt = QDateTime();
	if (details.contains("created_at"))
	{
		if (details["created_at"].toInt() != 0)
		{ m_createdAt.setTime_t(details["created_at"].toInt()); }
		else
		{ m_createdAt = qDateTimeFromString(details["created_at"]); }
	}

	// Tech details
	m_parent = parent;
	m_previewTry = 0;
	m_loadPreview = NULL;
	m_loadDetails = NULL;
	m_loadImage = NULL;
	m_pools = QList<Pool*>();

	m_settings = new QSettings(savePath("sites/"+m_parentSite->value("Model")+"/"+m_site+"/settings.ini"), QSettings::IniFormat, this);
}
Image::~Image()
{}

void Image::loadPreview()
{
	m_previewTry++;
	m_loadPreview = m_parentSite->get(m_previewUrl, m_parent, "preview");
	m_loadPreview->setParent(this);

	connect(m_loadPreview, SIGNAL(finished()), this, SLOT(parsePreview()));
}
void Image::abortPreview()
{
	if (m_loadPreview != NULL)
	{
		if (m_loadPreview->isRunning())
		{ m_loadPreview->abort(); }
	}
}
void Image::parsePreview()
{
	// Check redirection
	QUrl redir = m_loadPreview->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_previewUrl = redir;
		loadPreview();
		return;
	}

	// Loading error
	if (m_loadPreview->error() != QNetworkReply::NoError)
	{
		log(tr("<b>Erreur :</b> %1").arg(tr("erreur de chargement de la miniature (%1)").arg(m_loadPreview->errorString())));
	}

	// Load preview from raw result
	QByteArray data = m_loadPreview->readAll();
	m_imagePreview.loadFromData(data);
	m_loadPreview->deleteLater();
	m_loadPreview = NULL;

	// If nothing has been received
	if (m_imagePreview.isNull() && m_previewTry <= 3)
	{
		log(tr("<b>Attention :</b> %1").arg(tr("une des miniatures est vide (<a href=\"%1\">%1</a>). Nouvel essai (%2/%3)...").arg(m_previewUrl.toString()).arg(m_previewTry).arg(3)));
		loadPreview();
	}
	else
	{ emit finishedLoadingPreview(this); }
}

void Image::loadDetails()
{
	m_loadDetails = m_parentSite->get(m_pageUrl);
	m_loadDetails->setParent(this);

	connect(m_loadDetails, SIGNAL(finished()), this, SLOT(parseDetails()));
}
void Image::abortTags()
{
	if (m_loadDetails != NULL)
	{
		if (m_loadDetails->isRunning())
		{ m_loadDetails->abort(); }
	}
}
void Image::parseDetails()
{
	// Check redirection
	QUrl redir = m_loadDetails->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_pageUrl = redir;
		loadDetails();
		return;
	}
	QString source = QString::fromUtf8(m_loadDetails->readAll());

	// Pools
	if (m_parentSite->contains("Regex/Pools"))
	{
		m_pools.clear();
		QRegExp rx(m_parentSite->value("Regex/Pools"));
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString previous = rx.cap(1), id = rx.cap(2), name = rx.cap(3), next = rx.cap(4);
			m_pools.append(new Pool(id.toInt(), name, m_id, next.toInt(), previous.toInt()));
		}
	}

	// Tags
	QRegExp rx = QRegExp();
	if (m_parentSite->contains("Regex/ImageTags"))
	{ rx = QRegExp(m_parentSite->value("Regex/ImageTags")); }
	else if (m_parentSite->contains("Regex/Tags"))
	{ rx = QRegExp(m_parentSite->value("Regex/Tags")); }
	if (!rx.isEmpty())
	{
		rx.setMinimal(true);
		int pos = 0;
		QList<Tag> tgs;
		QStringList order = m_parentSite->value("Regex/TagsOrder").split('|', QString::SkipEmptyParts);
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = "", tag = "";
			int count = 1;
			if (order.empty())
			{
				switch (rx.captureCount())
				{
					case 4:	order << "type" << "" << "count" << "tag";	break;
					case 3:	order << "type" << "tag" << "count";		break;
					case 2:	order << "type" << "tag";					break;
					case 1:	order << "tag";								break;
				}
			}
			for (int o = 0; o < order.size(); o++)
			{
				if (order.at(o) == "tag" && tag.isEmpty())
				{ tag = rx.cap(o + 1).replace(" ", "_").replace("&amp;", "&").trimmed(); }
				else if (order.at(o) == "type" && type.isEmpty())
				{
					type = rx.cap(o + 1).toLower().trimmed();
					if (type.contains(", "))
					{ type = type.split(", ").at(0).trimmed(); }
					if (type == "series")
					{ type = "copyright"; }
					else if (type == "mangaka")
					{ type = "artist"; }
					else if (type == "game")
					{ type = "copyright"; }
					else if (type == "studio")
					{ type = "circle"; }
					else if (type == "source")
					{ type = "general"; }
					else if (type == "character group")
					{ type = "general"; }
					else if (type.length() == 1)
					{
						int tpe = type.toInt();
						if (tpe >= 0 && tpe <= 4)
						{
							QStringList types = QStringList() << "general" << "artist" << "unknown" << "copyright" << "character";
							type = types[tpe];
						}
					}
				}
				else if (order.at(o) == "count" && count != 0)
				{ count = rx.cap(o + 1).toLower().endsWith('k') ? rx.cap(3).left(rx.cap(3).length() - 1).toInt() * 1000 : rx.cap(3).toInt(); }
			}
			if (type.isEmpty())
			{ type = "unknown"; }
			tgs.append(Tag(tag, type, count));
		}
		if (!tgs.isEmpty())
		{ m_tags = tgs; }
	}

	// Image url
	if ((m_url.isEmpty() || m_parentSite->contains("Regex/ForceImageUrl")) && m_parentSite->contains("Regex/ImageUrl"))
	{
		QRegExp rx = QRegExp(m_parentSite->value("Regex/ImageUrl"));
		rx.setMinimal(true);
		int pos = 0;
		QString before = m_url;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			m_url = rx.cap(1);
			m_fileUrl = rx.cap(1);
		}
		if (before != m_url)
		{
			m_fileSize = 0;
			emit urlChanged(before, m_url);
		}
	}

	emit finishedLoadingTags(this);
}

int toDate(QString text)
{
	QDateTime date = QDateTime::fromString(text, "yyyy-MM-dd");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	date = QDateTime::fromString(text, "MM/dd/yyyy");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	return 0;
}
QString Image::filter(QStringList filters)
{
	QStringList mathematicaltypes = QStringList() << "id" << "width" << "height" << "score" << "mpixels" << "filesize" << "date";
	QStringList types = QStringList() << "rating" << "source" << mathematicaltypes;
	bool invert;
	QString filter, type;
	for (int i = 0; i < filters.count(); ++i)
	{
		invert = false;
		filter = filters.at(i);
		if (filter.startsWith('-'))
		{
			filter = filter.right(filter.length()-1);
			invert = true;
		}
		if (filter.contains(":"))
		{
			type = filter.section(':', 0, 0).toLower();
			filter = filter.section(':', 1).toLower();
			if (!types.contains(type))
			{ return QObject::tr("unknown type \"%1\" (available types: \"%2\")").arg(filter, types.join("\", \"")); }
			if (mathematicaltypes.contains(type))
			{
				int input = 0;
				if (type == "id")		{ input = m_id;										}
				if (type == "width")	{ input = m_size.width();							}
				if (type == "height")	{ input = m_size.height();							}
				if (type == "score")	{ input = m_score;									}
				if (type == "mpixels")	{ input = m_size.width()*m_size.height();			}
				if (type == "filesize")	{ input = m_fileSize;								}
				if (type == "date")		{ input = m_createdAt.toString("yyyyMMdd").toInt();	}

				bool cond;
				if (type == "date")
				{
					if (filter.startsWith("..") || filter.startsWith("<="))
					{ cond = input <= toDate(filter.right(filter.size()-2)); }
					else if (filter.endsWith(".."))
					{ cond = input >= toDate(filter.left(filter.size()-2)); }
					else if (filter.startsWith(">="))
					{ cond = input >= toDate(filter.right(filter.size()-2)); }
					else if (filter.startsWith("<"))
					{ cond = input < toDate(filter.right(filter.size()-1)); }
					else if (filter.startsWith(">"))
					{ cond = input > toDate(filter.right(filter.size()-1)); }
					else if (filter.contains(".."))
					{ cond = input >= toDate(filter.left(filter.indexOf(".."))) && input <= toDate(filter.right(filter.size()-filter.indexOf("..")-2));	}
					else
					{ cond = input == toDate(filter); }
				}
				else
				{
					if (filter.startsWith("..") || filter.startsWith("<="))
					{ cond = input <= filter.right(filter.size()-2).toInt(); }
					else if (filter.endsWith(".."))
					{ cond = input >= filter.left(filter.size()-2).toInt(); }
					else if (filter.startsWith(">="))
					{ cond = input >= filter.right(filter.size()-2).toInt(); }
					else if (filter.startsWith("<"))
					{ cond = input < filter.right(filter.size()-1).toInt(); }
					else if (filter.startsWith(">"))
					{ cond = input > filter.right(filter.size()-1).toInt(); }
					else if (filter.contains(".."))
					{ cond = input >= filter.left(filter.indexOf("..")).toInt() && input <= filter.right(filter.size()-filter.indexOf("..")-2).toInt();	}
					else
					{ cond = input == filter.toInt(); }
				}

				if (!cond && !invert)
				{ return QObject::tr("image's %1 does not match").arg(type); }
				if (cond && invert)
				{ return QObject::tr("image's %1 match").arg(type); }
			}
			else
			{
				if (type == "rating")
				{
					bool cond = m_rating.toLower().startsWith(filter.left(1));
					if (!cond && !invert)
					{ return QObject::tr("image is not \"%1\"").arg(filter); }
					if (cond && invert)
					{ return QObject::tr("image is \"%1\"").arg(filter); }
				}
				else if (type == "source")
				{
					QRegExp rx = QRegExp(filter+"*", Qt::CaseInsensitive, QRegExp::Wildcard);
					bool cond = rx.exactMatch(m_source);
					if (!cond && !invert)
					{ return QObject::tr("image's source does not starts with \"%1\"").arg(filter); }
					if (cond && invert)
					{ return QObject::tr("image's source starts with \"%1\"").arg(filter); }
				}
			}
		}
		else if (!filter.isEmpty())
		{
			bool cond = false;
			for (int t = 0; t < m_tags.count(); t++)
			{
				if (m_tags[t].text().toLower() == filter.toLower())
				{ cond = true; break; }
			}
			if (!cond && !invert)
			{ return QObject::tr("image does not contains \"%1\"").arg(filter); }
			if (cond && invert)
			{ return QObject::tr("image contains \"%1\"").arg(filter); }
		}
	}
	return QString();
}



QString analyse(QStringList tokens, QString text, QStringList tags)
{
	QString ret = text;
	QRegExp reg = QRegExp("\\<([^>]+)\\>");
	int pos = 0;
	while ((pos = reg.indexIn(text, pos)) != -1)
	{
		QString cap = reg.cap(1);
		if (!cap.isEmpty())
		{
			cap += QString(">").repeated(cap.count('<')-cap.count('>'));
			ret.replace("<"+cap+">", analyse(tokens, cap, tags));
		}
		pos += reg.matchedLength()+cap.count('<')-cap.count('>');
	}
	QString r = ret;
	for (int i = 0; i < tokens.size(); ++i)
	{ r.replace(QRegExp("%"+tokens.at(i)+"(?::([0-9]+))?%"), ""); }
	reg = QRegExp("\"([^\"]+)\"");
	pos = 0;
	while ((pos = reg.indexIn(text, pos)) != -1)
	{
		if (!reg.cap(1).isEmpty() && tags.contains(reg.cap(1)))
		{ ret.replace(reg.cap(0), reg.cap(1)); }
		pos += reg.matchedLength();
	}
	return r.contains("%") || ret.contains("\"") ? "" : ret;
}

typedef QPair<QString,QString> QStrP;
QList<QStrP> getReplace(QString setting, QMap<QString,QStringList> details, QSettings *settings)
{
	QList<QStrP> ret;
	QString first = "", second = "";
	int limit = settings->value(setting+"_multiple_limit", 1).toInt();
	QString separator = settings->value(setting+"_sep", " ").toString();
	if (details[setting+"s"].size() > limit)
	{
		QString whatToDo = settings->value(setting+"_multiple", "replaceAll").toString();
		if (whatToDo == "keepAll")
		{ first = details[setting+"s"].join(separator); }
		else if (whatToDo == "keepN")
		{
			int keepN = settings->value(setting+"_multiple_keepN", 1).toInt();
			first = QStringList(details[setting+"s"].mid(0, qMin(1, keepN))).join(separator);
		}
		else if (whatToDo == "keepNThenAdd")
		{
			int keepN = settings->value(setting+"_multiple_keepNThenAdd_keep", 1).toInt();
			QString thenAdd = settings->value(setting+"_multiple_keepNThenAdd_add", " (+ %count%)").toString();
			thenAdd.replace("%total%", QString::number(details[setting+"s"].size()));
			thenAdd.replace("%count%", QString::number(details[setting+"s"].size() - keepN));
			first = QStringList(details[setting+"s"].mid(0, qMin(1, keepN))).join(separator) + thenAdd;
		}
		else
		{ first = settings->value(setting+"_value").toString(); }
	}
	else
	{ first = first = details[setting+"s"].join(separator); }
	second = settings->value(setting+"_empty").toString();

	ret.append(QStrP(first, second));
	ret.append(QStrP(first.isEmpty() ? first : (first+"2"), second.isEmpty() ? second : (second+"2")));
	return ret;
}

QString cutLength(QString res, QString filename, QString pth, QString key, bool cut)
{
	if (cut && !filename.right(filename.length() - filename.indexOf(key)).contains("/"))
	{ return res.left(259 - pth.length() - 1 - filename.length()).trimmed(); }
	return res.trimmed();
}

/**
 * Return the filename of the image according to the user's settings.
 * @param fn The user's filename.
 * @param pth The user's root save path.
 * @param counter Current image count (used for batch downloads).
 * @param complex Whether the filename is complex or not (contains conditionals).
 * @param simple True to force using the fn and pth parameters.
 * @return The filename of the image, with any token replaced.
 */
QStringList Image::path(QString fn, QString pth, int counter, bool complex, bool simple, bool maxlength)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList ignore = loadIgnored(), remove = settings.value("ignoredtags").toString().split(' ', QString::SkipEmptyParts);

	if (!simple)
	{
		if (!m_filename.isEmpty())
		{ fn = m_filename; }
		if (!m_folder.isEmpty())
		{ pth = m_folder; }

		settings.beginGroup("Save");
		if (fn.isEmpty())
		{ fn = settings.value("filename").toString(); }
		if (pth.isEmpty())
		{ pth = settings.value("path").toString(); }
	}

	QStringList copyrights;
	QString cop;
	bool found;
	QMap<QString,QStringList> custom = QMap<QString,QStringList>(), scustom = getCustoms();
	QMap<QString,QStringList> details;
	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (int i = 0; i < m_tags.size(); ++i)
	{
		QString t = m_tags[i].text();
		bool removed = false;
		for (int j = 0; j < remove.size(); ++j)
		{
			reg.setPattern(remove.at(j));
			if (reg.exactMatch(t))
			{ removed = true; }
		}
		if (!removed)
		{
			for (int r = 0; r < scustom.size(); ++r)
			{
				if (!custom.contains(scustom.keys().at(r)))
				{ custom.insert(scustom.keys().at(r), QStringList()); }
				if (scustom.values().at(r).contains(t))
				{ custom[scustom.keys().at(r)].append(t); }
			}
			details["allos"].append(t);
			details[ignore.contains(m_tags[i].text(), Qt::CaseInsensitive) ? "generals" : m_tags[i].type()+"s"].append(t);
			details["alls"].append(t);
		}
	}
	if (settings.value("copyright_useshorter", true).toBool())
	{
		for (QString cop : details["copyrights"])
		{
			found = false;
			for (int r = 0; r < copyrights.size(); ++r)
			{
				if (copyrights.at(r).left(cop.size()) == cop.left(copyrights.at(r).size()))
				{
					if (cop.size() < copyrights.at(r).size())
					{ copyrights[r] = cop; }
					found = true;
				}
			}
			if (!found)
			{ copyrights.append(cop); }
		}
	}
	else
	{ copyrights = details["copyrights"]; }

	QString ext = getExtension(m_url);

	QMap<QString,QStrP> replaces = QMap<QString,QStrP>();
	replaces.insert("ext", QStrP(ext, "jpg"));
    replaces.insert("filename", QStrP(m_url.section('/', -1).section('.', 0, -2), ""));
    replaces.insert("website", QStrP(m_parentSite->url(), ""));
	replaces.insert("websitename", QStrP(m_parentSite->name(), ""));
	replaces.insert("md5", QStrP(m_md5, ""));
	replaces.insert("date", QStrP(m_createdAt.toString(tr("dd-MM-yyyy HH.mm")), ""));
	replaces.insert("id", QStrP(QString::number(m_id), "0"));
	for (int i = 0; i < m_search.size(); ++i)
	{ replaces.insert("search_"+QString::number(i+1), QStrP(m_search[i], "")); }
	replaces.insert("search", QStrP(m_search.join(settings.value("separator").toString()), ""));
	replaces.insert("rating", QStrP(m_rating, "unknown"));
	replaces.insert("score", QStrP(QString::number(m_score), ""));
	replaces.insert("height", QStrP(QString::number(m_size.height()), "0"));
	replaces.insert("width", QStrP(QString::number(m_size.width()), "0"));
	replaces.insert("general", QStrP(details["generals"].join(settings.value("separator").toString()), ""));
	replaces.insert("allo", QStrP(details["allos"].join(" "), ""));
	replaces.insert("all", QStrP(details["alls"].join(" "), ""));
	for (int i = 0; i < custom.size(); ++i)
	{ replaces.insert(custom.keys().at(i), QStrP(custom.values().at(i).join(settings.value("separator").toString()), "")); }

	// Filename
	QString filename = fn;
	QMap<QString,QString> filenames = getFilenames();
	for (int i = 0; i < filenames.size(); ++i)
	{
		QString cond = filenames.keys().at(i);
		if (cond.startsWith("%") && cond.endsWith("%"))
		{
			int contains = false;
			if (replaces.contains(cond.mid(1, cond.size()-2)))
			{
				contains = true;
				break;
			}
			if (contains)
			{
				if (!replaces[cond.mid(1, cond.size()-2)].first.isEmpty())
				{ filename = filenames.value(cond); }
			}
		}
		else if (details["alls"].contains(cond))
		{ filename = filenames.value(cond); }
	}

	// Remove duplicates in %all%
	QStringList rem = (filename.contains("%artist%") ? details["artists"] : QStringList()) +
		(filename.contains("%copyright%") ? copyrights : QStringList()) +
		(filename.contains("%character%") ? details["characters"] : QStringList()) +
		(filename.contains("%model%") ? details["models"] : QStringList()) +
		(filename.contains("%general%") ? details["generals"] : QStringList());
	QStringList l = details["alls"];
	for (int i = 0; i < rem.size(); ++i)
	{ l.removeAll(rem.at(i)); }
	replaces.insert("all", QStrP(l.join(settings.value("separator").toString()), ""));

	if (filename.startsWith("javascript:"))
	{
		// We remove the "javascript:" part
		filename = filename.right(filename.length() - 11);

		// Variables initialization
		QString inits = "";
		QStringList keys = replaces.keys();
		for (int i = 0; i < replaces.size(); ++i)
		{
			QString key = keys.at(i);
			QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
			if (key != "allo")
			{ res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed(); }
			if (!settings.value("replaceblanks", false).toBool())
			{ res.replace("_", " "); }

			inits += "var " + key + " = \"" + res + "\";\r\n";
		}

		// Script execution
		QScriptEngine engine;
		QScriptValue result = engine.evaluate(QScriptProgram(inits + filename));
		if (result.isError())
		{
			error(0, tr("Erreur d'évaluation du Javascript :<br/>") + result.toString());
			return QStringList();
		}

		filename = result.toString();
	}
	else
	{
		// We get path and remove useless slashes from filename
		pth.replace("\\", "/");
		filename.replace("\\", "/");
		if (filename.left(1) == "/")	{ filename = filename.right(filename.length()-1);	}
		if (pth.right(1) == "/")		{ pth = pth.left(pth.length()-1);					}

		// Conditionals
		if (complex)
		{
			QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << "date" << "date:([^%]+)" << "count(:\\d+)?(:\\d+)?" << "search_(\\d+)" << "score" << custom.keys();
			filename = analyse(tokens, filename, details["allos"]);
		}

		// Complex expressions using regexes
		QRegExp rxdate("%date:([^%]+)%");
		rxdate.setMinimal(true);
		int p = 0;
		while ((p = rxdate.indexIn(filename, p)) != -1)
		{ filename.replace(rxdate.cap(0), m_createdAt.toString(rxdate.cap(1))); }
		QRegExp rxcounter("%count(:\\d+)?(:\\d+)?%");
		rxcounter.setMinimal(true);
		p = 0;
		while ((p = rxcounter.indexIn(filename, p)) != -1)
		{ filename.replace(rxcounter.cap(0), rxcounter.captureCount() > 0 ? QString::number(counter, 'f', rxcounter.cap(1).toInt()) : QString::number(counter)); }
		QRegExp rxsize("%([^:]+):([^%]+)%");
		rxsize.setMinimal(true);
		p = 0;
		while ((p = rxsize.indexIn(filename, p)) != -1)
		{
			QString key = rxsize.cap(1);
			int length = rxsize.cap(2).toInt();
			if (replaces.contains(key) && length >= 0)
			{
				QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
				filename.replace(rxsize.cap(0), res.left(length));
			}
			else
			{ p += rxsize.matchedLength(); }
		}

		// We replace everything
		QStringList keys = replaces.keys();
		for (int i = 0; i < replaces.size(); ++i)
		{
			QString key = keys.at(i);
			QString res = replaces[key].first.isEmpty() ? replaces[key].second : replaces[key].first;
			if (key != "allo")
			{ res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed(); }
			if (!settings.value("replaceblanks", false).toBool())
			{ res.replace("_", " "); }

			// We only cut the name if it is not a folder
			filename.replace("%"+key+"%", cutLength(res, filename, pth, "%"+key+"%", maxlength && complex));
		}
	}

	QStringList fns = QStringList() << filename;
	QStringList keys = QStringList() << "artist" << "copyright" << "character" << "model";

	for (QString key : keys)
	{
		QList<QStrP> replaces = getReplace(key, details, &settings);
		int cnt = fns.count();
		for (int i = 0; i < cnt; ++i)
		{
			if (fns[i].contains("%"+key+"%"))
			{
				for (int j = 0; j < replaces.count(); ++j)
				{
					QString res = replaces[j].first.isEmpty() ? replaces[j].second : replaces[j].first;
					res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
					if (!settings.value("replaceblanks", false).toBool())
					{ res.replace("_", " "); }

					QString filename = QString(fns[i]);
					filename = filename.replace("%"+key+"%", cutLength(res, filename, pth, "%"+key+"%", maxlength && complex));

					if (j < replaces.count() - 1)
						fns.append(filename);
					else
						fns[i] = filename;
				}
			}
		}
	}

	int cnt = fns.count();
	for (int i = 0; i < cnt; ++i)
	{
		QString filename = fns[i];

		// Trim directory names
		filename = filename.trimmed();
		filename.replace(QRegExp(" */ *"), "/");

		// We remove empty directory names
		while (filename.indexOf("//") >= 0)
		{ filename.replace("//", "/"); }

		// Max filename size option
		if (maxlength)
		{
			if (complex && filename.length() > settings.value("limit").toInt() && settings.value("limit").toInt() > 0)
			{ filename = filename.left(filename.length()-ext.length()-1).left(settings.value("limit").toInt()-ext.length()-1) + filename.right(ext.length()+1); }
		}

		fns[i] = QDir::toNativeSeparators(filename);
	}

	return fns;
}

void Image::loadImage()
{
	m_loadImage = m_parentSite->get(m_url, m_parent, "image", this);
	m_loadImage->setParent(this);
	//m_timer.start();

	connect(m_loadImage, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgressImageS(qint64, qint64)));
	connect(m_loadImage, SIGNAL(finished()), this, SLOT(finishedImageS()));
}
void Image::finishedImageS()
{
	QUrl redir = m_loadImage->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = redir.toString();
		loadImage();
		return;
	}

	bool sampleFallback = m_settings->value("Save/samplefallback", true).toBool();
	QString ext = getExtension(m_url);
	if (m_loadImage->error() == QNetworkReply::ContentNotFoundError
			&& (ext != "webm" || (sampleFallback && !m_sampleUrl.isEmpty())))
	{
		if (ext == "webm")
		{
			setUrl(m_sampleUrl.toString());
			log(tr("Image non trouvée. Nouvel essai avec son sample..."));
		}
		else
		{
			QMap<QString,QString> nextext;
			nextext["jpg"] = "png";
			nextext["png"] = "gif";
			nextext["gif"] = "jpeg";
			nextext["jpeg"] = "webm";
			setUrl(m_url.section('.', 0, -2)+"."+nextext[ext]);
			log(tr("Image non trouvée. Nouvel essai avec l'extension %1...").arg(nextext[ext]));
		}
		loadImage();
		return;
	}

	m_data = m_loadImage->readAll();

	emit finishedImage(this);
}
void Image::downloadProgressImageS(qint64 v1, qint64 v2)
{
	if (m_loadImage != NULL && v2 > 0/* && (v1 == v2 || m_timer.elapsed() > 500)*/)
	{
		//m_timer.restart();
		emit downloadProgressImage(this, v1, v2);
	}
}
void Image::abortImage()
{
	if (m_loadImage != NULL)
	{
		if (m_loadImage->isRunning())
		{ m_loadImage->abort(); }
	}
}

/**
 * Try to guess the size of the image in pixels for sorting.
 * @return The guessed number of pixels in the image.
 */
int Image::value()
{
	if (!m_size.isEmpty())
        return m_size.width() * m_size.height();

    QStringList tags;
    for (int t = 0; t < m_tags.size(); t++)
    { tags.append(m_tags[t].text().toLower()); }

    if (tags.contains("incredibly_absurdres"))	{ return 10000 * 10000; }
    else if (tags.contains("absurdres"))		{ return 3200 * 2400; }
    else if (tags.contains("highres"))			{ return 1600 * 1200; }
    else if (tags.contains("lowres"))			{ return 500 * 500; }

    return 1200 * 900;
}

/**
 * Checks whether an image contains blacklisted tags.
 * @param blacklistedtags The list of blacklisted tags.
 * @return The blacklisted tags found in the image (empty list if none).
 */
QStringList Image::blacklisted(QStringList blacklistedtags)
{
	QStringList detected;
	QRegExp reg;
	reg.setCaseSensitivity(Qt::CaseInsensitive);
	reg.setPatternSyntax(QRegExp::Wildcard);
	for (QString tag : blacklistedtags)
	{
		for (int t = 0; t < m_tags.count(); ++t)
		{
			if (!tag.trimmed().isEmpty())
			{
				reg.setPattern(tag.trimmed());
				if (reg.exactMatch(m_tags[t].text()))
				{ detected.append(m_tags[t].text()); }
			}
		}
	}
	return detected;
}

QStringList Image::stylishedTags(QStringList ignored)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));

	QStringList tlist = QStringList() << "blacklisteds" << "ignoreds" << "artists" << "circles" << "copyrights" << "characters" << "models" << "generals" << "favorites";
	QStringList defaults = QStringList() << "000000" << "999999" << "aa0000" << "55bbff" << "aa00aa" << "00aa00" << "0000ee" << "000000" << "ffc0cb";
	QMap<QString,QString> styles;
	for (int i = 0; i < tlist.size(); ++i)
	{
		QFont font;
		font.fromString(settings.value("Coloring/Fonts/"+tlist.at(i)).toString());
		styles[tlist.at(i)] = "color:"+settings.value("Coloring/Colors/"+tlist.at(i), "#"+defaults.at(i)).toString()+"; "+qfonttocss(font);
	}
	QStringList t;
	for (int i = 0; i < m_tags.size(); ++i)
	{
		Tag tag = m_tags.at(i);
		QString type = blacklistedtags.contains(tag.text(), Qt::CaseInsensitive) ? "blacklisteds" : (ignored.contains(tag.text(), Qt::CaseInsensitive) ? "ignored" : tag.type());
		t.append("<a href=\""+tag.text()+"\" style=\""+(styles.contains(type+"s") ? styles[type+"s"] : styles["generals"])+"\">"+tag.text()+"</a>");
	}
	t.sort();
	return t;
}


QString			Image::url()			{ return m_url;				}
QString			Image::author()			{ return m_author;			}
QString			Image::status()			{ return m_status;			}
QString			Image::rating()			{ return m_rating;			}
QString			Image::source()			{ return m_source;			}
QString			Image::site()			{ return m_site;			}
QString			Image::filename()		{ return m_filename;		}
QString			Image::folder()			{ return m_folder;			}
QList<Tag>		Image::tags()			{ return m_tags;			}
QList<Pool*>	Image::pools()			{ return m_pools;			}
int				Image::id()				{ return m_id;				}
int				Image::score()			{ return m_score;			}
int				Image::parentId()		{ return m_parentId;		}
int				Image::fileSize()		{ return m_fileSize;		}
int				Image::width()			{ return m_size.width();	}
int				Image::height()			{ return m_size.height();	}
int				Image::authorId()		{ return m_authorId;		}
QDateTime		Image::createdAt()		{ return m_createdAt;		}
bool			Image::hasChildren()	{ return m_hasChildren;		}
bool			Image::hasNote()		{ return m_hasNote;			}
bool			Image::hasComments()	{ return m_hasComments;		}
bool			Image::hasScore()		{ return m_hasScore;		}
QUrl			Image::fileUrl()		{ return m_fileUrl;			}
QUrl			Image::sampleUrl()		{ return m_sampleUrl;		}
QUrl			Image::previewUrl()		{ return m_previewUrl;		}
QUrl			Image::pageUrl()		{ return m_pageUrl;			}
QSize			Image::size()			{ return m_size;			}
QPixmap			Image::previewImage()	{ return m_imagePreview;	}
Page			*Image::page()			{ return m_parent;			}
QByteArray		Image::data()			{ return m_data;			}
QNetworkReply	*Image::imageReply()	{ return m_loadImage;		}
QNetworkReply	*Image::tagsReply()		{ return m_loadDetails;		}
QSettings		*Image::settings()		{ return m_settings;		}

void	Image::setUrl(QString u)
{
	m_fileSize = 0;
	emit urlChanged(m_url, u);
	m_url = u;
}
void	Image::setFileSize(int s)		{ m_fileSize = s;			}
void	Image::setData(QByteArray d)
{
	m_data = d;
	if (m_md5.isEmpty())
	{ m_md5 = QCryptographicHash::hash(m_data, QCryptographicHash::Md5).toHex(); }
}
void Image::setSavePath(QString savePath)
{
	m_savePath = savePath;
}

QString Image::md5()
{
    // If we know the path to the image but not its md5, we calculate it first
	if (m_md5 == "" && m_savePath != "")
	{
		QCryptographicHash hash(QCryptographicHash::Md5);
		QFile f(m_savePath);
		f.open(QFile::ReadOnly);
		while (!f.atEnd())
			hash.addData(f.read(8192));
		f.close();
		m_md5 = hash.result().toHex();
	}

	return m_md5;
}

bool Image::hasTag(QString tag)
{
	tag = tag.trimmed();
	for (Tag t : m_tags)
		if (t.text().trimmed() == tag)
			return true;
	return false;
}
bool Image::hasTag(QStringList tags)
{
	for (QString tag : tags)
		if (this->hasTag(tag))
			return true;
	return false;
}
