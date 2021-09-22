#ifndef SITE_H
#define SITE_H

#include <QString>
#include <QUrl>
#include <QVariant>
#include "login/login.h"


class Api;
class Auth;
class Image;
class MixedSettings;
class NetworkManager;
class NetworkReply;
class Page;
class PersistentCookieJar;
class QNetworkCookie;
class QNetworkRequest;
class Source;
class Tag;
class TagDatabase;

class Site : public QObject
{
	Q_OBJECT

	public:
		enum QueryType
		{
			UnknownType = -1,
			List = 0,
			Img = 1,
			Thumbnail = 2,
			Details = 3,
			Retry = 4
		};

		enum LoginResult
		{
			Success = 0,
			Error = 1,
			Impossible = 2,
			Already = 3
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
		QMap<QString, QString> settingsHeaders() const;
		TagDatabase *tagDatabase() const;
		QNetworkRequest makeRequest(QUrl url, const QUrl &pageUrl = {}, const QString &ref = "", Image *img = nullptr, const QMap<QString, QString>& headers = {}, bool login = true);
		NetworkReply *get(const QUrl &url, Site::QueryType type, const QUrl &pageUrl = {}, const QString &ref = "", Image *img = nullptr, const QMap<QString, QString>& headers = {});
		QUrl fixUrl(const QUrl &url) const { return fixUrl(url.toString()); }
		QUrl fixUrl(const QString &url, const QUrl &old = QUrl()) const;
		void setRequestHeaders(QNetworkRequest &request) const;
		bool remove();

		// Api
		const QList<Api*> &getApis() const;
		QList<Api*> getLoggedInApis() const;

		Source *getSource() const;
		Api *firstValidApi() const;
		Api *detailsApi() const;
		Api *tagsApi() const;

		// Login
		void setAutoLogin(bool autoLogin);
		bool autoLogin() const;
		bool isLoggedIn(bool unknown = false, bool pending = false) const;
		bool canTestLogin() const;
		QString fixLoginUrl(QString url) const;

	public slots:
		void login(bool force = false);
		void loginFinished(Login::Result result);

	signals:
		void loggedIn(Site *site, Site::LoginResult result);
		void finishedLoadingTags(const QList<Tag> &tags);
		void removed();

	private:
		QString m_type;
		QString m_name;
		QString m_url;
		Source *m_source;
		QList<QNetworkCookie> m_cookies;
		MixedSettings *m_settings;
		NetworkManager *m_manager;
		PersistentCookieJar *m_cookieJar;
		QList<Api*> m_apis;
		TagDatabase *m_tagDatabase;

		// Login
		Login *m_login;
		Auth *m_auth;
		LoginStatus m_loggedIn = LoginStatus::Unknown;
		bool m_autoLogin;
};

Q_DECLARE_METATYPE(Site::LoginResult)

#endif // SITE_H
