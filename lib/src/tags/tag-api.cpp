#include "tags/tag-api.h"
#include <QDomDocument>
#include <QRegularExpression>
#include "functions.h"
#include "logger.h"
#include "models/api.h"
#include "models/site.h"
#include "vendor/json.h"


TagApi::TagApi(Profile *profile, Site *site, Api *api, int page, int limit, QObject *parent)
	: QObject(parent), m_profile(profile), m_site(site), m_api(api), m_page(page), m_limit(limit), m_reply(Q_NULLPTR)
{
	QString url = m_api->value("Urls/TagApi");

	// Basic information
	page = page - 1 + m_api->value("FirstPage").toInt();
	url.replace("{page}", QString::number(page));
	url.replace("{limit}", QString::number(limit));

	// Login information
	QString pseudo = m_site->username();
	QString password = m_site->password();
	bool hasLoginString = m_api->contains("Urls/Login") && (!pseudo.isEmpty() || !password.isEmpty());
	url.replace("{login}", hasLoginString ? m_api->value("Urls/Login") : "");
	url.replace("{pseudo}", pseudo);
	url.replace("{password}", password);

	m_url = m_site->fixUrl(url);
}

TagApi::~TagApi()
{
	if (m_reply != nullptr)
		m_reply->deleteLater();
}

void TagApi::load(bool rateLimit)
{
	m_site->getAsync(rateLimit ? Site::QueryType::Retry : Site::QueryType::List, m_url, [this](QNetworkReply *reply) {
		log(QString("[%1] Loading tags page <a href=\"%2\">%2</a>").arg(m_site->url()).arg(m_url.toString().toHtmlEscaped()), Logger::Info);

		if (m_reply != nullptr)
			m_reply->deleteLater();

		m_reply = reply;
		connect(m_reply, &QNetworkReply::finished, this, &TagApi::parse);
	});
}

void TagApi::abort()
{
	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();
}

void TagApi::parse()
{
	log(QString("[%1] Receiving tags page <a href=\"%2\">%2</a>").arg(m_site->url()).arg(m_reply->url().toString().toHtmlEscaped()), Logger::Info);

	// Check redirection
	QUrl redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty())
	{
		QUrl newUrl = m_site->fixUrl(redirection.toString(), m_url);
		log(QString("[%1] Redirecting tags page <a href=\"%2\">%2</a> to <a href=\"%3\">%3</a>").arg(m_site->url()).arg(m_url.toString().toHtmlEscaped()).arg(newUrl.toString().toHtmlEscaped()), Logger::Info);
		m_url = newUrl;
		load();
		return;
	}

	// Initializations
	QString source = m_reply->readAll();
	QString format = m_api->getName();
	m_site->tagDatabase()->loadTypes();
	QMap<int, TagType> tagTypes = m_site->tagDatabase()->tagTypes();
	m_tags.clear();

	if (source.isEmpty())
	{
		if (m_reply->error() != QNetworkReply::OperationCanceledError)
		{ log(QString("[%1] Loading error: %2 (%3)").arg(m_site->url()).arg(m_reply->errorString()).arg(m_reply->error())); }
		emit finishedLoading(this, LoadResult::Error);
		return;
	}

	// Tag variables definitions
	int id;
	QString name;
	int count;
	int typeId;
	QString ttype;

	// XML
	if (format == "Xml")
	{
		// Read source
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(QString("[%1] Error parsing XML file: %2 (%3 - %4).").arg(m_site->url()).arg(errorMsg).arg(errorLine).arg(errorColumn), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}
		QDomElement docElem = doc.documentElement();

		// Read tags
		QDomNodeList nodeList = docElem.elementsByTagName("tag");
		for (int i = 0; i < nodeList.count(); i++)
		{
			QDomNode node = nodeList.at(i);
			QDomNamedNodeMap attr = node.attributes();

			ttype = "";
			if (!node.namedItem("post-count").isNull())
			{
				id = node.namedItem("id").toElement().text().toInt();
				name = node.namedItem("name").toElement().text();
				count = node.namedItem("post-count").toElement().text().toInt();
				typeId = node.namedItem("category").toElement().text().toInt();
			}
			else if (attr.contains("name"))
			{
				id = attr.namedItem("id").toAttr().value().toInt();
				name = attr.namedItem("name").toAttr().value();
				count = attr.namedItem("count").toAttr().value().toInt();
				typeId = attr.namedItem("type").toAttr().value().toInt();
			}
			else
			{
				id = node.namedItem("id").toElement().text().toInt();
				name = node.namedItem("name").toElement().text();
				count = node.namedItem("count").toElement().text().toInt();
				typeId = node.namedItem("type").toElement().text().toInt();
			}

			TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
			m_tags.append(Tag(id, name, tagType, count));
		}
	}

	// JSON
	else if (format == "Json")
	{
		// Read source
		QVariant src = Json::parse(source);
		if (src.isNull())
		{
			log(QString("[%1] Error parsing JSON file: \"%2\"").arg(m_site->url()).arg(source.left(500)), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}
		QMap<QString, QVariant> data = src.toMap();

		// Check for a JSON error message
		if (data.contains("success") && !data["success"].toBool())
		{
			log(QString("[%1] JSON error reply: \"%2\"").arg(m_site->url()).arg(data["reason"].toString()), Logger::Warning);
			emit finishedLoading(this, LoadResult::Error);
			return;
		}

		// Read tags
		QList<QVariant> sourc = src.toList();
		for (const QVariant &el : sourc)
		{
			QMap<QString, QVariant> sc = el.toMap();

			ttype = "";
			if (sc.contains("short_description"))
			{
				id = sc.value("id").toInt();
				name = sc.value("name").toString();
				count = sc.value("images").toInt();
				ttype = sc.value("category").toString();
			}
			else if (sc.contains("post_count"))
			{
				id = sc.value("id").toInt();
				name = sc.value("name").toString();
				count = sc.value("post_count").toInt();
				typeId = sc.value("category").toInt();
			}
			else
			{
				id = sc.value("id").toInt();
				name = sc.value("name").toString();
				count = sc.value("count").toInt();
				typeId = sc.value("type").toInt();
			}

			TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
			m_tags.append(Tag(id, name, tagType, count));
		}
	}

	// Regexes
	else if (format == "Html")
	{
		// Read tags
		QRegularExpression rx(m_site->value("Regex/TagApi"), QRegularExpression::DotMatchesEverythingOption);
		auto matches = rx.globalMatch(source);
		while (matches.hasNext())
		{
			auto match = matches.next();

			// Parse result using the regex
			QMap<QString, QString> d;
			for (const QString &group : rx.namedCaptureGroups())
			{
				if (group.isEmpty())
					continue;

				QString val = match.captured(group);
				if (!val.isEmpty())
				{ d[group] = val.trimmed(); }
			}

			// Map variables
			id = d.contains("id") ? d["id"].toInt() : 0;
			name = d["tag"];
			count = d.contains("count") ? d["count"].toInt() : 0;
			typeId = d.contains("typeId") ? d["typeId"].toInt() : -1;
			ttype = d["type"];

			TagType tagType = !ttype.isEmpty() ? TagType(ttype) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
			m_tags.append(Tag(id, name, tagType, count));
		}
	}

	emit finishedLoading(this, LoadResult::Ok);
}

QList<Tag> TagApi::tags() const
{
	return m_tags;
}
