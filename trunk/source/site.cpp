#include <QFile>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QDebug>
#include <QUrlQuery>
#include "site.h"
#include "functions.h"



Site::Site(QString type, QString url, QMap<QString, QString> data) : m_type(type), m_url(url), m_data(data), m_manager(NULL), m_loggedIn(false), m_triedLogin(false), m_loginCheck(false), m_updateVersion("")
{ load(); }
Site::~Site()
{ delete m_settings; }
void Site::load()
{
	m_settings = new QSettings(savePath("sites/"+m_type+"/"+m_url+"/settings.ini"), QSettings::IniFormat);
	m_name = m_settings->value("name", m_url).toString();
}

void Site::initManager()
{
	if (m_manager == NULL)
	{
		m_manager = new QNetworkAccessManager(this);
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
	m_loggedIn = m_manager->cookieJar()->cookiesForUrl(m_loginReply->url()).isEmpty();
	log(tr("Connexion à %1 (%2) terminée (%2).").arg(m_name, m_url, m_loggedIn ? "succès" : "échec"));
	m_triedLogin = true;
	emit loggedIn(m_loggedIn ? LoginSuccess : LoginError);
}

QNetworkReply *Site::get(QUrl url, Page *page, QString ref, Image *img)
{
	if (!m_loggedIn)
	{ login(); }

	//qDebug() << url;
	/*QString u = url.toString();
	u = u.replace(".com/", ".com.s45.incloak.com/");
	u = u.replace(".us/", ".us.s45.incloak.com/");
	u = u.replace(".net/", ".net.s45.incloak.com/");
	u = u.replace(".org/", ".org.s45.incloak.com/");
	//u = u.replace("//", "/");
	url = QUrl(u);*/
	//qDebug() << url;

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

	initManager();
	return m_manager->get(request);
}
void Site::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }
void Site::finishedReply(QNetworkReply *r)
{
	if (r != m_loginReply)
	{ emit finished(r); }
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

bool Site::contains(QString what)				{ return m_data.contains(what); }
QString Site::value(QString what)				{ return m_data.value(what); }
void Site::insert(QString key, QString value)	{ m_data.insert(key, value); }

QVariant Site::setting(QString key, QVariant def)	{ return m_settings->value(key, def); }

QString Site::name()			{ return m_name;			}
QString Site::url()				{ return m_url;				}
QString Site::type()			{ return m_type;			}
QString Site::updateVersion()	{ return m_updateVersion;	}

QNetworkReply *Site::loginReply()	{ return m_loginReply; }
