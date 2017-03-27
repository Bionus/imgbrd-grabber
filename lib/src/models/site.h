#ifndef SITE_H
#define SITE_H

#include <QString>
#include <QVariant>
#include <QSettings>
#include <QMap>
#include <QList>
#include <QNetworkReply>
#include <QUrl>
#include <QSslError>
#include <functional>
#include "models/tag.h"


class Page;
class Image;
class Source;
class Api;
class Profile;
class CustomNetworkAccessManager;

class Site : public QObject
{
	Q_OBJECT

	public:
		enum QueryType
		{
			List = 0,
			Img = 1,
			Thumb = 2,
			Details = 3,
			Retry = 4
		};

		enum LoginResult
		{
			Success = 0,
			Error = 1,
			Impossible = 2,
			Already = 2
		};
		enum LoginStatus
		{
			Unknown = 0,
			Pending = 1,
			LoggedOut = 2,
			LoggedIn = 3,
		};

		Site(QString url, Source *source);
		~Site();
		void loadConfig();
		void initManager();
		QString type();
		QString name();
		QString url();
		QList<QNetworkCookie> cookies() const;
		QVariant setting(QString key, QVariant def = QVariant());
		QSettings *settings();
		QNetworkRequest makeRequest(QUrl url, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		QNetworkReply *get(QUrl url, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		void getAsync(QueryType type, QUrl url, std::function<void(QNetworkReply *)> callback, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		static QList<Site*> getSites(Profile *profile, QStringList sources);
		static QMap<QString, Site *> getAllSites(Profile *profile);
		QUrl fixUrl(QUrl url) const { return fixUrl(url.toString()); }
		QUrl fixUrl(QString url) const;
		QUrl fixUrl(QString url, QUrl old) const;

		// Api
		QList<Api*> getApis(bool filterAuth = false) const;
		Source *getSource() const;
		Api *firstValidApi() const;

		// Login
		void setAutoLogin(bool autoLogin);
		bool autoLogin() const;
		bool isLoggedIn(bool unknown = false) const;
		QString username() const;
		QString password() const;
		void setUsername(QString);
		void setPassword(QString);

		// XML info getters
		bool contains(QString key) const;
		QString value(QString key) const;
		QString operator[](QString key) const { return value(key); }

	private:
		QNetworkReply *getRequest(QNetworkRequest request);

	public slots:
		void login(bool force = false);
		void loginFinished();
		void loadTags(int page, int limit);
		void finishedTags();
		void getCallback();

	protected:
		void resetCookieJar();

	signals:
		void loggedIn(Site*, Site::LoginResult);
		void finished(QNetworkReply*);
		void finishedLoadingTags(QList<Tag>);

	private:
		QString m_type;
		QString m_name;
		QString m_url;
		Source *m_source;
		QList<QNetworkCookie> m_cookies;
		QSettings *m_settings;
		CustomNetworkAccessManager *m_manager;
		QNetworkCookieJar *m_cookieJar;
		QNetworkReply *m_updateReply, *m_tagsReply;
		QList<Api*> m_apis;

		// Login
		QNetworkReply *m_loginReply;
		Page *m_loginPage;
		LoginStatus m_loggedIn;
		bool m_loginCheck, m_autoLogin;
		QString m_username, m_password;

		// Async
		std::function<void(QNetworkReply*)> m_lastCallback;
		QDateTime m_lastRequest;
		QNetworkRequest m_callbackRequest;
};

Q_DECLARE_METATYPE(Site::LoginResult)

#endif // SITE_H
