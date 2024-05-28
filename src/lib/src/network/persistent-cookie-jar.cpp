#include "persistent-cookie-jar.h"
#include <utility>
#include <QFile>
#include <QMutexLocker>
#include <QNetworkCookie>
#include "utils/file-utils.h"


PersistentCookieJar::PersistentCookieJar(QString filename, QObject *parent)
	: QNetworkCookieJar(parent), m_filename(std::move(filename))
{
	load();
}

PersistentCookieJar::~PersistentCookieJar()
{
	save();
}

void PersistentCookieJar::clear()
{
	const QList<QNetworkCookie> list = allCookies();
	for (const QNetworkCookie &cookie : list) {
		deleteCookie(cookie);
	}
}

bool PersistentCookieJar::insertCookies(const QList<QNetworkCookie> &cookies)
{
	bool all = true;
	for (const QNetworkCookie &cookie : cookies) {
		all = all && insertCookie(cookie);
	}
	return all;
}


QList<QNetworkCookie> PersistentCookieJar::getAllCookies() const
{
	QMutexLocker lock(&m_mutex);
	return allCookies();
}

QList<QNetworkCookie> PersistentCookieJar::cookiesForUrl(const QUrl &url) const
{
	QMutexLocker lock(&m_mutex);
	return QNetworkCookieJar::cookiesForUrl(url);
}

bool PersistentCookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
	QMutexLocker lock(&m_mutex);
	return QNetworkCookieJar::setCookiesFromUrl(cookieList, url);
}


void PersistentCookieJar::save()
{
	QMutexLocker lock(&m_mutex);
	QList<QNetworkCookie> list = allCookies();
	if (list.isEmpty()) {
		return;
	}

	QByteArray data;
	for (const QNetworkCookie &cookie : list) {
		if (!cookie.isSessionCookie()) {
			data.append(cookie.toRawForm());
			data.append("\n");
		}
	}

	if (!data.isEmpty()) {
		writeFile(m_filename, data);
	}
}

void PersistentCookieJar::load()
{
	QMutexLocker lock(&m_mutex);

	QFile f(m_filename);
	if (f.exists() && f.open(QFile::ReadOnly | QFile::Text)) {
		QByteArray data = f.readAll();
		f.close();

		setAllCookies(QNetworkCookie::parseCookies(data));
	}
}
