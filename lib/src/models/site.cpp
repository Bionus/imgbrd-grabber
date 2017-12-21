#include "models/site.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkDiskCache>
#include <QStringList>
#include <QTimer>
#include <QUrlQuery>
#include "custom-network-access-manager.h"
#include "logger.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "login/oauth2-login.h"
#include "login/url-login.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/source.h"
#include "tags/tag-database-factory.h"
#include "vendor/json.h"

#ifdef QT_DEBUG
	// #define CACHE_POLICY QNetworkRequest::PreferCache
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#else
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#endif



Site::Site(const QString &url, Source *source)
	: m_type(source->getName()), m_url(url), m_source(source), m_settings(nullptr), m_manager(nullptr), m_cookieJar(nullptr), m_loggedIn(LoginStatus::Unknown), m_autoLogin(true)
{
	// Create the access manager and get its slots
	m_manager = new CustomNetworkAccessManager(this);
	connect(m_manager, &CustomNetworkAccessManager::finished, this, &Site::finished);

	// Cache
	auto *diskCache = new QNetworkDiskCache(m_manager);
	diskCache->setCacheDirectory(m_source->getProfile()->getPath() + "/cache/");
	m_manager->setCache(diskCache);

	// Cookies
	resetCookieJar();

	loadConfig();
}

void Site::loadConfig()
{
	QString siteDir = m_source->getPath() + "/" + m_url + "/";

	if (m_settings != nullptr)
		m_settings->deleteLater();
	QSettings *settingsCustom = new QSettings(siteDir + "settings.ini", QSettings::IniFormat);
	QSettings *settingsDefaults = new QSettings(siteDir + "defaults.ini", QSettings::IniFormat);
	m_settings = new MixedSettings(QList<QSettings*>() << settingsCustom << settingsDefaults);
	m_name = m_settings->value("name", m_url).toString();

	// Get default source order
	QSettings *pSettings = m_source->getProfile()->getSettings();
	QStringList defaults;
	defaults << pSettings->value("source_1").toString()
			 << pSettings->value("source_2").toString()
			 << pSettings->value("source_3").toString()
			 << pSettings->value("source_4").toString();
	defaults.removeAll("");
	if (defaults.isEmpty())
	{ defaults =  QStringList() << "Xml" << "Json" << "Regex" << "Rss"; }

	// Get overridden source order
	QStringList sources;
	if (!m_settings->value("sources/usedefault", true).toBool())
	{
		sources << m_settings->value("sources/source_1").toString()
				<< m_settings->value("sources/source_2").toString()
				<< m_settings->value("sources/source_3").toString()
				<< m_settings->value("sources/source_4").toString();
		sources.removeAll("");
		if (sources.isEmpty())
		{ sources = defaults; }
	}
	else
	{ sources = defaults; }
	for (int i = 0; i < sources.count(); i++)
	{ sources[i][0] = sources[i][0].toUpper(); }

	// Apis
	m_apis.clear();
	for (const QString &src : sources)
	{
		Api *api = m_source->getApi(src == "Regex" ? "Html" : src);
		if (api != nullptr && !m_apis.contains(api))
			m_apis.append(api);
	}

	// Auth information
	QString type = m_settings->value("login/type", "url").toString();
	if (type == "url")
		m_login = new UrlLogin(this, m_manager, m_settings);
	else if (type == "oauth2")
		m_login = new OAuth2Login(this, m_manager, m_settings);
	else if (type == "post")
		m_login = new HttpPostLogin(this, m_manager, m_cookieJar, m_settings);
	else if (type == "get")
		m_login = new HttpGetLogin(this, m_manager, m_cookieJar, m_settings);
	else
	{
		m_login = nullptr;
		log(QString("Invalid login type '%1'").arg(type), Logger::Error);
	}

	// Cookies
	m_cookies.clear();
	QList<QVariant> settingsCookies = m_settings->value("cookies", "").toList();
	for (const QVariant &variant : settingsCookies)
	{
		QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(variant.toByteArray());
		for (QNetworkCookie cookie : cookies)
		{
			cookie.setDomain(m_url);
			cookie.setPath("/");
			m_cookies.append(cookie);
		}
	}
	if (m_cookieJar != nullptr)
		resetCookieJar();

	// Tag database
	m_tagDatabase = TagDatabaseFactory::Create(siteDir);
}

Site::~Site()
{
	m_settings->deleteLater();
	delete m_tagDatabase;
}


/**
 * Initialize or reset the site's cookie jar.
 */
void Site::resetCookieJar()
{
	// Delete cookie jar if necessary
	if (m_cookieJar != nullptr)
	{ m_cookieJar->deleteLater(); }

	m_cookieJar = new QNetworkCookieJar(m_manager);

	for (const QNetworkCookie &cookie : m_cookies)
	{ m_cookieJar->insertCookie(cookie); }

	m_manager->setCookieJar(m_cookieJar);
	m_loggedIn = LoginStatus::Unknown;
}


/**
 * Try to log into the website.
 *
 * @param force	Whether to force login or not
 */
void Site::login(bool force)
{
	if (!force && m_loggedIn == LoginStatus::Pending)
		return;

	if (!force && m_loggedIn != LoginStatus::Unknown)
	{
		emit loggedIn(this, LoginResult::Already);
		return;
	}

	log(QString("[%1] Logging in...").arg(m_url), Logger::Info);

	// Clear cookies if we want to force a re-login
	if (force)
		resetCookieJar();

	m_loggedIn = LoginStatus::Pending;

	if (!m_login->isTestable())
	{
		emit loggedIn(this, LoginResult::Impossible);
		return;
	}

	connect(m_login, &Login::loggedIn, this, &Site::loginFinished);
	m_login->login();
}

bool Site::canTestLogin() const
{
	return m_login != nullptr && m_login->isTestable();
}

/**
 * Called when the login try is finished.
 */
void Site::loginFinished(Login::Result result)
{
	bool ok = result == Login::Result::Success;
	m_loggedIn = ok ? LoginStatus::LoggedIn : LoginStatus::LoggedOut;

	log(QString("[%1] Login finished: %2.").arg(m_url, ok ? "success" : "failure"));
	emit loggedIn(this, ok ? LoginResult::Success : LoginResult::Error);
}


QNetworkRequest Site::makeRequest(QUrl url, Page *page, const QString &ref, Image *img)
{
	if (m_autoLogin && m_loggedIn == LoginStatus::Unknown)
		login();

	// Force HTTPS if set so in the settings (no mixed content allowed)
	if (m_settings->value("ssl", false).toBool() && url.scheme() == "http")
		url.setScheme("https");

	QNetworkRequest request(url);
	QString referer = m_settings->value("referer"+(!ref.isEmpty() ? "_"+ref : ""), "").toString();
	if (referer.isEmpty() && !ref.isEmpty())
	{ referer = m_settings->value("referer", "none").toString(); }
	if (referer != "none" && (referer != "page" || page != Q_NULLPTR))
	{
		QString refHeader;
		if (referer == "host")
		{ refHeader = url.scheme()+"://"+url.host(); }
		else if (referer == "image")
		{ refHeader = url.toString(); }
		else if (referer == "page" && page)
		{ refHeader = page->url().toString(); }
		else if (referer == "details" && img)
		{ refHeader = img->pageUrl().toString(); }
		request.setRawHeader("Referer", refHeader.toLatin1());
	}

	m_login->complementRequest(&request);

	QMap<QString, QVariant> headers = m_settings->value("headers").toMap();
	for (auto it = headers.begin(); it != headers.end(); ++it)
	{ request.setRawHeader(it.key().toLatin1(), it.value().toString().toLatin1()); }

	// User-Agent header tokens and default value
	QString userAgent = request.rawHeader("User-Agent");
	if (userAgent.isEmpty())
		userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:52.0) Gecko/20100101 Firefox/52.0 Grabber/%version%";
	userAgent.replace("%version%", QString(VERSION));
	request.setRawHeader("User-Agent", userAgent.toLatin1());

	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);
	return request;
}

/**
 * Get an URL from the site.
 *
 * @param url	The URL to get
 * @param page	The related page
 * @param ref	The type of referer to use (page, image, etc.)
 * @param img	The related image
 * @return		The equivalent network request
 */
void Site::getAsync(QueryType type, const QUrl &url, const std::function<void(QNetworkReply*)> &callback, Page *page, const QString &ref, Image *img)
{
	m_lastCallback = callback;
	m_callbackRequest = this->makeRequest(url, page, ref, img);

	qint64 sinceLastRequest = m_lastRequest.msecsTo(QDateTime::currentDateTime());

	QString key = (type == QueryType::Retry ? "retry" : (type == QueryType::List ? "page" : (type == QueryType::Img ? "image" : (type == QueryType::Thumb ? "thumbnail" : "details"))));
	int def = (type == QueryType::Retry ? 60 : 0);
	int ms = setting("download/throttle_" + key, def).toInt() * 1000;
	ms -= sinceLastRequest;

	if (ms > 0)
	{ QTimer::singleShot(ms, this, SLOT(getCallback())); }
	else
	{ getCallback(); }
}

void Site::getCallback()
{
	m_lastCallback(this->getRequest(m_callbackRequest));
}

QNetworkReply *Site::get(const QUrl &url, Page *page, const QString &ref, Image *img)
{
	QNetworkRequest request = this->makeRequest(url, page, ref, img);
	return this->getRequest(request);
}

QNetworkReply *Site::getRequest(const QNetworkRequest &request)
{
	m_lastRequest = QDateTime::currentDateTime();
	return m_manager->get(request);
}


void Site::loadTags(int page, int limit)
{
	QString protocol = (m_settings->value("ssl", false).toBool() ? "https" : "http");
	m_tagsReply = get(QUrl(protocol + "://"+m_url+"/tags.json?search[hide_empty]=yes&limit="+QString::number(limit)+"&page=" + QString::number(page)));
	connect(m_tagsReply, SIGNAL(finished()), this, SLOT(finishedTags()));
}

void Site::finishedTags()
{
	QString source = m_tagsReply->readAll();
	m_tagsReply->deleteLater();
	QList<Tag> tags;
	QVariant src = Json::parse(source);
	if (!src.isNull())
	{
		QList<QVariant> sourc = src.toList();
		tags.reserve(sourc.count());
		for (int id = 0; id < sourc.count(); id++)
		{
			QMap<QString, QVariant> sc = sourc.at(id).toMap();
			int cat = sc.value("category").toInt();
			tags.append(Tag(sc.value("name").toString(),
							cat == 0 ? "general" : (cat == 1 ? "artist" : (cat == 3 ? "copyright" : "character")),
							sc.value("post_count").toInt(),
							sc.value("related_tags").toString().split(' ')));
		}
	}
	emit finishedLoadingTags(tags);
}

QVariant Site::setting(const QString &key, const QVariant &def)	{ return m_settings->value(key, def); }
void Site::setSetting(const QString &key, const QVariant &value, const QVariant &def)	{ m_settings->setValue(key, value, def); }
void Site::syncSettings() { m_settings->sync(); }
TagDatabase *Site::tagDatabase() const  { return m_tagDatabase;	}

QString Site::name() const { return m_name;	}
QString Site::url() const	{ return m_url;	}
QString Site::type() const	{ return m_type;	}

Source *Site::getSource() const	{ return m_source;		}
QList<Api*> Site::getApis(bool filterAuth) const
{
	if (!filterAuth)
		return m_apis;

	QList<Api*> ret;
	bool loggedIn = isLoggedIn(true);
	for (Api *api : m_apis)
		if (!api->needAuth() || loggedIn)
			ret.append(api);

	return ret;
}
Api *Site::firstValidApi() const
{
	bool loggedIn = isLoggedIn(true);
	for (Api *api : m_apis)
		if (!api->needAuth() || loggedIn)
			return api;
	return nullptr;
}

bool Site::autoLogin() const			{ return m_autoLogin;		}
void Site::setAutoLogin(bool autoLogin)	{ m_autoLogin = autoLogin;	}

QString Site::fixLoginUrl(QString url, const QString &loginPart) const
{
	return m_login->complementUrl(url, loginPart);
}

QUrl Site::fixUrl(const QString &url) const
{
	return this->fixUrl(url, QUrl());
}

QUrl Site::fixUrl(const QString &url, const QUrl &old) const
{
	if (url.isEmpty())
		return QUrl();

	bool ssl = m_settings->value("ssl", false).toBool();
	QString protocol = (ssl ? "https" : "http");

	if (url.startsWith("//"))
	{ return QUrl(protocol + ":" + url); }
	if (url.startsWith("/"))
	{ return QUrl(protocol + "://" + m_url + url); }

	if (!url.startsWith("http"))
	{
		if (old.isValid())
		{ return old.resolved(QUrl(url)); }
		return QUrl(protocol + "://" + m_url + "/" + url);
	}

	if (url.startsWith("http://") && ssl)
	{ return QUrl(protocol + "://" + url.mid(7)); }

	return QUrl(url);
}

QList<QNetworkCookie> Site::cookies() const
{
	return m_cookies;
}

bool Site::isLoggedIn(bool unknown) const
{
	if (unknown)
		return m_loggedIn != LoginStatus::LoggedOut;

	return m_loggedIn == LoginStatus::LoggedIn;
}


bool Site::contains(const QString &key) const
{
	return !m_apis.isEmpty() && m_apis.first()->contains(key);
}
QString Site::value(const QString &key) const
{
	if (m_apis.isEmpty())
		return QString();
	return m_apis.first()->value(key);
}
