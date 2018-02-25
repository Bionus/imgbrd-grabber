#include "models/api/javascript-api.h"
#include <QJSValueIterator>
#include "logger.h"
#include "models/page.h"
#include "models/site.h"


QString normalize(QString key)
{
	key = key.toLower();
	key[0] = key[0].toUpper();
	return key;
}

JavascriptApi::JavascriptApi(const QMap<QString, QString> &data, const QJSValue &source, const QString &key)
	: Api(normalize(key), data), m_source(source), m_key(key)
{}


void JavascriptApi::fillUrlObject(const QJSValue &result, Site *site, PageUrl &ret) const
{
	// Script errors and exceptions
	if (result.isError())
	{
		QString err = QString("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return;
	}

	// Parse result
	QString url;
	if (result.isObject())
	{
		if (result.hasProperty("error"))
		{
			ret.error = result.property("error").toString();
			return;
		}

		url = result.property("url").toString();
	}
	else
	{ url = result.toString(); }

	// Site-ize url
	url = site->fixLoginUrl(url, this->value("Urls/Login"));
	url = site->fixUrl(url).toString();

	ret.url = url;
}


PageUrl JavascriptApi::pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const
{
	PageUrl ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("search").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support search";
		return ret;
	}

	QJSValue query = m_source.engine()->newObject();
	query.setProperty("search", search);
	query.setProperty("page", page);

	QJSValue opts = m_source.engine()->newObject();
	opts.setProperty("limit", limit);
	QJSValue auth = m_source.engine()->newObject();
	MixedSettings *settings = site->settings();
	settings->beginGroup("auth");
	for (const QString &key : settings->childKeys())
	{
		QString value = settings->value(key).toString();
		if (key == "pseudo" && !auth.hasProperty("login"))
		{ auth.setProperty("login", value); }
		if (key == "password" && !auth.hasProperty("password_hash"))
		{ auth.setProperty("password_hash", value); }
		auth.setProperty(key, value);
	}
	settings->endGroup();
	opts.setProperty("auth", auth);

	QJSValue previous = QJSValue(QJSValue::UndefinedValue);
	if (lastPage > 0)
	{
		previous = m_source.engine()->newObject();
		previous.setProperty("page", lastPage);
		previous.setProperty("minId", lastPageMinId);
		previous.setProperty("maxId", lastPageMaxId);
	}

	QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts << previous);
	fillUrlObject(result, site, ret);

	return ret;
}

QList<Tag> JavascriptApi::makeTags(const QJSValue &tags, Site *site) const
{
	QList<Tag> ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	QJSValueIterator it(tags);
	while (it.hasNext())
	{
		it.next();
		QJSValue tag = it.value();

		int id = tag.hasProperty("id") ? tag.property("id").toInt() : 0;
		QString text = tag.property("name").toString();
		QString type = tag.hasProperty("type") ? tag.property("type").toString() : "unknown";
		int typeId = tag.hasProperty("typeId") ? tag.property("typeId").toInt() : -1;
		int count = tag.hasProperty("count") ? tag.property("count").toInt() : 0;

		TagType tagType = !type.isEmpty() ? TagType(type) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType("unknown"));
		ret.append(Tag(id, text, tagType, count));
	}

	return ret;
}

ParsedPage JavascriptApi::parsePage(Page *parentPage, const QString &source, int first, int limit) const
{
	Q_UNUSED(limit);

	ParsedPage ret;

	Site *site = parentPage->site();
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("search").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QString("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error"))
	{ ret.error = results.property("error").toString(); }

	if (results.hasProperty("tags"))
	{ ret.tags = makeTags(results.property("tags"), site); }

	if (results.hasProperty("images"))
	{
		QJSValue images = results.property("images");
		QJSValueIterator it(images);
		while (it.hasNext())
		{
			it.next();

			QList<Tag> tags;

			QMap<QString, QString> d;
			QJSValueIterator it3(it.value());
			while (it3.hasNext())
			{
				it3.next();

				QString key = it3.name();
				if (key == "tags_obj")
				{ tags = makeTags(it3.value(), site); }
				else
				{ d[key] = it3.value().toString(); }
			}

			if (!d.isEmpty())
			{
				int id = d["id"].toInt();
				QSharedPointer<Image> img = parseImage(parentPage, d, id + first, tags);
				if (!img.isNull())
				{ ret.images.append(img); }
			}
		}
	}

	// Basic properties
	if (results.hasProperty("imageCount"))
	{ ret.imageCount = results.property("imageCount").toInt(); }
	if (results.hasProperty("pageCount"))
	{ ret.pageCount = results.property("pageCount").toInt(); }
	if (results.hasProperty("urlNextPage"))
	{ ret.urlNextPage = results.property("urlNextPage").toString(); }
	if (results.hasProperty("urlPrevPage"))
	{ ret.urlPrevPage = results.property("urlPrevPage").toString(); }
	if (results.hasProperty("wiki"))
	{ ret.wiki = results.property("wiki").toString(); }

	return ret;
}


bool JavascriptApi::canLoadTags() const
{
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tags").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::tagsUrl(int page, int limit, Site *site) const
{
	PageUrl ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tags").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support tag loading";
		return ret;
	}

	QJSValue query = m_source.engine()->newObject();
	query.setProperty("page", page);

	QJSValue opts = m_source.engine()->newObject();
	opts.setProperty("limit", limit);
	QJSValue auth = m_source.engine()->newObject();
	MixedSettings *settings = site->settings();
	settings->beginGroup("auth");
	for (const QString &key : settings->childKeys())
	{
		QString value = settings->value(key).toString();
		if (key == "pseudo" && !auth.hasProperty("login"))
		{ auth.setProperty("login", value); }
		if (key == "password" && !auth.hasProperty("password_hash"))
		{ auth.setProperty("password_hash", value); }
		auth.setProperty(key, value);
	}
	settings->endGroup();
	opts.setProperty("auth", auth);

	QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts);
	fillUrlObject(result, site, ret);

	return ret;
}

ParsedTags JavascriptApi::parseTags(const QString &source, Site *site) const
{
	ParsedTags ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("tags").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QString("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error"))
	{ ret.error = results.property("error").toString(); }

	if (results.hasProperty("tags"))
	{ ret.tags = makeTags(results.property("tags"), site); }

	return ret;
}


QJSValue JavascriptApi::getJsConst(const QString &key, const QJSValue &def) const
{
	QJSValue api = m_source.property("apis").property(m_key);
	if (api.hasProperty(key))
	{ return api.property(key); }
	if (m_source.hasProperty(key))
	{ return m_source.property(key); }
	return def;
}

int JavascriptApi::forcedLimit() const
{ return getJsConst("forcedLimit", 0).toInt(); }
int JavascriptApi::maxLimit() const
{ return getJsConst("maxLimit", 0).toInt(); }
