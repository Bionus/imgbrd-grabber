#include "javascript-source-engine.h"
#include <QFile>
#include <QJSEngine>
#include <QJSValueIterator>
#include <QMutex>
#include "api/javascript-api.h"
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "js-helpers.h"
#include "logger.h"


JavaScriptSourceEngine::JavaScriptSourceEngine(QString path, QString helperFile, QObject *parent)
	: SourceEngine(parent), m_path(std::move(path)), m_helperFile(std::move(helperFile))
{
	load();

	m_watcher.addPath(m_path);
	connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &JavaScriptSourceEngine::reload);
}

void JavaScriptSourceEngine::load()
{
	// Tag format mapper
	static const QMap<QString, TagNameFormat::CaseFormat> caseAssoc
	{
		{ "lower", TagNameFormat::Lower },
		{ "upper_first", TagNameFormat::UpperFirst },
		{ "upper", TagNameFormat::Upper },
		{ "caps", TagNameFormat::Caps },
	};

	// Javascript models
	QFile js(m_path);
	if (!js.exists() || !js.open(QIODevice::ReadOnly | QIODevice::Text)) {
		log(QStringLiteral("Could not load JavaScript model file `%1`").arg(m_path), Logger::Error);
		return;
	}

	// Parse JS file, wrapped in a "source" variable
	const QString src = "(function() { var window = {}; " + js.readAll().replace("export var source = ", "return ") + " })()";
	js.close();
	auto *engine = jsEngine();
	m_jsSource = engine->evaluate(src, js.fileName());
	if (m_jsSource.isError()) {
		log(QStringLiteral("Uncaught exception at line %1: %2").arg(m_jsSource.property("lineNumber").toInt()).arg(m_jsSource.toString()), Logger::Error);
		return;
	}

	m_name = m_jsSource.property("name").toString();
	m_additionalTokens = jsToStringList(m_jsSource.property("tokens"));

	// Get the list of APIs for this Source
	const QJSValue apis = m_jsSource.property("apis");
	QJSValueIterator it(apis);
	while (it.hasNext()) {
		it.next();
		m_apis.append(new JavascriptApi(engine, m_jsSource, jsEngineMutex(), it.name()));
	}
	if (m_apis.isEmpty()) {
		log(QStringLiteral("No valid source has been found in the model.js file from %1.").arg(m_name));
	}

	// Read tag naming format
	const QJSValue &tagFormat = m_jsSource.property("tagFormat");
	if (!tagFormat.isUndefined()) {
		const auto caseFormat = caseAssoc.value(tagFormat.property("case").toString(), TagNameFormat::Lower);
		m_tagNameFormat = TagNameFormat(caseFormat, tagFormat.property("wordSeparator").toString());
	}

	// Read auth information
	const QJSValue auths = m_jsSource.property("auth");
	QJSValueIterator authIt(auths);
	while (authIt.hasNext()) {
		authIt.next();

		const QString &id = authIt.name();
		const QJSValue &auth = authIt.value();

		const QString type = auth.property("type").toString();
		Auth *ret = nullptr;

		const QJSValue check = auth.property("check");
		const QString checkType = check.isObject() ? check.property("type").toString() : QString();

		if (type == "oauth2") {
			ret = new OAuth2Auth(type, auth);
		} else if (type == "oauth1") {
			ret = new OAuth1Auth(type, auth);
		} else if (type == "http_basic") {
			const int maxPage = checkType == "max_page" ? check.property("value").toInt() : 0;
			const QString passwordType = auth.property("passwordType").toString();
			ret = new HttpBasicAuth(type, maxPage, passwordType);
		} else {
			QList<AuthField*> fields;
			const QJSValue &jsFields = auth.property("fields");
			const quint32 length = jsFields.property("length").toUInt();
			for (quint32 i = 0; i < length; ++i) {
				const QJSValue &field = jsFields.property(i);

				const QString fid = !field.property("id").isUndefined() ? field.property("id").toString() : QString();
				const QString key = !field.property("key").isUndefined() ? field.property("key").toString() : QString();
				const QString type = field.property("type").toString();

				if (type == "hash") {
					const QString algoStr = field.property("hash").toString();
					const auto algo = algoStr == "sha1" ? QCryptographicHash::Sha1 : QCryptographicHash::Md5;
					fields.append(new AuthHashField(key, algo, field.property("salt").toString()));
				} else if (type == "const") {
					const QString value = field.property("value").toString();
					fields.append(new AuthConstField(key, value));
				} else {
					const QString def = !field.property("def").isUndefined() ? field.property("def").toString() : QString();
					fields.append(new AuthField(fid, key, type == "password" ? AuthField::Password : AuthField::Text, def));
				}
			}

			if (type == "get" || type == "post") {
				const QString url = auth.property("url").toString();
				const QString cookie = checkType == "cookie" ? check.property("key").toString() : QString();
				const QString redirectUrl = checkType == "redirect" ? check.property("url").toString() : QString();

				const QJSValue &csrf = auth.property("csrf");
				const QString csrfUrl = csrf.isObject() ? csrf.property("url").toString() : QString();
				const QStringList csrfFields = csrf.isObject() ? jsToStringList(csrf.property("fields")) : QStringList();

				ret = new HttpAuth(type, url, fields, cookie, redirectUrl, csrfUrl, csrfFields);
			} else {
				const int maxPage = checkType == "max_page" ? check.property("value").toInt() : 0;
				ret = new UrlAuth(type, fields, maxPage);
			}
		}

		m_auths.insert(id, ret);
	}
}

void JavaScriptSourceEngine::reload()
{
	// Ignore "deletion" signal when editors delete the file before overwriting it
	if (!QFile::exists(m_path)) {
		return;
	}

	log(QStringLiteral("Reloading JS source file `%1`...").arg(m_path));

	// Clear previous information
	/*qDeleteAll(m_apis);
	qDeleteAll(m_auths);*/
	m_name.clear();
	m_apis.clear();
	m_auths.clear();
	m_additionalTokens.clear();

	load();
	emit changed();
}

JavaScriptSourceEngine::~JavaScriptSourceEngine()
{
	qDeleteAll(m_apis);
	qDeleteAll(m_auths);
}


QJSEngine *JavaScriptSourceEngine::jsEngine()
{
	static QJSEngine *engine = nullptr;

	if (engine == nullptr) {
		engine = buildJsEngine(m_helperFile);
	}

	return engine;
}
QMutex *JavaScriptSourceEngine::jsEngineMutex()
{
	static QMutex *mutex = nullptr;

	if (mutex == nullptr) {
		mutex = new QMutex();
	}

	return mutex;
}


bool JavaScriptSourceEngine::isValid() const
{
	return !m_apis.isEmpty();
}

const QString &JavaScriptSourceEngine::getName() const { return m_name; }
const QList<Api*> &JavaScriptSourceEngine::getApis() const { return m_apis; }
const QStringList &JavaScriptSourceEngine::getAdditionalTokens() const { return m_additionalTokens; }
const QMap<QString, Auth*> &JavaScriptSourceEngine::getAuths() const { return m_auths; }
