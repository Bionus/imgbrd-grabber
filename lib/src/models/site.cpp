#include "models/site.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkCookie>
#include <QNetworkDiskCache>
#include <QSettings>
#include <QStringList>
#include "auth/http-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "functions.h"
#include "logger.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "login/oauth2-login.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/source.h"
#include "network/network-manager.h"
#include "network/persistent-cookie-jar.h"
#include "tags/tag.h"
#include "tags/tag-database.h"
#include "tags/tag-database-factory.h"

#ifdef QT_DEBUG
	// #define CACHE_POLICY QNetworkRequest::PreferCache
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#else
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#endif



Site::Site(QString url, Source *source)
	: m_type(source->getName()), m_url(std::move(url)), m_source(source), m_settings(nullptr), m_manager(nullptr), m_cookieJar(nullptr), m_tagDatabase(nullptr), m_login(nullptr), m_loggedIn(LoginStatus::Unknown), m_autoLogin(true)
{
	// Create the access manager and get its slots
	m_manager = new NetworkManager(this);

	// Cache
	auto *diskCache = new QNetworkDiskCache(m_manager);
	diskCache->setCacheDirectory(m_source->getProfile()->getPath() + "/cache/");
	diskCache->setMaximumCacheSize(50 * 1024 * 1024);
	m_manager->setCache(diskCache);

	loadConfig();
}

void Site::loadConfig()
{
	const QString siteDir = m_source->getPath() + "/" + m_url + "/";

	if (m_settings != nullptr) {
		m_settings->deleteLater();
	}
	QSettings *settingsCustom = new QSettings(siteDir + "settings.ini", QSettings::IniFormat);
	QSettings *settingsDefaults = new QSettings(siteDir + "defaults.ini", QSettings::IniFormat);
	m_settings = new MixedSettings(QList<QSettings*>() << settingsCustom << settingsDefaults);
	m_name = m_settings->value("name", m_url).toString();

	// Cookies
	if (m_cookieJar == nullptr) {
		m_cookieJar = new PersistentCookieJar(siteDir + "cookies.txt", m_manager);
		m_manager->setCookieJar(m_cookieJar);
	}

	// Get default source order
	QSettings *pSettings = m_source->getProfile()->getSettings();
	QStringList defaults;
	defaults << pSettings->value("source_1").toString()
		<< pSettings->value("source_2").toString()
		<< pSettings->value("source_3").toString()
		<< pSettings->value("source_4").toString();
	defaults.removeAll("");
	if (defaults.isEmpty()) {
		defaults = QStringList() << "Xml" << "Json" << "Regex" << "Rss";
	}

	// Get overridden source order
	QStringList sources;
	if (!m_settings->value("sources/usedefault", true).toBool()) {
		for (int i = 0; i < 4; ++i) {
			const QString def = defaults.count() > i ? defaults[i] : QString();
			sources << m_settings->value("sources/source_" + QString::number(i + 1), def).toString();
		}
		sources.removeAll("");
		if (sources.isEmpty()) {
			sources = defaults;
		}
	} else {
		sources = defaults;
	}
	for (int i = 0; i < sources.count(); i++) {
		sources[i][0] = sources[i][0].toUpper();
	}

	// Apis
	m_apis.clear();
	for (const QString &src : qAsConst(sources)) {
		Api *api = m_source->getApi(src == "Regex" ? "Html" : src);
		if (api != nullptr && !m_apis.contains(api)) {
			m_apis.append(api);
		}
	}

	// Auth information
	m_auth = nullptr;
	const QString defType = m_settings->value("login/type", "url").toString();
	if (defType != "disabled") {
		const auto &auths = m_source->getAuths();
		for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
			if (it.value()->type() == defType || m_auth == nullptr) {
				m_auth = it.value();
			}
		}
		if (m_login != nullptr) {
			m_login->deleteLater();
		}
		if (m_auth != nullptr) {
			QString type = m_auth->type();
			if (type == "url") {
				m_login = new UrlLogin(dynamic_cast<UrlAuth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "oauth2") {
				m_login = new OAuth2Login(dynamic_cast<OAuth2Auth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "post") {
				m_login = new HttpPostLogin(dynamic_cast<HttpAuth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "get") {
				m_login = new HttpGetLogin(dynamic_cast<HttpAuth*>(m_auth), this, m_manager, m_settings);
			} else {
				m_login = nullptr;
				log(QStringLiteral("[%1] Invalid login type '%1'").arg(m_url, type), Logger::Error);
			}
		} else {
			m_login = nullptr;
		}
	}

	// Cookies
	m_cookies.clear();
	QList<QVariant> settingsCookies = m_settings->value("cookies").toList();
	for (const QVariant &variant : settingsCookies) {
		QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(variant.toByteArray());
		for (QNetworkCookie cookie : cookies) {
			cookie.setDomain(m_url);
			cookie.setPath("/");
			m_cookies.append(cookie);
		}
	}
	m_cookieJar->insertCookies(m_cookies);

	// Tag database
	delete m_tagDatabase;
	m_tagDatabase = TagDatabaseFactory::Create(siteDir);
	m_tagDatabase->open();
	m_tagDatabase->loadTypes();

	// Setup throttling
	m_manager->setMaxConcurrency(setting("download/simultaneous", 10).toInt());
	m_manager->setInterval(QueryType::List, setting("download/throttle_page", 0).toInt() * 1000);
	m_manager->setInterval(QueryType::Img, setting("download/throttle_image", 0).toInt() * 1000);
	m_manager->setInterval(QueryType::Thumbnail, setting("download/throttle_thumbnail", 0).toInt() * 1000);
	m_manager->setInterval(QueryType::Details, setting("download/throttle_details", 0).toInt() * 1000);
	m_manager->setInterval(QueryType::Retry, setting("download/throttle_retry", 60).toInt() * 1000);
}

Site::~Site()
{
	m_settings->deleteLater();
	delete m_tagDatabase;
}


/**
 * Try to log into the website.
 *
 * @param force Whether to force login or not
 */
void Site::login(bool force)
{
	if (!force && m_loggedIn == LoginStatus::Pending) {
		return;
	}

	if (!force && m_loggedIn != LoginStatus::Unknown) {
		emit loggedIn(this, LoginResult::Already);
		return;
	}

	if (!canTestLogin()) {
		emit loggedIn(this, LoginResult::Impossible);
		return;
	}

	log(QStringLiteral("[%1] Logging in...").arg(m_url), Logger::Info);

	// Clear cookies if we want to force a re-login
	if (force) {
		m_cookieJar->clear();
		m_cookieJar->insertCookies(m_cookies);
	}

	m_loggedIn = LoginStatus::Pending;

	connect(m_login, &Login::loggedIn, this, &Site::loginFinished);
	m_login->login();
}

bool Site::canTestLogin() const
{
	return m_auth != nullptr && m_login != nullptr && m_login->isTestable();
}

/**
 * Called when the login try is finished.
 */
void Site::loginFinished(Login::Result result)
{
	const bool ok = result == Login::Result::Success;
	m_loggedIn = ok ? LoginStatus::LoggedIn : LoginStatus::LoggedOut;

	log(QStringLiteral("[%1] Login finished: %2.").arg(m_url, ok ? "success" : "failure"));
	emit loggedIn(this, ok ? LoginResult::Success : LoginResult::Error);
}


QNetworkRequest Site::makeRequest(QUrl url, Page *page, const QString &ref, Image *img)
{
	if (m_autoLogin && m_loggedIn == LoginStatus::Unknown) {
		login();
	}

	// Force HTTPS if set so in the settings (no mixed content allowed)
	if (m_settings->value("ssl", false).toBool() && url.scheme() == "http" && url.toString().contains(m_url)) {
		url.setScheme("https");
	}

	QNetworkRequest request(url);
	QString referer = m_settings->value("referer" + (!ref.isEmpty() ? "_" + ref : QString())).toString();
	if (referer.isEmpty() && !ref.isEmpty()) {
		referer = m_settings->value("referer", "none").toString();
	}
	if (referer != "none" && (referer != "page" || page != nullptr)) {
		QString refHeader;
		if (referer == "host") {
			refHeader = url.scheme() + "://" + url.host();
		} else if (referer == "image") {
			refHeader = fixUrl(url).toString();
		} else if (referer == "page" && page != nullptr) {
			refHeader = fixUrl(page->url()).toString();
		} else if (referer == "details" && img != nullptr) {
			refHeader = fixUrl(img->pageUrl()).toString();
		}
		request.setRawHeader("Referer", refHeader.toLatin1());
	}

	if (m_login != nullptr) {
		m_login->complementRequest(&request);
	}

	QMap<QString, QVariant> headers = m_settings->value("headers").toMap();
	for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
		request.setRawHeader(it.key().toLatin1(), it.value().toString().toLatin1());
	}

	// User-Agent header tokens and default value
	QString userAgent = request.rawHeader("User-Agent");
	if (userAgent.isEmpty()) {
		userAgent = QStringLiteral("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:52.0) Gecko/20100101 Firefox/52.0 Grabber/%version%");
	}
	userAgent.replace("%version%", QString(VERSION));
	request.setRawHeader("User-Agent", userAgent.toLatin1());

	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);
	return request;
}

NetworkReply *Site::get(const QUrl &url,  Site::QueryType type, Page *page, const QString &ref, Image *img)
{
	const QNetworkRequest request = this->makeRequest(url, page, ref, img);
	return m_manager->get(request, static_cast<int>(type));
}


QVariant Site::setting(const QString &key, const QVariant &def) const { return m_settings->value(key, def); }
void Site::setSetting(const QString &key, const QVariant &value, const QVariant &def) const { m_settings->setValue(key, value, def); }
void Site::syncSettings() const { m_settings->sync(); }
MixedSettings *Site::settings() const { return m_settings; }
TagDatabase *Site::tagDatabase() const { return m_tagDatabase; }

QString Site::baseUrl() const
{
	const bool ssl = m_settings->value("ssl", false).toBool();
	const QString protocol = (ssl ? QStringLiteral("https") : QStringLiteral("http"));
	return protocol + "://" + m_url;
}

const QString &Site::name() const { return m_name; }
const QString &Site::url() const { return m_url; }
const QString &Site::type() const { return m_type; }

Source *Site::getSource() const { return m_source; }
const QList<Api *> &Site::getApis() const { return m_apis; }
QList<Api *> Site::getLoggedInApis() const
{
	QList<Api*> ret;
	const bool loggedIn = isLoggedIn(true);
	for (Api *api : m_apis) {
		if (!api->needAuth() || loggedIn) {
			ret.append(api);
		}
	}

	return ret;
}
Api *Site::firstValidApi() const
{
	const bool loggedIn = isLoggedIn(true);
	for (Api *api : m_apis) {
		if (!api->needAuth() || loggedIn) {
			return api;
		}
	}
	return nullptr;
}
Api *Site::detailsApi() const
{
	for (Api *api : m_apis) {
		if (api->canLoadDetails()) {
			return api;
		}
	}
	return nullptr;
}

bool Site::autoLogin() const { return m_autoLogin; }
void Site::setAutoLogin(bool autoLogin) { m_autoLogin = autoLogin; }

QString Site::fixLoginUrl(QString url) const
{
	if (m_auth == nullptr || m_login == nullptr) {
		return url;
	}

	return m_login->complementUrl(std::move(url));
}

Auth *Site::getAuth() const { return m_auth; }

QUrl Site::fixUrl(const QString &url, const QUrl &old) const
{
	if (url.isEmpty()) {
		return QUrl();
	}

	const bool ssl = m_settings->value("ssl", false).toBool();
	const QString protocol = (ssl ? QStringLiteral("https") : QStringLiteral("http"));

	if (url.startsWith("//")) {
		return QUrl(protocol + ":" + url);
	}
	if (url.startsWith("/")) {
		const QString baseUrl = m_url.mid(m_url.indexOf('/'));
		const QString right = url.startsWith(baseUrl) ? url.mid(baseUrl.length()) : url;
		return QUrl(protocol + "://" + m_url + right);
	}

	if (!url.startsWith("http")) {
		if (old.isValid()) {
			return old.resolved(QUrl(url));
		}
		return QUrl(protocol + "://" + m_url + "/" + url);
	}

	if (url.startsWith("http://") && ssl && url.contains(m_url)) {
		return QUrl(protocol + "://" + url.mid(7));
	}

	return QUrl(url);
}

const QList<QNetworkCookie> &Site::cookies() const
{
	return m_cookies;
}

bool Site::isLoggedIn(bool unknown, bool pending) const
{
	if (unknown) {
		return m_loggedIn != LoginStatus::LoggedOut;
	}

	if (pending && m_loggedIn == LoginStatus::Pending) {
		return true;
	}

	return m_loggedIn == LoginStatus::LoggedIn;
}

