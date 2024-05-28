#ifndef PERSISTENT_COOKIE_JAR_H
#define PERSISTENT_COOKIE_JAR_H

#include <QMutex>
#include <QNetworkCookieJar>
#include <QList>


class QNetworkCookie;
class QObject;
class QUrl;

/**
 * Network cookie jar which loads and stores cookies on a persistent file on disk.
 */
class PersistentCookieJar : public QNetworkCookieJar
{
	Q_OBJECT

	public:
		/**
		 * Create a new persistent cookie jar.
		 * @param filename The full path of the file to use to load and store cookies.
		 * @param parent The Qt parent object.
		 */
		explicit PersistentCookieJar(QString filename, QObject *parent = nullptr);

		/**
		 * Saves the cookies before destroying the instance.
		 */
		~PersistentCookieJar() override;

		/**
		 * Remove all cookies from the cookie jar.
		 */
		void clear();

		/**
		 * Add new cookies to the cookie jar.
		 * @param cookies The list of cookies to add to the cookie jar.
		 * @return Whether all cookies were successfully added to the cookie jar.
		 */
		bool insertCookies(const QList<QNetworkCookie> &cookies);

		QList<QNetworkCookie> getAllCookies() const;
		QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
		bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) override;

		/**
		 * Save the cookies to the file.
		 */
		void save();

		/**
		 * Load the cookies from the file.
		 */
		void load();

	private:
		QString m_filename;
		mutable QMutex m_mutex;
};

#endif // PERSISTENT_COOKIE_JAR_H
