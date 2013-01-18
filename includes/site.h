#ifndef SITE_H
#define SITE_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QMap>
#include <QList>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QSslError>
#include "page.h"
#include "image.h"



class Page;
class Image;

class Site : public QObject
{
	Q_OBJECT


	public:
		enum LoginResult
		{
			LoginError = -1,
			LoginNoLogin = 1,
			LoginSuccess = 0
		};
		Site(QString type, QString name, QMap<QString,QString> data);
		~Site();
		QString type();
		QString name();
		QNetworkReply *loginReply();
		bool contains(QString);
		QString value(QString);
		QString operator[](QString key) { return value(key); }
		void insert(QString, QString);
		QVariant setting(QString key, QVariant def = QVariant());
		QNetworkReply *get(QUrl url, Page *page = NULL, QString referer = "", Image *img = NULL);
		QNetworkReply *get(QString url, Page *page = NULL, QString referer = "", Image *img = NULL) { return get(QUrl(url), page, referer, img); }

	public slots:
		void login();
		void loginFinished();
		void sslErrorHandler(QNetworkReply* qnr, QList<QSslError>);
		void finishedReply(QNetworkReply*);

	signals:
		void loggedIn(Site::LoginResult);
		void finished(QNetworkReply*);

	private:
		QString m_type;
		QString m_name;
		QMap<QString,QString> m_data;
		QSettings *m_settings;
		QString m_sessionId;
		QNetworkAccessManager *m_manager;
		QNetworkReply *m_loginReply;
		bool m_loggedIn, m_triedLogin, m_loginCheck;
};

#endif // SITE_H
