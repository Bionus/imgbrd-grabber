#ifndef SITE_H
#define SITE_H

#include <QList>
#include <QMap>
#include <QNetworkReply>
#include <QSettings>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <functional>
#include "mixed-settings.h"
#include "source.h"
#include "tags/tag.h"
#include "tags/tag-database-in-memory.h"


class Page;
class Image;
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
		~Site() override;
		void loadConfig();
		void initManager();
		QString type() const;
		QString name() const;
		QString url() const;
		QList<QNetworkCookie> cookies() const;
		QVariant setting(const QString &key, const QVariant &def = QVariant());
		void setSetting(const QString &key, const QVariant &value, const QVariant &def);
		void syncSettings();
		TagDatabase *tagDatabase() const;
		QNetworkRequest makeRequest(QUrl url, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		QNetworkReply *get(QUrl url, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		void getAsync(QueryType type, QUrl url, std::function<void(QNetworkReply *)> callback, Page *page = nullptr, QString referer = "", Image *img = nullptr);
		QUrl fixUrl(const QUrl &url) const { return fixUrl(url.toString()); }
		QUrl fixUrl(const QString &url) const;
		QUrl fixUrl(const QString &url, const QUrl &old) const;

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
		void setUsername(const QString &username);
		void setPassword(const QString &password);
		bool canTestLogin() const;

		// XML info getters
		bool contains(const QString &key) const;
		QString value(const QString &key) const;
		QString operator[](const QString &key) const { return value(key); }

	private:
		QNetworkReply *getRequest(const QNetworkRequest &request);

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
		MixedSettings *m_settings;
		CustomNetworkAccessManager *m_manager;
		QNetworkCookieJar *m_cookieJar;
		QNetworkReply *m_updateReply, *m_tagsReply;
		QList<Api*> m_apis;
		TagDatabase *m_tagDatabase;

		// Login
		QNetworkReply *m_loginReply;
		Page *m_loginPage;
		LoginStatus m_loggedIn;
		bool m_loginCheck, m_autoLogin;
		QString m_username, m_password, m_token;

		// Async
		std::function<void(QNetworkReply*)> m_lastCallback;
		QDateTime m_lastRequest;
		QNetworkRequest m_callbackRequest;
};

Q_DECLARE_METATYPE(Site::LoginResult)

#endif // SITE_H
