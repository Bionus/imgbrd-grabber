#include "models/api/javascript-api.h"
#include <QJSEngine>
#include <QJSValueIterator>
#include "functions.h"
#include "logger.h"
#include "mixed-settings.h"
#include "models/page.h"
#include "models/pool.h"
#include "models/site.h"
#include "tags/tag-database.h"


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
	url = site->fixLoginUrl(url);
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
		if (tag.isString())
		{
			ret.append(Tag(tag.toString()));
			continue;
		}
		else if (!tag.isObject())
		{ continue; }

		int id = tag.hasProperty("id") ? tag.property("id").toInt() : 0;
		QString text = tag.property("name").toString();
		QString type = tag.hasProperty("type") ? tag.property("type").toString() : "";
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
	const QJSValue &api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("search").property("parse");
	const QJSValue &results = parseFunction.call(QList<QJSValue>() << source);

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

				const QString &key = it3.name();
				const QJSValue &val = it3.value();

				if (it3.value().isUndefined())
				{ continue; }

				if (key == "tags_obj" || (key == "tags" && val.isArray()))
				{ tags = makeTags(val, site); }
				else
				{ d[key] = val.toString(); }
			}

			if (!d.isEmpty())
			{
				int id = d["id"].toInt();
				QSharedPointer<Image> img = parseImage(parentPage, d, id + first, tags, false);
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


bool JavascriptApi::canLoadDetails() const
{
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::detailsUrl(qulonglong id, const QString &md5, Site *site) const
{
	PageUrl ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support details loading";
		return ret;
	}

	QJSValue result = urlFunction.call(QList<QJSValue>() << QString::number(id) << md5);
	fillUrlObject(result, site, ret);

	return ret;
}

ParsedDetails JavascriptApi::parseDetails(const QString &source, Site *site) const
{
	ParsedDetails ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("details").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QString("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error") && results.property("error").isString())
	{ ret.error = results.property("error").toString(); }
	if (results.hasProperty("tags"))
	{ ret.tags = makeTags(results.property("tags"), site); }
	if (results.hasProperty("imageUrl") && results.property("imageUrl").isString())
	{ ret.imageUrl = results.property("imageUrl").toString(); }
	if (results.hasProperty("createdAt") && results.property("createdAt").isString())
	{ ret.createdAt = qDateTimeFromString(results.property("createdAt").toString()); }

	if (results.hasProperty("pools"))
	{
		QJSValue images = results.property("pools");
		QJSValueIterator it(images);
		while (it.hasNext())
		{
			it.next();

			QJSValue pool = it.value();
			if (!pool.isObject())
				continue;

			int id = pool.hasProperty("id") ? pool.property("id").toInt() : 0;
			QString name = pool.property("name").toString();
			int next = pool.hasProperty("next") ? pool.property("next").toInt() : 0;
			int previous = pool.hasProperty("previous") ? pool.property("previous").toInt() : 0;

			ret.pools.append(Pool(id, name, 0, next, previous));
		}
	}

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

bool JavascriptApi::needAuth() const
{
	QStringList requiredAuths;

	QJSValueIterator it(getJsConst("forcedLimit"));
	while (it.hasNext())
	{
		it.next();
		requiredAuths.append(it.value().toString());
	}

	return !requiredAuths.isEmpty();
}

int JavascriptApi::forcedLimit() const
{ return getJsConst("forcedLimit", 0).toInt(); }
int JavascriptApi::maxLimit() const
{ return getJsConst("maxLimit", 0).toInt(); }

QStringList JavascriptApi::modifiers() const
{
	QStringList ret;
	QJSValue modifiers = getJsConst("modifiers");

	QJSValueIterator it(modifiers);
	while (it.hasNext())
	{
		it.next();
		ret.append(it.value().toString());
	}

	return ret;
}
