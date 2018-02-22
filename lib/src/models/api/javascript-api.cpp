#include "models/api/javascript-api.h"
#include <QJSValueIterator>
#include "logger.h"
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

QString JavascriptApi::pageUrl(const QString &search, int page, int limit, int lastPage, int lastPageMinId, int lastPageMaxId, Site *site) const
{
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

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue urlFunction = api.property("search").property("url");
	QJSValue result = urlFunction.call(QList<QJSValue>() << query << opts << previous);

	// Script errors and exceptions
	if (result.isError())
	{
		log(QString("Uncaught exception at line %1: %2").arg(result.property("lineNumber").toInt()).arg(result.toString()), Logger::Error);
		return QString();
	}

	// Site-ize url
	QString url = result.toString();
	url = site->fixLoginUrl(url, this->value("Urls/Login"));
	url = site->fixUrl(url).toString();

	return url;
}

QList<Tag> JavascriptApi::makeTags(const QJSValue &tags) const
{
	QList<Tag> ret;

	QJSValueIterator it(tags);
	while (it.hasNext())
	{
		it.next();
		QJSValue tag = it.value();

		int id = tag.hasProperty("id") ? tag.property("id").toInt() : 0;
		QString text = tag.property("tag").toString();
		QString type = tag.hasProperty("type") ? tag.property("type").toString() : "unknown";
		int count = tag.hasProperty("count") ? tag.property("count").toInt() : 0;

		ret.append(Tag(id, text, TagType(type), count));
	}

	return ret;
}

ParsedPage JavascriptApi::parsePage(Page *parentPage, const QString &source, int first) const
{
	ParsedPage ret;

	QJSValue api = m_source.property("apis").property(m_key);
	QJSValue parseFunction = api.property("search").property("parse");
	QJSValue results = parseFunction.call(QList<QJSValue>() << source);

	// Script errors and exceptions
	if (results.isError())
	{
		ret.error = QString("Uncaught exception at line %1: %2").arg(results.property("lineNumber").toInt()).arg(results.toString());
		return ret;
	}

	if (results.hasProperty("tags"))
	{ ret.tags = makeTags(results.property("tags")); }

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
				{ tags = makeTags(it3.value()); }
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

	return ret;
}
