#include "models/api/javascript-api.h"
#include <QJSEngine>
#include <QJSValueIterator>
#include <QMap>
// #include <QMutexLocker>
#include "functions.h"
#include "js-helpers.h"
#include "logger.h"
#include "mixed-settings.h"
#include "models/image.h"
#include "models/page.h"
#include "models/page-information.h"
#include "models/pool.h"
#include "models/site.h"
#include "tags/tag.h"
#include "tags/tag-database.h"
#include "tags/tag-type-with-id.h"


QString normalize(QString key)
{
	key = key.toLower();
	key[0] = key[0].toUpper();
	return key;
}

JavascriptApi::JavascriptApi(QJSEngine *engine, const QJSValue &source, QMutex *jsEngineMutex, const QString &key)
	: Api(normalize(key)), m_engine(engine), m_source(source), m_key(key), m_engineMutex(jsEngineMutex)
{}


void JavascriptApi::fillUrlObject(const QJSValue &result, Site *site, PageUrl &ret) const
{
	// Script errors and exceptions
	if (result.isError()) {
		const QString err = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.error = err;
		log(err, Logger::Error);
		return;
	}

	// Parse result
	QString url;
	if (result.isObject()) {
		if (result.hasProperty("error")) {
			ret.error = result.property("error").toString();
			return;
		}

		url = result.property("url").toString();

		if (result.hasProperty("headers")) {
			const QJSValue headers = result.property("headers");
			QJSValueIterator headersIt(headers);
			while (headersIt.hasNext()) {
				headersIt.next();
				ret.headers[headersIt.name()] = headersIt.value().toString();
			}
		}
	} else {
		url = result.toString();
	}

	// Site-ize url
	if (site != nullptr) {
		url = site->fixLoginUrl(url);
		url = site->fixUrl(url).toString();
	}

	ret.url = url;
}


QJSValue buildParsedSearchTag(QJSEngine *engine, const QString &name, int id)
{
	QJSValue ret = engine->newObject();
	ret.setProperty("name", name);
	if (id > 0) {
		ret.setProperty("id", id);
	}
	return ret;
}
QJSValue buildParsedSearchOperator(QJSEngine *engine, const QString &op, const QJSValue &left, const QJSValue &right)
{
	QJSValue ret = engine->newObject();
	ret.setProperty("operator", op);
	ret.setProperty("left", left);
	ret.setProperty("right", right);
	return ret;
}

PageUrl JavascriptApi::pageUrl(const QString &search, int page, int limit, const PageInformation &lastPage, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("search").property("url");
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support search";
		return ret;
	}

	// Parse query if necessary
	QJSValue parsedSearch;
	const bool parseInput = getJsConst("search.parseInput", false).toBool();
	if (parseInput && !search.trimmed().isEmpty()) {
		QStringList operands = search.split(" ", Qt::SkipEmptyParts);
		const auto tagIds = site->tagDatabase()->getTagIds(operands);

		const QString firstTag = operands.takeFirst();
		const auto first = buildParsedSearchTag(m_engine, firstTag, tagIds[firstTag]);

		if (operands.isEmpty()) {
			parsedSearch = first;
		} else {
			const QString secondTag = operands.takeFirst();
			const auto second = buildParsedSearchTag(m_engine, secondTag, tagIds[secondTag]);
			parsedSearch = buildParsedSearchOperator(m_engine, "and", first, second);

			while (!operands.isEmpty()) {
				const QString nextTag = operands.takeFirst();
				const auto next = buildParsedSearchTag(m_engine, nextTag, tagIds[nextTag]);
				parsedSearch = buildParsedSearchOperator(m_engine, "and", parsedSearch, next);
			}
		}
	}

	QJSValue query = m_engine->newObject();
	query.setProperty("search", search);
	query.setProperty("parsedSearch", parsedSearch);
	query.setProperty("page", page);

	QJSValue opts = m_engine->newObject();
	opts.setProperty("limit", limit);
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	QJSValue previous = QJSValue(QJSValue::UndefinedValue);
	if (lastPage.page > 0) {
		previous = m_engine->newObject();
		previous.setProperty("page", lastPage.page);
		previous.setProperty("minIdM1", QString::number(lastPage.minId - 1));
		previous.setProperty("minId", QString::number(lastPage.minId));
		previous.setProperty("maxId", QString::number(lastPage.maxId));
		previous.setProperty("maxIdP1", QString::number(lastPage.maxId + 1));
		previous.setProperty("minDate", lastPage.minDate);
		previous.setProperty("maxDate", lastPage.maxDate);
	}

	const QJSValue result = urlFunction.call(QList<QJSValue> { query, opts, previous });
	fillUrlObject(result, site, ret);

	return ret;
}

QList<Tag> JavascriptApi::makeTags(const QJSValue &tags, Site *site) const
{
	QList<Tag> ret;
	QMap<int, TagType> tagTypes = site->tagDatabase()->tagTypes();

	const quint32 length = tags.property("length").toUInt();
	for (quint32 i = 0; i < length; ++i) {
		const QJSValue tag = tags.property(i);
		if (tag.isString()) {
			ret.append(Tag(tag.toString()));
			continue;
		}
		if (!tag.isObject()) {
			continue;
		}

		const int id = getPropertyOr(tag, "id", 0);
		const QString text = getPropertyOr(tag, "name", QString());
		const int count = getPropertyOr(tag, "count", 0);

		QString type;
		int typeId = -1;
		if (tag.hasProperty("type") && !tag.property("type").isUndefined()) {
			if (tag.property("type").isNumber()) {
				typeId = tag.property("type").toInt();
			} else {
				type = tag.property("type").toString();
			}
		}
		getProperty(tag, "typeId", typeId);

		QStringList related;
		if (tag.hasProperty("related") && !tag.property("related").isUndefined()) {
			if (tag.property("related").isArray()) {
				related = jsToStringList(tag.property("related"));
			} else {
				related = tag.property("related").toString().split(' ', Qt::SkipEmptyParts);
			}
		}

		const TagType tagType = !type.isEmpty() ? TagType(Tag::GetType(type)) : (tagTypes.contains(typeId) ? tagTypes[typeId] : TagType());
		ret.append(Tag(id, text, tagType, count, related));
	}

	return ret;
}

QSharedPointer<Image> JavascriptApi::makeImage(const QJSValue &raw, Site *site, Page *parentPage, int index, int first) const
{
	QList<Tag> tags;
	QVariantMap data;

	QMap<QString, QString> d;
	QJSValueIterator it(raw);
	while (it.hasNext()) {
		it.next();

		const QString &key = it.name();
		const QJSValue &val = it.value();

		if (val.isUndefined()) {
			log(QStringLiteral("Undefined value returned by JS model: %1").arg(key), Logger::Debug);
			continue;
		}

		if (key == QLatin1String("tags_obj") || (key == QLatin1String("tags") && val.isArray())) {
			tags = makeTags(val, site);
		} else if (key == QLatin1String("tokens")) {
			QJSValueIterator dit(val);
			while (dit.hasNext()) {
				dit.next();
				QVariant dval = dit.value().toVariant();
				if (dit.value().isString() && dval.toString().startsWith("date:")) {
					const QDateTime date = qDateTimeFromString(dval.toString().mid(5));
					if (date.isValid()) {
						dval = date;
					}
				}
				if (dit.value().isString() && dval.toString().startsWith("b64:")) {
					dval = QByteArray::fromBase64(dval.toString().mid(4).toLatin1()).toHex();
				}
				data[dit.name()] = dval;
			}
		} else if (val.isArray()) {
			d[key] = jsToStringList(val).join(key == QLatin1String("sources") ? '\n' : ' ');
		} else {
			d[key] = val.toString();
		}
	}

	if (!d.isEmpty()) {
		const int pos = first + (d.contains("position") ? d["position"].toInt() : static_cast<int>(index));
		QSharedPointer<Image> img = parseImage(site, parentPage, d, data, pos, tags);
		if (!img.isNull()) {
			return img;
		}
	}

	return nullptr;
}

ParsedPage JavascriptApi::parsePageInternal(const QString &type, Page *parentPage, const QString &source, int statusCode, int first) const
{
	ParsedPage ret;

	// QMutexLocker locker(m_engineMutex);
	Site *site = parentPage->site();
	const QJSValue &api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property(type).property("parse");
	const QJSValue &results = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (results.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error")) {
		ret.error = results.property("error").toString();
	}

	if (results.hasProperty("tags")) {
		ret.tags = makeTags(results.property("tags"), site);
	}

	if (results.hasProperty("images")) {
		const QJSValue images = results.property("images");
		const quint32 length = images.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			auto img = makeImage(images.property(i), site, parentPage, i, first);
			if (!img.isNull()) {
				ret.images.append(img);
			}
		}
	}

	// Basic properties
	getProperty(results, "imageCount", ret.imageCount);
	getProperty(results, "pageCount", ret.pageCount);
	getProperty(results, "urlNextPage", ret.urlNextPage);
	getProperty(results, "urlPrevPage", ret.urlPrevPage);
	if (results.hasProperty("wiki") && results.property("wiki").isString()) {
		ret.wiki = results.property("wiki").toString();
		ret.wiki = ret.wiki.replace("href=\"/", "href=\"" + site->baseUrl() + "/");
	}

	return ret;
}

bool JavascriptApi::parsePageErrors() const
{
	return getJsConst("search.parseErrors").toBool();
}

ParsedPage JavascriptApi::parsePage(Page *parentPage, const QString &source, int statusCode, int first) const
{
	return parsePageInternal("search", parentPage, source, statusCode, first);
}


PageUrl JavascriptApi::galleryUrl(const QSharedPointer<Image> &gallery, int page, int limit, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("gallery").property("url");
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support galleries";
		return ret;
	}

	QJSValue query = m_engine->newObject();
	query.setProperty("id", QString::number(gallery->id()));
	query.setProperty("md5", gallery->md5());
	query.setProperty("page", page);

	QJSValue opts = m_engine->newObject();
	opts.setProperty("limit", limit);
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	const QJSValue result = urlFunction.call(QList<QJSValue> { query, opts });
	fillUrlObject(result, site, ret);

	return ret;
}

bool JavascriptApi::parseGalleryErrors() const
{
	return getJsConst("gallery.parseErrors").toBool();
}

ParsedPage JavascriptApi::parseGallery(Page *parentPage, const QString &source, int statusCode, int first) const
{
	return parsePageInternal("gallery", parentPage, source, statusCode, first);
}


bool JavascriptApi::mustLoadTagTypes() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue tagTypes = api.property("tagTypes");
	return tagTypes.isUndefined() || !tagTypes.isBool();
}

bool JavascriptApi::canLoadTagTypes() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tagTypes").property("url");
	return !urlFunction.isUndefined() && urlFunction.isCallable();
}

PageUrl JavascriptApi::tagTypesUrl(Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tagTypes").property("url");
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support tag type loading";
		return ret;
	}

	const QJSValue result = urlFunction.call();
	fillUrlObject(result, site, ret);

	return ret;
}

bool JavascriptApi::parseTagTypesErrors() const
{
	return getJsConst("tagTypes.parseErrors").toBool();
}

ParsedTagTypes JavascriptApi::parseTagTypes(const QString &source, int statusCode, Site *site) const
{
	Q_UNUSED(site);

	ParsedTagTypes ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("tagTypes").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (results.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error")) {
		ret.error = results.property("error").toString();
	}
	if (results.hasProperty("types")) {
		const auto &types = results.property("types");
		const quint32 length = types.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			const QJSValue tagType = types.property(i);
			if (!tagType.isObject()) {
				continue;
			}
			TagTypeWithId tt;
			tt.id = tagType.property("id").toInt();
			tt.name = tagType.property("name").toString();
			ret.types.append(tt);
		}
	}

	return ret;
}


bool JavascriptApi::canLoadTags() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tags").property("url");
	return !urlFunction.isUndefined();
}

PageUrl JavascriptApi::tagsUrl(int page, int limit, const QString &order, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("tags").property("url");
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support tag loading";
		return ret;
	}

	QJSValue query = m_engine->newObject();
	query.setProperty("page", page);
	query.setProperty("order", order);

	QJSValue opts = m_engine->newObject();
	opts.setProperty("limit", limit);
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	const QJSValue result = urlFunction.call(QList<QJSValue> { query, opts });
	fillUrlObject(result, site, ret);

	return ret;
}

bool JavascriptApi::parseTagsErrors() const
{
	return getJsConst("tags.parseErrors").toBool();
}

ParsedTags JavascriptApi::parseTags(const QString &source, int statusCode, Site *site) const
{
	ParsedTags ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("tags").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (results.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("error")) {
		ret.error = results.property("error").toString();
	}
	if (results.hasProperty("tags")) {
		ret.tags = makeTags(results.property("tags"), site);
	}

	return ret;
}


bool JavascriptApi::canLoadDetails() const
{
	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	return !urlFunction.isUndefined();
}
bool JavascriptApi::canLoadFullDetails() const
{
	return canLoadDetails()
		&& getJsConst("details.fullResults").toBool();
}

PageUrl JavascriptApi::detailsUrl(qulonglong id, const QString &md5, Site *site) const
{
	PageUrl ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("details").property("url");
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support details loading";
		return ret;
	}

	QJSValue opts = m_engine->newObject();
	opts.setProperty("baseUrl", site->baseUrl());
	opts.setProperty("loggedIn", site->isLoggedIn(false, true));

	const QJSValue result = urlFunction.call(QList<QJSValue> { QString::number(id), md5, opts });
	fillUrlObject(result, site, ret);

	return ret;
}

bool JavascriptApi::parseDetailsErrors() const
{
	return getJsConst("details.parseErrors").toBool();
}

ParsedDetails JavascriptApi::parseDetails(const QString &source, int statusCode, Site *site) const
{
	ParsedDetails ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("details").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (results.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	// Full details
	if (canLoadFullDetails()) {
		ret.image = makeImage(results, site);

		// "Backward compatibility" for older call sites
		ret.tags = ret.image->tags();
		ret.imageUrl = ret.image->fileUrl().toString();
		ret.createdAt = ret.image->createdAt();

		return ret;
	}

	if (results.hasProperty("error") && results.property("error").isString()) {
		ret.error = results.property("error").toString();
	}
	if (results.hasProperty("tags")) {
		ret.tags = makeTags(results.property("tags"), site);
	}
	if (results.hasProperty("imageUrl") && results.property("imageUrl").isString()) {
		ret.imageUrl = results.property("imageUrl").toString();
	}
	if (results.hasProperty("createdAt") && results.property("createdAt").isString()) {
		ret.createdAt = qDateTimeFromString(results.property("createdAt").toString());
	}
	if (results.hasProperty("source") && results.property("source").isString()) {
		ret.sources = QStringList { results.property("source").toString() };
	}
	if (results.hasProperty("sources") && results.property("sources").isArray()) {
		ret.sources = jsToStringList(results.property("sources"));
	}

	if (results.hasProperty("pools")) {
		const QJSValue pools = results.property("pools");
		const quint32 length = pools.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			const QJSValue pool = pools.property(i);
			if (!pool.isObject()) {
				continue;
			}

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
	if (urlFunction.isUndefined()) {
		ret.error = "This API does not support checking";
		return ret;
	}

	const QJSValue result = urlFunction.call();
	fillUrlObject(result, nullptr, ret);

	return ret;
}

bool JavascriptApi::parseCheckErrors() const
{
	return getJsConst("check.parseErrors").toBool();
}

ParsedCheck JavascriptApi::parseCheck(const QString &source, int statusCode) const
{
	ParsedCheck ret;

	// QMutexLocker locker(m_engineMutex);
	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("check").property("parse");
	QJSValue result = parseFunction.call(QList<QJSValue> { source, statusCode });

	// Script errors and exceptions
	if (result.isError()) {
		ret.error = QStringLiteral("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString());
		ret.ok = false;
		return ret;
	}

	ret.ok = result.toBool();

	return ret;
}


static QJSValue tryGetValue(QJSValue api, const QStringList &properties)
{
	for (int i = 0; !api.isUndefined() && i < properties.length(); ++i) {
		api = api.property(properties[i]);
	}
	return api;
}

QJSValue JavascriptApi::getJsConst(const QString &fullKey, const QJSValue &def) const
{
	// QMutexLocker locker(m_engineMutex);

	const QStringList properties = fullKey.split('.');

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue fromApi = tryGetValue(api, properties);
	if (!fromApi.isUndefined()) {
		return fromApi;
	}

	QJSValue fromSource = tryGetValue(m_source, properties);
	if (!fromSource.isUndefined()) {
		return fromSource;
	}

	return def;
}

bool JavascriptApi::needAuth() const
{
	QStringList requiredAuths = jsToStringList(getJsConst("auth"));
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
