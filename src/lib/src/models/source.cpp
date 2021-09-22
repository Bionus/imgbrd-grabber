#include "models/source.h"
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QJSEngine>
#include <QJSValue>
#include <QJSValueIterator>
#include <QMutex>
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "functions.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/api/javascript-api.h"
#include "models/site.h"
#include "js-helpers.h"


QString getUpdaterBaseUrl()
{
	#if defined NIGHTLY || defined QT_DEBUG
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/src/sites");
	#else
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/src/sites");
	#endif
}

QJSEngine *Source::jsEngine()
{
	static QJSEngine *engine = nullptr;

	if (engine == nullptr) {
		engine = buildJsEngine(m_dir + "/../helper.js");
	}

	return engine;
}
QMutex *Source::jsEngineMutex()
{
	static QMutex *mutex = nullptr;

	if (mutex == nullptr) {
		mutex = new QMutex();
	}

	return mutex;
}

Source::Source(Profile *profile, const QString &dir)
	: m_dir(dir), m_diskName(QFileInfo(dir).fileName()), m_profile(profile), m_updater(m_diskName, m_dir, getUpdaterBaseUrl())
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
	QFile js(m_dir + "/model.js");
	if (js.exists() && js.open(QIODevice::ReadOnly | QIODevice::Text)) {
		log(QStringLiteral("Using Javascript model for %1").arg(m_diskName), Logger::Debug);

		const QString src = "(function() { var window = {}; " + js.readAll().replace("export var source = ", "return ") + " })()";

		m_jsSource = jsEngine()->evaluate(src, js.fileName());
		if (m_jsSource.isError()) {
			log(QStringLiteral("Uncaught exception at line %1: %2").arg(m_jsSource.property("lineNumber").toInt()).arg(m_jsSource.toString()), Logger::Error);
		} else {
			m_name = m_jsSource.property("name").toString();
			m_additionalTokens = jsToStringList(m_jsSource.property("tokens"));

			// Get the list of APIs for this Source
			const QJSValue apis = m_jsSource.property("apis");
			QJSValueIterator it(apis);
			while (it.hasNext()) {
				it.next();
				m_apis.append(new JavascriptApi(m_jsSource, jsEngineMutex(), it.name()));
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
					const QString authType = auth.property("authType").toString();
					const QString tokenUrl = auth.property("tokenUrl").toString();
					const QString authorizationUrl = auth.property("authorizationUrl").toString();
					const QString redirectUrl = auth.property("redirectUrl").toString();
					const QString urlProtocol = auth.property("urlProtocol").toString();
					ret = new OAuth2Auth(type, authType, tokenUrl, authorizationUrl, redirectUrl, urlProtocol);
				} else if (type == "oauth1") {
					const QString temporaryCredentialsUrl = auth.property("temporaryCredentialsUrl").toString();
					const QString authorizationUrl = auth.property("authorizationUrl").toString();
					const QString tokenCredentialsUrl = auth.property("tokenCredentialsUrl").toString();
					ret = new OAuth1Auth(type, temporaryCredentialsUrl, authorizationUrl, tokenCredentialsUrl);
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
						ret = new HttpAuth(type, url, fields, cookie);
					} else {
						const int maxPage = checkType == "max_page" ? check.property("value").toInt() : 0;
						ret = new UrlAuth(type, fields, maxPage);
					}
				}

				if (ret != nullptr) {
					m_auths.insert(id, ret);
				}
			}
		}

		js.close();
	} else {
		log(QStringLiteral("Javascript model not found for '%1' in '%2'").arg(m_diskName, js.fileName()), Logger::Warning);
	}

	// Get the list of all sites pertaining to this source
	QFile f(m_dir + "/sites.txt");
	if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
		while (!f.atEnd()) {
			QString line = f.readLine().trimmed();
			if (line.isEmpty()) {
				continue;
			}

			auto site = new Site(line, this);
			m_sites.append(site);
		}
	}
	if (m_sites.isEmpty()) {
		log(QStringLiteral("No site for source %1").arg(m_name), Logger::Debug);
	}
}

Source::~Source()
{
	qDeleteAll(m_apis);
	qDeleteAll(m_sites);
	qDeleteAll(m_auths);
}


QString Source::getName() const { return m_name; }
QString Source::getPath() const { return m_dir; }
const QList<Site*> &Source::getSites() const { return m_sites; }
const QList<Api*> &Source::getApis() const { return m_apis; }
Profile *Source::getProfile() const { return m_profile; }
const SourceUpdater &Source::getUpdater() const { return m_updater; }
const QStringList &Source::getAdditionalTokens() const { return m_additionalTokens; }
const QMap<QString, Auth*> &Source::getAuths() const { return m_auths; }

Api *Source::getApi(const QString &name) const
{
	for (Api *api : this->getApis()) {
		if (api->getName() == name) {
			return api;
		}
	}
	return nullptr;
}
