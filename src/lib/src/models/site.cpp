#include "models/site.h"
#include <QCryptographicHash>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkCookie>
#include <QNetworkDiskCache>
#include <QSettings>
#include <QStringList>
#include <utility>
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "functions.h"
#include "logger.h"
#include "login/http-basic-login.h"
#include "login/http-get-login.h"
#include "login/http-post-login.h"
#include "login/oauth1-login.h"
#include "login/oauth2-login.h"
#include "login/url-login.h"
#include "mixed-settings.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/source.h"
#include "models/source-engine.h"
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



Site::Site(QString url, Source *source, Profile *profile)
	: Site(url, source->getEngine(), source->getPath().readWritePath(url), profile)
{}

Site::Site(QString url, SourceEngine *engine, const ReadWritePath &dir, Profile *profile)
	: m_profile(profile), m_dir(dir), m_type(engine->getName()), m_url(std::move(url)), m_sourceEngine(engine), m_settings(nullptr), m_manager(nullptr), m_cookieJar(nullptr), m_tagDatabase(nullptr), m_login(nullptr), m_loggedIn(LoginStatus::Unknown), m_autoLogin(true)
{
	// Create the access manager and get its slots
	m_manager = new NetworkManager(this);

	// Cache
	auto *diskCache = new QNetworkDiskCache(m_manager);
	diskCache->setCacheDirectory(m_profile->getPath() + "/cache/");
	diskCache->setMaximumCacheSize(50 * 1024 * 1024);
	m_manager->setCache(diskCache);

	loadConfig();
}

void Site::loadConfig()
{
	if (m_settings != nullptr) {
		m_settings->deleteLater();
	}
	auto *settingsCustom = new QSettings(m_dir.writePath("settings.ini"), QSettings::IniFormat);
	auto *settingsDefaults = new QSettings(m_dir.readPath("defaults.ini"), QSettings::IniFormat);
	m_settings = new MixedSettings(QList<QSettings*> { settingsCustom, settingsDefaults });
	m_name = m_settings->value("name", m_url).toString();

	// Cookies
	if (m_cookieJar == nullptr) {
		m_cookieJar = new PersistentCookieJar(m_dir.writePath("cookies.txt"), m_manager);
		m_manager->setCookieJar(m_cookieJar);
	}

	// Get default source order
	QSettings *pSettings = m_profile->getSettings();
	QStringList defaults {
		pSettings->value("source_1").toString(),
		pSettings->value("source_2").toString(),
		pSettings->value("source_3").toString(),
		pSettings->value("source_4").toString()
	};
	defaults.removeAll("");
	if (defaults.isEmpty()) {
		defaults = QStringList { "Xml", "Json", "Regex", "Rss" };
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
		Api *api = m_sourceEngine->getApi(src == "Regex" ? "Html" : src);
		if (api != nullptr && !m_apis.contains(api)) {
			m_apis.append(api);
		}
	}

	// Auth information
	m_auth = nullptr;
	const QString defType = m_settings->value("login/type", "url").toString();
	if (defType != "disabled") {
		const auto &auths = m_sourceEngine->getAuths();
		if (auths.contains(defType)) {
			m_auth = auths[defType];
		} else if (!auths.isEmpty()) {
			m_auth = auths.first();
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
			} else if (type == "oauth1") {
				m_login = new OAuth1Login(dynamic_cast<OAuth1Auth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "post") {
				m_login = new HttpPostLogin(dynamic_cast<HttpAuth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "get") {
				m_login = new HttpGetLogin(dynamic_cast<HttpAuth*>(m_auth), this, m_manager, m_settings);
			} else if (type == "http_basic") {
				m_login = new HttpBasicLogin(dynamic_cast<HttpBasicAuth*>(m_auth), this, m_manager, m_settings);
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
		QByteArray byteArray = variant.type() == QVariant::ByteArray ? variant.toByteArray() : variant.toString().toUtf8();
		QList<QNetworkCookie> cookies = QNetworkCookie::parseCookies(byteArray);
		for (QNetworkCookie cookie : cookies) {
			cookie.setDomain(m_url);
			cookie.setPath("/");
			m_cookies.append(cookie);
		}
	}
	m_cookieJar->insertCookies(m_cookies);

	// Tag database
	delete m_tagDatabase;
	m_tagDatabase = TagDatabaseFactory::Create(m_dir);
	m_tagDatabase->loadTypes();
	m_tagDatabase->open();

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


QNetworkRequest Site::makeRequest(QUrl url, const QUrl &pageUrl, const QString &ref, Image *img, const QMap<QString, QString> &cHeaders, bool autoLogin)
{
	if (m_autoLogin && autoLogin && m_loggedIn == LoginStatus::Unknown) {
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
	if (referer != "none" && (referer != "page" || !pageUrl.isEmpty())) {
		QString refHeader;
		if (referer == "host") {
			refHeader = url.scheme() + "://" + url.host();
		} else if (referer == "image") {
			refHeader = fixUrl(url).toString();
		} else if (referer == "page" && !pageUrl.isEmpty()) {
			refHeader = fixUrl(pageUrl).toString();
		} else if (referer == "details" && img != nullptr) {
			refHeader = fixUrl(img->pageUrl()).toString();
		}
		request.setRawHeader("Referer", refHeader.toLatin1());
	}

	if (m_login != nullptr) {
		m_login->complementRequest(&request);
	}

	setRequestHeaders(request);

	// Additional headers
	for (auto it = cHeaders.constBegin(); it != cHeaders.constEnd(); ++it) {
		const QString &name = it.key();
		const QString &value = it.value();

		QByteArray val = value.startsWith("md5:")
			? QCryptographicHash::hash(value.toLatin1(), QCryptographicHash::Md5).toHex()
			: value.toLatin1();
		request.setRawHeader(name.toLatin1(), val);
	}

	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);
	return request;
}

void Site::setRequestHeaders(QNetworkRequest &request) const
{
	// Custom headers
	QMap<QString, QString> headers = settingsHeaders();
	for (auto it = headers.constBegin(); it != headers.constEnd(); ++it) {
		request.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
	}

	// User-Agent header tokens and default value
	QString userAgent = request.rawHeader("User-Agent");
	if (userAgent.isEmpty()) {
		userAgent = QStringLiteral("Mozilla/5.0 (Windows NT 10.0; rv:78.0) Gecko/20100101 Firefox/78.0");
	}
	userAgent.replace("%version%", QString(VERSION));
	request.setRawHeader("User-Agent", userAgent.toLatin1());
}

QMap<QString, QString> Site::settingsHeaders() const
{
	QMap<QString, QString> headers;

	QMap<QString, QVariant> legacyHeaders = m_settings->value("headers").toMap();
	for (auto it = legacyHeaders.constBegin(); it != legacyHeaders.constEnd(); ++it) {
		headers.insert(it.key(), it.value().toString());
	}

	m_settings->beginGroup("Headers");
	for (const QString &key : m_settings->childKeys()) {
		headers.insert(key, m_settings->value(key).toString());
	}
	m_settings->endGroup();

	return headers;
}

NetworkReply *Site::get(const QUrl &url, Site::QueryType type, const QUrl &pageUrl, const QString &ref, Image *img, const QMap<QString, QString> &headers)
{
	const QNetworkRequest request = this->makeRequest(url, pageUrl, ref, img, headers);
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

SourceEngine *Site::getSourceEngine() const { return m_sourceEngine; }
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
Api *Site::fullDetailsApi() const
{
	for (Api *api : m_apis) {
		if (api->canLoadFullDetails()) {
			return api;
		}
	}
	return nullptr;
}
Api *Site::tagsApi() const
{
	for (Api *api : m_apis) {
		if (api->canLoadTags()) {
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
