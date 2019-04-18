#ifndef PERSISTENT_COOKIE_JAR_H
#define PERSISTENT_COOKIE_JAR_H

#include <QMutex>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QList>
#include <QObject>
#include <QUrl>


class PersistentCookieJar : public QNetworkCookieJar
{
	Q_OBJECT

	public:
		explicit PersistentCookieJar(QString filename, QObject *parent = nullptr);
		~PersistentCookieJar();

		void clear();
		bool insertCookies(const QList<QNetworkCookie> &cookies);

		virtual QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
		virtual bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) override;

	protected:
		void save();
		void load();

	private:
		QString m_filename;
		mutable QMutex m_mutex;
};

#endif // PERSISTENT_COOKIE_JAR_H
