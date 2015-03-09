#include <QFile>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QDebug>
#include <QUrlQuery>
#include <QDir>
#include "site.h"
#include "functions.h"
#include "json.h"



Site::Site(QString type, QString url, QMap<QString, QString> data) : m_type(type), m_url(url), m_data(data), m_manager(NULL), m_loggedIn(false), m_triedLogin(false), m_loginCheck(false), m_updateVersion("")
{ load(); }
Site::~Site()
{
    delete m_settings;
    /*delete m_manager;
    delete m_cookieJar;*/
}
void Site::load()
{
	m_settings = new QSettings(savePath("sites/"+m_type+"/"+m_url+"/settings.ini"), QSettings::IniFormat);
	m_name = m_settings->value("name", m_url).toString();
}

void Site::initManager()
{
	if (m_manager == NULL)
	{
        m_cookieJar = new QNetworkCookieJar(this);

		m_manager = new QNetworkAccessManager(this);
        m_manager->setCookieJar(m_cookieJar);
		connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
		connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrorHandler(QNetworkReply*,QList<QSslError>)));
	}
}

void Site::login()
{
	if (!m_settings->value("login/parameter").toBool() && !m_loggedIn && !m_triedLogin)
	{
		if (!m_settings->value("login/url", "").toString().isEmpty())
		{
			log(tr("Connexion à %1 (%2)...").arg(m_name, m_url));
			initManager();

			QString method = m_settings->value("login/method", "post").toString();
			if (method == "post")
			{
				QUrl postData;
				QUrlQuery query;
				query.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_settings->value("auth/pseudo", "").toString());
				query.addQueryItem(m_settings->value("login/password", "").toString(), m_settings->value("auth/password", "").toString());
				postData.setQuery(query);

				QNetworkRequest request(QUrl(m_settings->value("login/url", "").toString()));
				request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

				m_loginReply = m_manager->post(request, query.query(QUrl::FullyEncoded).toUtf8());
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}
			else
			{
				QUrl url = QUrl(m_settings->value("login/url", "").toString());
				QUrlQuery query;
				query.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_settings->value("auth/pseudo", "").toString());
				query.addQueryItem(m_settings->value("login/password", "").toString(), m_settings->value("auth/password", "").toString());
				url.setQuery(query);

				QNetworkRequest request(url);

				m_loginReply = m_manager->get(request);
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}

			return;
		}
	}

	emit loggedIn(LoginNoLogin);
}
void Site::loginFinished()
{
	m_loggedIn = m_cookieJar->cookiesForUrl(m_loginReply->url()).isEmpty();

	m_triedLogin = true;
	log(tr("Connexion à %1 (%2) terminée (%3).").arg(m_name, m_url, m_loggedIn ? "succès" : "échec"));

	emit loggedIn(m_loggedIn ? LoginSuccess : LoginError);
}

QNetworkReply *Site::get(QUrl url, Page *page, QString ref, Image *img)
{
	if (!m_loggedIn && !m_triedLogin)
        login();

	QNetworkRequest request(url);
	QString referer = m_settings->value("referer"+(!ref.isEmpty() ? "_"+ref : ""), "").toString();
		if (referer.isEmpty() && !ref.isEmpty())
		{ referer = m_settings->value("referer", "none").toString(); }
		if (referer != "none" && (referer != "page" || page != NULL))
		{
			if (referer == "host")
			{ request.setRawHeader("Referer", QString(url.scheme()+"://"+url.host()).toLatin1()); }
			else if (referer == "image")
			{ request.setRawHeader("Referer", url.toString().toLatin1()); }
			else if (referer == "page" && page)
			{ request.setRawHeader("Referer", page->url().toString().toLatin1()); }
			else if (referer == "details" && img)
			{ request.setRawHeader("Referer", img->pageUrl().toString().toLatin1()); }
		}
		QMap<QString,QVariant> headers = m_settings->value("headers").toMap();
		for (int i = 0; i < headers.size(); i++)
		{
			QString key = headers.keys().at(i);
			request.setRawHeader(key.toLatin1(), headers[key].toString().toLatin1());
        }
		request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:35.0) Gecko/20100101 Firefox/35.0");

    initManager();
	return m_manager->get(request);
}
void Site::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }
void Site::finishedReply(QNetworkReply *r)
{
	if (r != m_loginReply)
        emit finished(r);
}

void Site::checkForUpdates()
{
	QString path = m_settings->value("models", "http://imgbrd-grabber.googlecode.com/svn/trunk/release/sites/").toString();
	QString url = path + m_type + "/model.xml";
	initManager();
	m_updateReply = m_manager->get(QNetworkRequest(QUrl(url)));
	connect(m_updateReply, SIGNAL(finished()), this, SLOT(checkForUpdatesDone()));
}
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
		{
			m_updateVersion = VERSION;
		}
	}
	emit checkForUpdatesFinished(this);
}

void _prependUrl(QMap<QString,QString> *details, QString url, QString key, QString lkey = NULL)
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
							QString lineSsl = QString(settings.value("ssl", false).toBool() ? "https" : "http") + "://" + line;
							for (int j = 0; j < srcs.size(); j++)
							{
								QString sr = srcs[j] == "Regex" ? "Html" : srcs[j];
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Tags", "Urls/"+QString::number(j+1)+"/Tags");
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Home", "Urls/"+QString::number(j+1)+"/Home");
								_prependUrl(&details, lineSsl, "Urls/"+sr+"/Pools", "Urls/"+QString::number(j+1)+"/Pools");
								if (details.contains("Urls/"+sr+"/Limit"))
									details["Urls/"+QString::number(j+1)+"/Limit"] = details["Urls/"+sr+"/Limit"];
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
	m_tagsReply = m_manager->get(QNetworkRequest(QUrl("http://"+m_url+"/tags.json?search[hide_empty]=yes&limit="+QString::number(limit)+"&page=" + QString::number(page))));
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

QUrl Site::fixUrl(QString url)
{
	return this->fixUrl(url, QUrl());
}
QUrl Site::fixUrl(QString url, QUrl old)
{
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

QNetworkReply *Site::loginReply()	{ return m_loginReply; }
