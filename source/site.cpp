#include <QDebug>
#include <QFile>
#include <QNetworkCookieJar>
#include "site.h"
#include "functions.h"



Site::Site(QString type, QString name, QMap<QString, QString> data) : m_type(type), m_name(name), m_data(data), m_manager(NULL), m_loggedIn(false)
{
	m_settings = new QSettings(savePath("sites/"+type+"/"+name+"/settings.ini"), QSettings::IniFormat);
}
Site::~Site()
{
	delete m_settings;
}

void Site::login()
{
	if (m_manager == NULL)
	{
		m_manager = new QNetworkAccessManager(this);
		connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
		connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)));
	}

	if (!m_settings->value("login/parameter").toBool() && !m_loggedIn)
	{
		if (!m_settings->value("login/url", "").toString().isEmpty())
		{
			log(tr("Connexion à %1...").arg(m_name));

			QString method = m_settings->value("login/method", "post").toString();
			if (method == "post")
			{
				QUrl postData;
				postData.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_settings->value("auth/pseudo", "").toString());
				postData.addQueryItem(m_settings->value("login/password", "").toString(), m_settings->value("auth/password", "").toString());

				QNetworkRequest request(QUrl(m_settings->value("login/url", "").toString()));
				request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

				m_loginReply = m_manager->post(request, postData.encodedQuery());
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}
			else
			{
				QUrl url = QUrl(m_settings->value("login/url", "").toString());
				url.addQueryItem(m_settings->value("login/pseudo", "").toString(), m_settings->value("auth/pseudo", "").toString());
				url.addQueryItem(m_settings->value("login/password", "").toString(), m_settings->value("auth/pseudo", "").toString());

				QNetworkRequest request(url);

				m_loginReply = m_manager->get(request);
				connect(m_loginReply, SIGNAL(finished()), this, SLOT(loginFinished()));
			}
		}
	}
}
void Site::loginFinished()
{
	m_loggedIn = m_manager->cookieJar()->cookiesForUrl(m_loginReply->url()).isEmpty();
	log(tr("Connexion à %1 terminée (%2).").arg(m_name, m_loggedIn ? "succès" : "échec"));
	emit loggedIn();
}

QNetworkReply *Site::get(QUrl url, Page *page, QString ref, Image *img)
{
	if (m_manager == NULL)
	{
		m_manager = new QNetworkAccessManager(this);
		connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finishedReply(QNetworkReply*)));
		connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrorHandler(QNetworkReply*,QList<QSslError>)));
	}

	if (!m_loggedIn)
	{ login(); }

	QNetworkRequest request(url);
	QString referer = m_settings->value("referer"+(!ref.isEmpty() ? "_"+ref : ""), "").toString();
		if (referer.isEmpty() && !ref.isEmpty())
		{ referer = m_settings->value("referer", "none").toString(); }
		if (referer != "none" && (referer != "page" || page != NULL))
		{
			if (referer == "host")
			{ request.setRawHeader("Referer", QString(url.scheme()+"://"+url.host()).toAscii()); }
			else if (referer == "image")
			{ request.setRawHeader("Referer", url.toString().toAscii()); }
			else if (referer == "page" && page)
			{ request.setRawHeader("Referer", page->url().toString().toAscii()); }
			else if (referer == "details" && img)
			{ request.setRawHeader("Referer", img->pageUrl().toString().toAscii()); }
		}
		QMap<QString,QVariant> headers = m_settings->value("headers").toMap();
		for (int i = 0; i < headers.size(); i++)
		{
			QString key = headers.keys().at(i);
			request.setRawHeader(key.toAscii(), headers[key].toString().toAscii());
		}

	return m_manager->get(request);
}
void Site::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }
void Site::finishedReply(QNetworkReply *r)
{
	if (r != m_loginReply)
	{ emit finished(r); }
}

bool Site::contains(QString what)				{ return m_data.contains(what); }
QString Site::value(QString what)				{ return m_data.value(what); }
void Site::insert(QString key, QString value)	{ m_data.insert(key, value); }

QVariant Site::setting(QString key, QVariant def)	{ return m_settings->value(key, def); }

QString Site::name() { return m_name; }
QString Site::type() { return m_type; }

QNetworkReply *Site::loginReply()	{ return m_loginReply; }
