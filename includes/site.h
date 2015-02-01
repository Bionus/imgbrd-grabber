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
		Site(QString type, QString url, QMap<QString,QString> data);
		~Site();
		void load();
		void initManager();
		QString type();
		QString name();
		QString url();
		QString updateVersion();
		QNetworkReply *loginReply();
		bool contains(QString);
		QString value(QString);
		QString operator[](QString key) { return value(key); }
		void insert(QString, QString);
		QVariant setting(QString key, QVariant def = QVariant());
		QNetworkReply *get(QUrl url, Page *page = NULL, QString referer = "", Image *img = NULL);
		QNetworkReply *get(QString url, Page *page = NULL, QString referer = "", Image *img = NULL) { return get(QUrl(url), page, referer, img); }
		static QMap<QString, Site*> *getAllSites();
		QUrl fixUrl(QString url);
		QUrl fixUrl(QString url, QUrl old);

	public slots:
		void login();
		void loginFinished();
		void checkForUpdates();
		void checkForUpdatesDone();
		void sslErrorHandler(QNetworkReply* qnr, QList<QSslError>);
		void finishedReply(QNetworkReply*);
		void loadTags(int page, int limit);
		void finishedTags();

	signals:
		void loggedIn(Site::LoginResult);
		void finished(QNetworkReply*);
		void checkForUpdatesFinished(Site*);
		void finishedLoadingTags(QList<Tag>);

	private:
		QString m_type;
		QString m_name;
		QString m_url;
		QMap<QString,QString> m_data;
		QSettings *m_settings;
		QString m_sessionId;
		QNetworkAccessManager *m_manager;
        QNetworkCookieJar *m_cookieJar;
		QNetworkReply *m_loginReply, *m_updateReply, *m_tagsReply;
		bool m_loggedIn, m_triedLogin, m_loginCheck;
		QString m_updateVersion;
};

#endif // SITE_H
