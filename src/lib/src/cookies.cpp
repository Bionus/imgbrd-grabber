#include "cookies.h"
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTimeZone>

#include "logger.h"


QList<QNetworkCookie> loadCookiesFromFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		log(QStringLiteral("Error opening cookie file `%1`.").arg(path));
		return {};
	}

	const QString contents = file.readAll();
	file.close();

	// JSON files are always expected to use the JSON loader
	if (path.endsWith(".json")) {
		return loadCookiesFromJson(contents);
	}

	// TXT files have two available parsers
	if (path.endsWith(".txt")) {
		// Netscape files use tabs as separators
		if (contents.count('\t') > contents.count(';')) {
			return loadCookiesFromNetscape(contents);
		}

		// Set-Cookie header format
		if (contents.contains("Domain=") || contents.contains("Expires=") || contents.contains("Path=")) {
			// return loadCookiesFromHeader(contents);
		}

		// Basic key=value format with semicolon separator
		return loadCookiesFromTxt(contents);
	}

	log(QStringLiteral("Unknown cookie file extension for `%1`.").arg(path));
	return {};
}

QList<QNetworkCookie> loadCookiesFromJson(const QString &json)
{
	const QJsonDocument jsonDoc = QJsonDocument::fromJson(json.toUtf8());
	if (jsonDoc.isNull() || !jsonDoc.isArray()) {
		return {};
	}

	QList<QNetworkCookie> cookies;

	const QJsonArray entries = jsonDoc.array();
	for (auto entry : entries) {
		if (!entry.isObject()) {
			continue;
		}

		const QJsonObject cookieObj = entry.toObject();

		QNetworkCookie cookie;
		cookie.setName(cookieObj.value("name").toString().toLatin1());
		cookie.setValue(cookieObj.value("value").toString().toLatin1());
		if (cookieObj.contains("domain")) {
			cookie.setDomain(cookieObj.value("domain").toString());
		}
		if (cookieObj.contains("path")) {
			cookie.setPath(cookieObj.value("path").toString());
		}
		if (cookieObj.contains("secure")) {
			cookie.setSecure(cookieObj.value("secure").toBool());
		}
		if (cookieObj.contains("expirationDate")) {
			cookie.setExpirationDate(QDateTime::fromSecsSinceEpoch(cookieObj.value("expirationDate").toDouble(), QTimeZone::UTC));
		}

		cookies.append(cookie);
	}

	return cookies;
}

QList<QNetworkCookie> loadCookiesFromHeaders(const QString &txt)
{
	return QNetworkCookie::parseCookies(txt.toLatin1());
}

QList<QNetworkCookie> loadCookiesFromTxt(const QString &txt)
{
	static const QRegularExpression separator("[;\\n]");

	QList<QNetworkCookie> cookies;

	const QStringList pairs = txt.split(separator, Qt::SkipEmptyParts);
	for (const QString &pair : pairs) {
		const QStringList parts = pair.trimmed().split('=');
		if (parts.count() == 2) {
			cookies.append(QNetworkCookie(parts[0].toLatin1(), parts[1].toLatin1()));
		}
	}

	return cookies;
}

QList<QNetworkCookie> loadCookiesFromNetscape(const QString &txt)
{
	QList<QNetworkCookie> cookies;

	for (QString line : txt.split('\n', Qt::SkipEmptyParts)) {
		line = line.trimmed();

		// Remove "#HttpOnly_" prefix
		if (line.startsWith("#HttpOnly_")) {
			line = line.mid(10);
		}

		// Ignore empty lines and comments
		if (line.isEmpty() || line.startsWith('#') || line.startsWith('$')) {
			continue;
		}

		// Split the line into cookie parts
		const QStringList parts = line.split('\t');

		// Create the cookie object
		QNetworkCookie cookie;
		cookie.setDomain(parts[0]);
		cookie.setPath(parts[2]);
		cookie.setSecure(parts[3] == "TRUE");
		if (!parts[4].isEmpty() && parts[4] != "0") {
			cookie.setExpirationDate(QDateTime::fromSecsSinceEpoch(parts[4].toLongLong(), QTimeZone::UTC));
		}
		if (!parts[5].isEmpty()) {
			cookie.setName(parts[5].toLatin1());
			cookie.setValue(parts[6].toLatin1());
		} else {
			cookie.setName(parts[6].toLatin1());
		}

		cookies.append(cookie);
	}

	return cookies;
}
