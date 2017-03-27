#include <QFile>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkDiskCache>
#include <QStringList>
#include <QUrlQuery>
#include <QDir>
#include <QTimer>
#include "site.h"
#include "functions.h"
#include "vendor/json.h"
#include "page.h"
#include "image.h"
#include "source.h"
#include "profile.h"
#include "api.h"
#include "custom-network-access-manager.h"
#include "vendor/qcustomnetworkreply.h"
#ifdef QT_DEBUG
	#include <QDebug>
	// #define CACHE_POLICY QNetworkRequest::PreferCache
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#else
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#endif



Site::Site(QString url, Source *source)
	: m_type(source->getName()), m_url(url), m_source(source), m_settings(nullptr), m_manager(nullptr), m_cookieJar(nullptr), m_loggedIn(LoginStatus::Unknown), m_loginCheck(false), m_autoLogin(true)
{
	loadConfig();
}

void Site::loadConfig()
{
	if (m_settings != nullptr)
		m_settings->deleteLater();
	m_settings = new QSettings(m_source->getPath() + "/" + m_url + "/settings.ini", QSettings::IniFormat);
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

	// Get overriden source order
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
	for (QString src : sources)
	{
		Api *api = m_source->getApi(src == "Regex" ? "Html" : src);
		if (api != nullptr && !m_apis.contains(api))
			m_apis.append(api);
	}

	// Auth information
	m_username = m_settings->value("auth/pseudo", "").toString();
	m_password = m_settings->value("auth/password", "").toString();

	// Cookies
	m_cookies.clear();
	QList<QVariant> cookies = m_settings->value("cookies", "").toList();
	for (QVariant variant : cookies)
	{
		QList<QNetworkCookie> cookiz = QNetworkCookie::parseCookies(variant.toByteArray());
		for (QNetworkCookie cookie : cookiz)
		{
			cookie.setDomain(m_url);
			cookie.setPath("/");
			m_cookies.append(cookie);
		}
	}
	if (m_cookieJar != nullptr)
		resetCookieJar();
}

Site::~Site()
{
	m_settings->deleteLater();
}


/**
 * Initialize the network manager.
 */
void Site::initManager()
{
	if (m_manager == nullptr)
	{
		// Create the access manager and get its slots
		m_manager = new CustomNetworkAccessManager(this);
		connect(m_manager, &CustomNetworkAccessManager::finished, this, &Site::finished);

		// Cache
		QNetworkDiskCache *diskCache = new QNetworkDiskCache(m_manager);
		diskCache->setCacheDirectory(m_source->getProfile()->getPath() + "/cache/");
		m_manager->setCache(diskCache);

		// Cookies
		resetCookieJar();
	}
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

	for (QNetworkCookie cookie : m_cookies)
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
	if (!force && m_loggedIn != LoginStatus::Unknown && m_loggedIn != LoginStatus::Pending)
	{
		emit loggedIn(this, LoginResult::Already);
		return;
	}

	log(QString("Logging into %1 (%2)...").arg(m_name, m_url));
	initManager();

	// Clear cookies if we want to force a re-login
	if (force)
		resetCookieJar();

	m_loggedIn = LoginStatus::Pending;

	bool byParameter = m_settings->value("login/parameter", true).toBool();
	if (byParameter)
	{
		int maxPageAnonymous = m_settings->value("login/maxPage", 0).toInt();
		if (maxPageAnonymous <= 0)
		{
			emit loggedIn(this, LoginResult::Impossible);
			return;
		}

		m_loginPage = new Page(m_source->getProfile(), this, QList<Site*>() << this, QStringList(), maxPageAnonymous);
		connect(m_loginPage, &Page::finishedLoading, this, &Site::loginFinished);
		connect(m_loginPage, &Page::failedLoading, this, &Site::loginFinished);
		m_loginPage->load();

		return;
	}

	// Cannot post login information without an URL
	QString loginUrl = m_settings->value("login/url", "").toString();
	if (loginUrl.isEmpty())
	{
		emit loggedIn(this, LoginResult::Impossible);
		return;
	}

	QUrlQuery query;
	query.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_username);
	query.addQueryItem(m_settings->value("login/password", "").toString(), m_password);

	m_settings->beginGroup("login/fields");
		QStringList keys = m_settings->childKeys();
		for (QString key : keys)
			query.addQueryItem(key, setting(key).toString());
	m_settings->endGroup();

	QString method = m_settings->value("login/method", "post").toString();
	if (method == "post")
	{
		QUrl postData;
		postData.setQuery(query);

		QNetworkRequest request(fixUrl(m_settings->value("login/url", "").toString()));
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

		m_loginReply = m_manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
		connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
	}
	else
	{
		QUrl url = fixUrl(m_settings->value("login/url", "").toString());
		url.setQuery(query);

		QNetworkRequest request(url);
		request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);

		m_loginReply = getRequest(request);
		connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
	}
}

/**
 * Called when the login try is finished.
 */
void Site::loginFinished()
{
	bool byParameter = m_settings->value("login/parameter", true).toBool();
	bool ok = false;
	if (byParameter)
	{
		ok = !m_loginPage->images().isEmpty();
	}
	else
	{
		QString cookiename = m_settings->value("login/cookie", "").toString();

		QList<QNetworkCookie> cookies = m_cookieJar->cookiesForUrl(m_loginReply->url());
		for (QNetworkCookie cookie : cookies)
		{
			if (cookie.name() == cookiename && !cookie.value().isEmpty())
			{ ok = true; }
		}
	}
	m_loggedIn = ok ? LoginStatus::LoggedIn : LoginStatus::LoggedOut;

	log(QString("Logging into %1 (%2) finished (%3).").arg(m_name, m_url, ok ? tr("success") : tr("failure")));
	emit loggedIn(this, ok ? LoginResult::Success : LoginResult::Error);
}


QNetworkRequest Site::makeRequest(QUrl url, Page *page, QString ref, Image *img)
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
	if (referer != "none" && (referer != "page" || page != NULL))
	{
		QString ref;
		if (referer == "host")
		{ ref = url.scheme()+"://"+url.host(); }
		else if (referer == "image")
		{ ref = url.toString(); }
		else if (referer == "page" && page)
		{ ref = page->url().toString(); }
		else if (referer == "details" && img)
		{ ref = img->pageUrl().toString(); }
		request.setRawHeader("Referer", ref.toLatin1());
	}

	QMap<QString,QVariant> headers = m_settings->value("headers").toMap();
	request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:52.0) Gecko/20100101 Firefox/52.0");
	for (QString key : headers.keys())
	{ request.setRawHeader(key.toLatin1(), headers[key].toString().toLatin1()); }

	initManager();
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
void Site::getAsync(QueryType type, QUrl url, std::function<void(QNetworkReply*)> callback, Page *page, QString ref, Image *img)
{
	m_lastCallback = callback;
	m_callbackRequest = this->makeRequest(url, page, ref, img);

	int sinceLastRequest = m_lastRequest.msecsTo(QDateTime::currentDateTime());

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

QNetworkReply *Site::get(QUrl url, Page *page, QString ref, Image *img)
{
	QNetworkRequest request = this->makeRequest(url, page, ref, img);
	return this->getRequest(request);
}

QNetworkReply *Site::getRequest(QNetworkRequest request)
{
	m_lastRequest = QDateTime::currentDateTime();
	return m_manager->get(request);
}


QList<Site*> Site::getSites(Profile *profile, QStringList sources)
{
	QMap<QString, Site*> sites = Site::getAllSites(profile);

	QList<Site*> ret;
	for (QString source : sources)
		if (sites.contains(source))
			ret.append(sites.value(source));

	return ret;
}
QMap<QString, Site*> Site::getAllSites(Profile *profile)
{
	QMap<QString, Site*> ret;

	QList<Source*> *sources = Source::getAllSources(profile);
	for (Source *source : *sources)
		for (Site *site : source->getSites())
			ret.insert(site->url(), site);

	return ret;
}


void Site::loadTags(int page, int limit)
{
	initManager();

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

QVariant Site::setting(QString key, QVariant def)	{ return m_settings->value(key, def); }
QSettings	*Site::settings()						{ return m_settings; }

QString Site::name()			{ return m_name;			}
QString Site::url()				{ return m_url;				}
QString Site::type()			{ return m_type;			}

Source *Site::getSource() const	{ return m_source;			}
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


bool Site::autoLogin() const	{ return m_autoLogin;	}
QString Site::username() const	{ return m_username;	}
QString Site::password() const	{ return m_password;	}

void Site::setAutoLogin(bool autoLogin)		{ m_autoLogin = autoLogin;	}
void Site::setUsername(QString username)	{ m_username = username;	}
void Site::setPassword(QString password)	{ m_password = password;	}

QUrl Site::fixUrl(QString url) const
{
	return this->fixUrl(url, QUrl());
}

QUrl Site::fixUrl(QString url, QUrl old) const
{
	if (url.isEmpty())
		return QUrl();

	QString protocol = (m_settings->value("ssl", false).toBool() ? "https" : "http");

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

	return QUrl(url);
}

QList<QNetworkCookie> Site::cookies() const
{
	return m_cookies;
}

bool Site::isLoggedIn(bool unknown) const
{
	if (m_settings->value("login/parameter", true).toBool() && !m_username.isEmpty() && !m_password.isEmpty())
		return true;

	if (unknown)
		return m_loggedIn != LoginStatus::LoggedOut;

	return m_loggedIn == LoginStatus::LoggedIn;
}


bool Site::contains(QString key) const
{
	if (m_apis.isEmpty())
		return false;
	return m_apis.first()->contains(key);
}
QString Site::value(QString key) const
{
	if (m_apis.isEmpty())
		return QString();
	return m_apis.first()->value(key);
}
