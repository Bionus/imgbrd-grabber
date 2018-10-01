#include "models/api/javascript-api.h"
#include <QJSEngine>
#include <QJSValueIterator>
#include <QMap>
// #include <QMutexLocker>
#include "functions.h"
#include "logger.h"
#include "mixed-settings.h"
#include "models/page.h"
#include "models/pool.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-database.h"


QStringList jsToStringList(const QJSValue &val)
{
	QStringList ret;

	const quint32 length = val.property("length").toUInt();
	for (quint32 i = 0; i < length; ++i) {
		ret.append(val.property(i).toString());
	}

	return ret;
}

QString normalize(QString key)
{
	key = key.toLower();
	key[0] = key[0].toUpper();
	return key;
}

JavascriptApi::JavascriptApi(const QJSValue &source, QMutex *jsEngineMutex, const QString &key)
	: Api(normalize(key)), m_source(source), m_key(key), m_engineMutex(jsEngineMutex)
{}


void JavascriptApi::fillUrlObject(const QJSValue &result, Site *site, PageUrl &ret) const
{
	// Script errors and exceptions
	if (result.isError())
	{
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
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
	if (site != nullptr)
	{
		url = site->fixLoginUrl(url);
		url = site->fixUrl(url).toString();
	}

	ret.url = url;
}


PageUrl JavascriptApi::pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
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
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	QJSValue previous = QJSValue(QJSValue::UndefinedValue);
	if (lastPage > 0)
	{
		previous = m_source.engine()->newObject();
		previous.setProperty("page", lastPage);
		previous.setProperty("minId", lastPageMinId);
		previous.setProperty("maxId", lastPageMaxId);
	}

	const QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts << previous);
	fillUrlObject(result, site, ret);

	return ret;
}

QList<Tag> JavascriptApi::makeTags(const QJSValue &tags, Site *site) const
{
	QList<Tag> ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	const quint32 length = tags.property("length").toUInt();
	for (quint32 i = 0; i < length; ++i)
	{
		const QJSValue tag = tags.property(i);
		if (tag.isString())
		{
			ret.append(Tag(tag.toString()));
			continue;
		}
		if (!tag.isObject())
		{ continue; }

		const int id = tag.hasProperty("id") && !tag.property("id").isUndefined() ? tag.property("id").toInt() : 0;
		const QString text = tag.property("name").toString();
		const int count = tag.hasProperty("count") && !tag.property("count").isUndefined() ? tag.property("count").toInt() : 0;

		QString type;
		int typeId = -1;
		if (tag.hasProperty("type") && !tag.property("type").isUndefined())
		{
			if (tag.property("type").isNumber())
			{ typeId = tag.property("type").toInt(); }
			else
			{ type = tag.property("type").toString(); }
		}
		if (tag.hasProperty("typeId") && !tag.property("typeId").isUndefined())
		{ typeId = tag.property("typeId").toInt(); }

		const TagType tagType = !type.isEmpty() ? TagType(type) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType());
		ret.append(Tag(id, text, tagType, count));
	}

	return ret;
}

ParsedPage JavascriptApi::parsePageInternal(const QString &type, Page *parentPage, const QString &source, int first) const
{
	ParsedPage ret;

	// QMutexLocker locker(m_engineMutex);
	Site *site = parentPage->site();
	const QJSValue &api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property(type).property("parse");
	const QJSValue &results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error"))
	{ ret.error = results.property("error").toString(); }

	if (results.hasProperty("tags"))
	{ ret.tags = makeTags(results.property("tags"), site); }

	if (results.hasProperty("images"))
	{
		const QJSValue images = results.property("images");
		const quint32 length = images.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i)
		{
			QList<Tag> tags;

			QMap<QString, QString> d;
			QJSValueIterator it(images.property(i));
			while (it.hasNext())
			{
				it.next();

				const QString &key = it.name();
				const QJSValue &val = it.value();

				if (val.isUndefined())
				{
					log(QStringLiteral("Undefined value returned by JS model: %1").arg(key), Logger::Debug);
					continue;
				}

				if (key == QLatin1String("tags_obj") || (key == QLatin1String("tags") && val.isArray()))
				{ tags = makeTags(val, site); }
				else if (val.isArray())
				{ d[key] = jsToStringList(val).join(key == QLatin1String("sources") ? '\n' : ' '); }
				else
				{ d[key] = val.toString(); }
			}

			if (!d.isEmpty())
			{
				const int id = d["id"].toInt();
				QSharedPointer<Image> img = parseImage(parentPage, d, id + first, tags);
				if (!img.isNull())
				{ ret.images.append(img); }
			}
		}
	}

	// Basic properties
	if (results.hasProperty("imageCount") && !results.property("imageCount").isUndefined())
	{ ret.imageCount = results.property("imageCount").toInt(); }
	if (results.hasProperty("pageCount") && !results.property("pageCount").isUndefined())
	{ ret.pageCount = results.property("pageCount").toInt(); }
	if (results.hasProperty("urlNextPage") && results.property("urlNextPage").isString())
	{ ret.urlNextPage = results.property("urlNextPage").toString(); }
	if (results.hasProperty("urlPrevPage") && results.property("urlPrevPage").isString())
	{ ret.urlPrevPage = results.property("urlPrevPage").toString(); }
	if (results.hasProperty("wiki") && results.property("wiki").isString())
	{
		ret.wiki = results.property("wiki").toString();
		ret.wiki = ret.wiki.replace("href=\"/", "href=\"" + site->baseUrl() + "/");
	}

	return ret;
}

ParsedPage JavascriptApi::parsePage(Page *parentPage, const QString &source, int first) const
{
	return parsePageInternal("search", parentPage, source, first);
}


PageUrl JavascriptApi::galleryUrl(const QString &id, int page, int limit, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("gallery").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support galleries";
		return ret;
	}

	QJSValue query = m_source.engine()->newObject();
	query.setProperty("id", id);
	query.setProperty("page", page);

	QJSValue opts = m_source.engine()->newObject();
	opts.setProperty("limit", limit);
	opts.setProperty("baseUrl", site->baseUrl());

	const QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts);
	fillUrlObject(result, site, ret);

	return ret;
}

ParsedPage JavascriptApi::parseGallery(Page *parentPage, const QString &source, int first) const
{
	return parsePageInternal("gallery", parentPage, source, first);
}


bool JavascriptApi::canLoadTags() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tags").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::tagsUrl(int page, int limit, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
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
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	const QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts);
	fillUrlObject(result, site, ret);

	return ret;
}

ParsedTags JavascriptApi::parseTags(const QString &source, Site *site) const
{
	ParsedTags ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("tags").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
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
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::detailsUrl(qulonglong id, const QString &md5, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support details loading";
		return ret;
	}

	const QJSValue result = urlFunction.call(QList<QJSValue>() << QString::number(id) << md5);
	fillUrlObject(result, site, ret);

	return ret;
}

ParsedDetails JavascriptApi::parseDetails(const QString &source, Site *site) const
{
	ParsedDetails ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("details").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
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
		const QJSValue pools = results.property("pools");
		const quint32 length = pools.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i)
		{
			const QJSValue pool = pools.property(i);
			if (!pool.isObject())
				continue;

			const int id = pool.hasProperty("id") ? pool.property("id").toInt() : 0;
			const QString name = pool.property("name").toString();
			const int next = pool.hasProperty("next") ? pool.property("next").toInt() : 0;
			const int previous = pool.hasProperty("previous") ? pool.property("previous").toInt() : 0;

			ret.pools.append(Pool(id, name, 0, next, previous));
		}
	}

	return ret;
}


bool JavascriptApi::canLoadCheck() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("check").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::checkUrl() const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("check").property("url");
	if (urlFunction.isUndefined())
	{
		ret.error = "This API does not support checking";
		return ret;
	}

	const QJSValue result = urlFunction.call();
	fillUrlObject(result, nullptr, ret);

	return ret;
}

ParsedCheck JavascriptApi::parseCheck(const QString &source) const
{
	ParsedCheck ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("check").property("parse");
	QJSValue result = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (result.isError())
	{
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.ok = false;
		return ret;
	}

	ret.ok = result.toBool();

	return ret;
}


QJSValue JavascriptApi::getJsConst(const QString &key, const QJSValue &def) const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	if (api.hasProperty(key))
	{ return api.property(key); }
	if (m_source.hasProperty(key))
	{ return m_source.property(key); }
	return def;
}

bool JavascriptApi::needAuth() const
{
	QStringList requiredAuths = jsToStringList(getJsConst("forcedLimit"));
	return !requiredAuths.isEmpty();
}

int JavascriptApi::forcedLimit() const
{ return getJsConst("forcedLimit", 0).toInt(); }
int JavascriptApi::maxLimit() const
{ return getJsConst("maxLimit", 0).toInt(); }

QStringList JavascriptApi::modifiers() const
{ return jsToStringList(getJsConst("modifiers")); }
QStringList JavascriptApi::forcedTokens() const
{ return jsToStringList(getJsConst("forcedTokens")); }
