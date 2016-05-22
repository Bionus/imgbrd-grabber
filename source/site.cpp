#include <QFile>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkDiskCache>
#include <QUrlQuery>
#include <QDir>
#include <QTimer>
#include "site.h"
#include "functions.h"
#include "json.h"
#include "page.h"
#include "image.h"
#ifdef QT_DEBUG
	#include <QDebug>
	#define CACHE_POLICY QNetworkRequest::PreferCache
#else
	#define CACHE_POLICY QNetworkRequest::PreferNetwork
#endif



Site::Site(QString type, QString url, QMap<QString, QString> data) : m_type(type), m_url(url), m_data(data), m_settings(nullptr), m_manager(nullptr), m_cookieJar(nullptr), m_loggedIn(false), m_triedLogin(false), m_loginCheck(false), m_updateVersion("")
{
	load();
}

Site::~Site()
{
    delete m_settings;
	//delete m_manager->deleteLater();
	//delete m_cookieJar->deleteLater();
}

/**
 * Load or reload the settings.
 */
void Site::load()
{
	// Delete settings if necessary
	if (m_settings != nullptr)
	{ m_settings->deleteLater(); }

	m_settings = new QSettings(savePath("sites/"+m_type+"/"+m_url+"/settings.ini"), QSettings::IniFormat);
	m_name = m_settings->value("name", m_url).toString();

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
	{ resetCookieJar(); }
}

/**
 * Initialize or reset the site's cookie jar.
 */
void Site::resetCookieJar()
{
	// Delete cookie jar if necessary
	if (m_cookieJar != nullptr)
	{ m_cookieJar->deleteLater(); }

	m_cookieJar = new QNetworkCookieJar(this);

	for (QNetworkCookie cookie : m_cookies)
	{ m_cookieJar->insertCookie(cookie); }

	m_manager->setCookieJar(m_cookieJar);
	m_loggedIn = false;
	m_triedLogin = false;
}

/**
 * Initialize the network manager.
 */
void Site::initManager()
{
	if (m_manager == nullptr)
	{
		// Create the access manager and get its slots
		m_manager = new QNetworkAccessManager(this);
		connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
		connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrorHandler(QNetworkReply*,QList<QSslError>)));

		// Cache
		QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
		diskCache->setCacheDirectory(savePath("cache/"));
		m_manager->setCache(diskCache);

		// Cookies
		resetCookieJar();
	}
}

/**
 * Try to log into the website.
 *
 * @param force	Whether to force login or not
 */
void Site::login(bool force)
{
	if (!m_settings->value("login/parameter").toBool() && (force || (!m_loggedIn && !m_triedLogin)))
	{
		if (!m_settings->value("login/url", "").toString().isEmpty())
		{
			log(tr("Connexion à %1 (%2)...").arg(m_name, m_url));
			initManager();

			if (force)
			{ resetCookieJar(); }

			m_triedLogin = true;

			QString method = m_settings->value("login/method", "post").toString();
			if (method == "post")
			{
				QUrl postData;
				QUrlQuery query;
				query.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_username);
				query.addQueryItem(m_settings->value("login/password", "").toString(), m_password);
				postData.setQuery(query);

				QNetworkRequest request(fixUrl(m_settings->value("login/url", "").toString()));
				request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

				m_loginReply = m_manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}
			else
			{
				QUrl url = fixUrl(m_settings->value("login/url", "").toString());
				QUrlQuery query;
				query.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_username);
				query.addQueryItem(m_settings->value("login/password", "").toString(), m_password);
				url.setQuery(query);

				QNetworkRequest request(url);
				request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);

				m_loginReply = m_manager->get(request);
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}

			return;
		}
	}

	emit loggedIn(this, LoginNoLogin);
}

/**
 * Called when the login try is finished.
 */
void Site::loginFinished()
{
	m_loggedIn = false;
	QString cookiename = m_settings->value("login/cookie", "").toString();

	QList<QNetworkCookie> cookies = m_cookieJar->cookiesForUrl(m_loginReply->url());
	for (QNetworkCookie cookie : cookies)
	{
		if (cookie.name() == cookiename && !cookie.value().isEmpty())
		{ m_loggedIn = true; }
	}

	log(tr("Connexion à %1 (%2) terminée (%3).").arg(m_name, m_url, m_loggedIn ? tr("succès") : tr("échec")));

	emit loggedIn(this, m_loggedIn ? LoginSuccess : LoginError);
}

QNetworkRequest Site::makeRequest(QUrl url, Page *page, QString ref, Image *img)
{
	if (!m_loggedIn && !m_triedLogin)
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
	request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:35.0) Gecko/20100101 Firefox/35.0");
	for (int i = 0; i < headers.size(); i++)
	{
		QString key = headers.keys().at(i);
		request.setRawHeader(key.toLatin1(), headers[key].toString().toLatin1());
	}

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
	int ms = setting("download/throttle_" + key, 0).toInt() * 1000;
	ms -= sinceLastRequest;

	if (ms > 0)
	{ QTimer::singleShot(ms, this, SLOT(getCallback())); }
	else
	{ getCallback(); }
}
void Site::getCallback()
{
	m_lastRequest = QDateTime::currentDateTime();
	m_lastCallback(m_manager->get(m_callbackRequest));
}

QNetworkReply *Site::get(QUrl url, Page *page, QString ref, Image *img)
{
	m_lastRequest = QDateTime::currentDateTime();
	QNetworkRequest request = this->makeRequest(url, page, ref, img);
	return m_manager->get(request);
}

/**
 * Log SSL errors in debug mode only.
 *
 * @param qnr		The network reply who generated the SSL errors
 * @param errors	The list of SSL errors that occured
 */
void Site::sslErrorHandler(QNetworkReply* qnr, QList<QSslError> errors)
{
	#ifdef QT_DEBUG
		qDebug() << errors;
	#endif
	qnr->ignoreSslErrors();
}

/**
 * Called when a reply is finished.
 *
 * @param r	The finished reply
 */
void Site::finishedReply(QNetworkReply *r)
{
	if (r != m_loginReply)
        emit finished(r);
}

/**
 * Check if an update is available for this source's model file.
 */
void Site::checkForUpdates()
{
	QString path = m_settings->value("models", "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/release/sites/").toString();
	QString url = path + m_type + "/model.xml";

	initManager();

	QNetworkRequest request = QNetworkRequest(QUrl(url));
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);

	m_updateReply = m_manager->get(request);
	connect(m_updateReply, SIGNAL(finished()), this, SLOT(checkForUpdatesDone()));
}

/**
 * Called when the update check is finished.
 */
void Site::checkForUpdatesDone()
{
	QString source = m_updateReply->readAll();
	if (source.left(5) == "<?xml")
	{
		QFile current(savePath("sites/"+m_type+"/model.xml"));
		current.open(QFile::ReadOnly);
		QString compare = current.readAll();
		current.close();

		if (compare != source)
		{ m_updateVersion = VERSION; }
	}
	emit checkForUpdatesFinished(this);
}

void _prependUrl(QMap<QString,QString> *details, QString url, QString key, QString lkey = QString())
{
	if (details->contains(key))
		details->insert(lkey == NULL ? key : lkey, url + details->value(key));
}

QMap<QString, Site*> *Site::getAllSites()
{
	QMap<QString, Site*> *stes = new QMap<QString, Site*>();
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList dir = QDir(savePath("sites")).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i < dir.count(); i++)
	{
		QFile file(savePath("sites/"+dir.at(i)+"/model.xml"));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString source = file.readAll();
			QDomDocument doc;
			QString errorMsg;
			int errorLine, errorColumn;
			if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
			{ log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Error); }
			else
			{
				QDomElement docElem = doc.documentElement();
				QMap<QString,QString> detals = domToMap(docElem);
				QStringList defaults = QStringList() << "xml" << "json" << "rss" << "regex";
				QStringList source;
				for (int s = 0; s < 4; s++)
				{
					QString t = settings.value("source_"+QString::number(s+1), defaults.at(s)).toString();
					t[0] = t[0].toUpper();
					if (detals.contains("Urls/"+(t == "Regex" ? "Html" : t)+"/Tags"))
					{ source.append(t); }
				}
				if (!source.isEmpty())
				{
					QFile f(savePath("sites/"+dir[i]+"/sites.txt"));
					if (f.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						while (!f.atEnd())
						{
							QString line = f.readLine().trimmed();
							if (line.isEmpty())
								continue;

							QStringList srcs;
							QSettings sets(savePath("sites/"+dir[i]+"/"+line+"/settings.ini"), QSettings::IniFormat);
							if (!sets.value("sources/usedefault", true).toBool())
							{
								srcs = QStringList() << sets.value("sources/source_1").toString()
													 << sets.value("sources/source_2").toString()
													 << sets.value("sources/source_3").toString()
													 << sets.value("sources/source_4").toString();
								srcs.removeAll("");
								if (srcs.isEmpty())
								{ srcs = source; }
								else
								{
									for (int i = 0; i < srcs.size(); i++)
									{ srcs[i][0] = srcs[i][0].toUpper(); }
								}
							}
							else
							{ srcs = source; }

							QMap<QString,QString> details = detals;
							details["Model"] = dir[i];
							details["Url"] = line;
							details["Selected"] = srcs.join("/").toLower();
							QString lineSsl = QString(sets.value("ssl", false).toBool() ? "https" : "http") + "://" + line;
							for (int j = 0; j < srcs.size(); j++)
							{
								QString sr = srcs[j] == "Regex" ? "Html" : srcs[j];
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Tags", "Urls/"+QString::number(j+1)+"/Tags");
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Home", "Urls/"+QString::number(j+1)+"/Home");
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Pools", "Urls/"+QString::number(j+1)+"/Pools");
								if (details.contains("Urls/"+sr+"/Login"))
									details["Urls/"+QString::number(j+1)+"/Login"] = details["Urls/"+sr+"/Login"];
								if (details.contains("Urls/"+sr+"/Limit"))
									details["Urls/"+QString::number(j+1)+"/Limit"] = details["Urls/"+sr+"/Limit"];
								if (details.contains("Urls/"+sr+"/Image"))
									details["Urls/"+QString::number(j+1)+"/Image"] = details["Urls/"+sr+"/Image"];
								if (details.contains("Urls/"+sr+"/Sample"))
									details["Urls/"+QString::number(j+1)+"/Sample"] = details["Urls/"+sr+"/Sample"];
								if (details.contains("Urls/"+sr+"/Preview"))
									details["Urls/"+QString::number(j+1)+"/Preview"] = details["Urls/"+sr+"/Preview"];
								if (details.contains("Urls/"+sr+"/MaxPage"))
									details["Urls/"+QString::number(j+1)+"/MaxPage"] = details["Urls/"+sr+"/MaxPage"];
								if (details.contains("Urls/"+sr+"/AltPagePrev"))
									details["Urls/"+QString::number(j+1)+"/AltPagePrev"] = details["Urls/"+sr+"/AltPagePrev"];
								if (details.contains("Urls/"+sr+"/AltPageNext"))
									details["Urls/"+QString::number(j+1)+"/AltPageNext"] = details["Urls/"+sr+"/AltPageNext"];
							}
							_prependUrl(&details, lineSsl, "Urls/Html/Post");
							_prependUrl(&details, lineSsl, "Urls/Html/Tags");
							_prependUrl(&details, lineSsl, "Urls/Html/Home");
							_prependUrl(&details, lineSsl, "Urls/Html/Pools");

							Site *site = new Site(dir[i], line, details);
							stes->insert(line, site);
						}
					}
					else
					{ log(tr("Fichier sites.txt du modèle %1 introuvable.").arg(dir[i]), Error); }
					f.close();
				}
				else
				{ log(tr("Aucune source valide trouvée dans le fichier model.xml de %1.").arg(dir[i])); }
			}
			file.close();
		}
	}
	return stes;
}

void Site::loadTags(int page, int limit)
{
	initManager();

	QString protocol = (m_settings->value("ssl", false).toBool() ? "https" : "http");
	QNetworkRequest request(QUrl(protocol + "://"+m_url+"/tags.json?search[hide_empty]=yes&limit="+QString::number(limit)+"&page=" + QString::number(page)));
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, CACHE_POLICY);
	m_tagsReply = m_manager->get(request);
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

bool Site::contains(QString what)				{ return m_data.contains(what); }
QString Site::value(QString what)				{ return m_data.value(what); }
void Site::insert(QString key, QString value)	{ m_data.insert(key, value); }

QVariant Site::setting(QString key, QVariant def)	{ return m_settings->value(key, def); }

QString Site::name()			{ return m_name;			}
QString Site::url()				{ return m_url;				}
QString Site::type()			{ return m_type;			}
QString Site::updateVersion()	{ return m_updateVersion;	}
QString Site::username()		{ return m_username;		}
QString Site::password()		{ return m_password;		}

void Site::setUsername(QString username)	{ m_username = username;	}
void Site::setPassword(QString password)	{ m_password = password;	}

QUrl Site::fixUrl(QString url)
{
	return this->fixUrl(url, QUrl());
}

QUrl Site::fixUrl(QString url, QUrl old)
{
	if (url.isEmpty())
		return QUrl();

	QString protocol = (m_settings->value("ssl", false).toBool() ? "https" : "http");

	if (url.startsWith("//"))
	{ return QUrl(protocol + ":" + url); }
	if (url.startsWith("/"))
	{ return QUrl(protocol + "://" + m_data.value("Url") + url); }

	if (!url.startsWith("http"))
	{
		if (old.isValid())
		{ return old.resolved(QUrl(url)); }
		return QUrl(protocol + "://" + m_data.value("Url") + "/" + url);
	}

	return QUrl(url);
}

QNetworkReply *Site::loginReply()
{
	return m_loginReply;
}

QList<QNetworkCookie> Site::cookies()
{
	return m_cookies;
}
