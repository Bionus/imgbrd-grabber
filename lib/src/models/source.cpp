#include "models/source.h"
#include <QCryptographicHash>
#include <QJSValue>
#include <QJSValueIterator>
#include <QMutex>
#include "auth/http-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "auth/auth-const-field.h"
#include "auth/auth-field.h"
#include "auth/auth-hash-field.h"
#include "functions.h"
#include "models/api/api.h"
#include "models/api/javascript-api.h"
#include "models/api/javascript-console-helper.h"
#include "models/api/javascript-grabber-helper.h"
#include "models/site.h"


QString getUpdaterBaseUrl()
{
	#if defined NIGHTLY || defined QT_DEBUG
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/release/sites");
	#else
		return QStringLiteral("https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites");
	#endif
}

QJSEngine *Source::jsEngine()
{
	static QJSEngine *engine = nullptr;

	if (engine == nullptr)
	{
		engine = new QJSEngine();
		engine->globalObject().setProperty("Grabber", engine->newQObject(new JavascriptGrabberHelper(*engine)));
		engine->globalObject().setProperty("console", engine->newQObject(new JavascriptConsoleHelper("[JavaScript] ", engine)));

		// JavaScript helper file
		QFile jsHelper(m_dir + "/../helper.js");
		if (jsHelper.open(QFile::ReadOnly | QFile::Text))
		{
			QJSValue helperResult = engine->evaluate(jsHelper.readAll(), jsHelper.fileName());
			jsHelper.close();

			if (helperResult.isError())
			{ log(QStringLiteral("Uncaught exception at line %1: %2").arg(helperResult.property("lineNumber").toInt()).arg(helperResult.toString()), Logger::Error); }
		}
		else
		{ log(QStringLiteral("JavaScript helper file could not be opened"), Logger::Error); }
	}

	return engine;
}
QMutex *Source::jsEngineMutex()
{
	static QMutex *mutex = nullptr;

	if (mutex == nullptr)
	{ mutex = new QMutex(); }

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
	if (js.exists() && js.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		log(QStringLiteral("Using Javascript model for %1").arg(m_diskName), Logger::Debug);

		const QString src = "(function() { var window = {}; " + js.readAll().replace("export var source = ", "return ") + " })()";

		m_jsSource = jsEngine()->evaluate(src, js.fileName());
		if (m_jsSource.isError())
		{ log(QStringLiteral("Uncaught exception at line %1: %2").arg(m_jsSource.property("lineNumber").toInt()).arg(m_jsSource.toString()), Logger::Error); }
		else
		{
			m_name = m_jsSource.property("name").toString();

			// Get the list of APIs for this Source
			const QJSValue apis = m_jsSource.property("apis");
			QJSValueIterator it(apis);
			while (it.hasNext())
			{
				it.next();
				m_apis.append(new JavascriptApi(m_jsSource, jsEngineMutex(), it.name()));
			}
			if (m_apis.isEmpty())
			{ log(QStringLiteral("No valid source has been found in the model.js file from %1.").arg(m_name)); }

			// Read tag naming format
			const QJSValue &tagFormat = m_jsSource.property("tagFormat");
			if (!tagFormat.isUndefined())
			{
				const auto caseFormat = caseAssoc.value(tagFormat.property("case").toString(), TagNameFormat::Lower);
				m_tagNameFormat = TagNameFormat(caseFormat, tagFormat.property("wordSeparator").toString());
			}

			// Read auth information
			const QJSValue auths = m_jsSource.property("auth");
			QJSValueIterator authIt(auths);
			while (authIt.hasNext())
			{
				authIt.next();

				const QString &id = authIt.name();
				const QJSValue &auth = authIt.value();

				const QString type = auth.property("type").toString();
				Auth *ret = nullptr;

				if (type == "oauth2")
				{
					const QString authType = auth.property("authType").toString();
					const QString tokenUrl = auth.property("tokenUrl").toString();
					ret = new OAuth2Auth(type, authType, tokenUrl);
				}
				else
				{
					QList<AuthField*> fields;
					const QJSValue &jsFields = auth.property("fields");
					const quint32 length = jsFields.property("length").toUInt();
					for (quint32 i = 0; i < length; ++i)
					{
						const QJSValue &field = jsFields.property(i);

						const QString key = field.property("key").toString();
						const QString type = field.property("type").toString();

						if (type == "hash")
						{
							const QString algoStr = field.property("hash").toString();
							const auto algo = algoStr == "sha1" ? QCryptographicHash::Sha1 : QCryptographicHash::Md5;
							fields.append(new AuthHashField(key, algo, field.property("salt").toString()));
						}
						else if (type == "const")
						{
							const QString value = field.property("value").toString();
							fields.append(new AuthConstField(key, value));
						}
						else
						{ fields.append(new AuthField(key, type == "password" ? AuthField::Password : AuthField::Username)); }
					}

					if (type == "get" ||  type == "post")
					{
						const QString url = auth.property("url").toString();
						ret = new HttpAuth(type, url, fields);
					}
					else
					{ ret = new UrlAuth(type, fields); }
				}

				if (ret != nullptr)
				{ m_auths.insert(id, ret); }
			}
		}

		js.close();
	}
	else
	{ log(QStringLiteral("Javascript model not found for %1").arg(m_diskName), Logger::Warning); }

	// Get the list of all sites pertaining to this source
	QFile f(m_dir + "/sites.txt");
	if (f.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!f.atEnd())
		{
			QString line = f.readLine().trimmed();
			if (line.isEmpty())
				continue;

			auto site = new Site(line, this);
			m_sites.append(site);
		}
	}
	if (m_sites.isEmpty())
	{ log(QStringLiteral("No site for source %1").arg(m_name), Logger::Debug); }
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
const QMap<QString, Auth*> &Source::getAuths() const { return m_auths; }
Auth *Source::getAuth(const QString &name) const { return m_auths.value(name); }

Api *Source::getApi(const QString &name) const
{
	for (Api *api : this->getApis())
		if (api->getName() == name)
			return api;
	return nullptr;
}
