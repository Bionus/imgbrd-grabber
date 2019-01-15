#ifndef SITE_H
#define SITE_H

#include <QDateTime>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QVariant>
#include "login/login.h"


class Api;
class Auth;
class CustomNetworkAccessManager;
class Image;
class MixedSettings;
class Page;
class QNetworkCookie;
class QNetworkCookieJar;
class QNetworkReply;
class QSettings;
class Source;
class Tag;
class TagDatabase;

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
		~Site() override;
		void loadConfig();
		const QString &type() const;
		const QString &name() const;
		QString baseUrl() const;
		const QString &url() const;
		const QList<QNetworkCookie> &cookies() const;
		QVariant setting(const QString &key, const QVariant &def = QVariant()) const;
		void setSetting(const QString &key, const QVariant &value, const QVariant &def) const;
		void syncSettings() const;
		MixedSettings *settings() const;
		TagDatabase *tagDatabase() const;
		QNetworkRequest makeRequest(QUrl url, Page *page = nullptr, const QString &ref = "", Image *img = nullptr);
		QNetworkReply *get(const QUrl &url, Page *page = nullptr, const QString &ref = "", Image *img = nullptr);
		int msToRequest(QueryType type) const;
		QUrl fixUrl(const QUrl &url) const { return fixUrl(url.toString()); }
		QUrl fixUrl(const QString &url, const QUrl &old = QUrl()) const;

		// Api
		const QList<Api*> &getApis() const;
		QList<Api*> getLoggedInApis() const;

		Source *getSource() const;
		Api *firstValidApi() const;
		Api *detailsApi() const;

		// Login
		void setAutoLogin(bool autoLogin);
		bool autoLogin() const;
		bool isLoggedIn(bool unknown = false, bool pending = false) const;
		bool canTestLogin() const;
		QString fixLoginUrl(QString url) const;
		Auth *getAuth() const;

	private:
		QNetworkReply *getRequest(const QNetworkRequest &request);

	public slots:
		void login(bool force = false);
		void loginFinished(Login::Result result);

	protected:
		void resetCookieJar();

	signals:
		void loggedIn(Site *site, Site::LoginResult result);
		void finished(QNetworkReply *reply);
		void finishedLoadingTags(const QList<Tag> &tags);

	private:
		QString m_type;
		QString m_name;
		QString m_url;
		Source *m_source;
		QList<QNetworkCookie> m_cookies;
		MixedSettings *m_settings;
		CustomNetworkAccessManager *m_manager;
		QNetworkCookieJar *m_cookieJar;
		QNetworkReply *m_updateReply, *m_tagsReply;
		QList<Api*> m_apis;
		TagDatabase *m_tagDatabase;

		// Login
		Login *m_login;
		Auth *m_auth;
		LoginStatus m_loggedIn;
		bool m_autoLogin;

		// Async
		QDateTime m_lastRequest;
};

Q_DECLARE_METATYPE(Site::LoginResult)

#endif // SITE_H
