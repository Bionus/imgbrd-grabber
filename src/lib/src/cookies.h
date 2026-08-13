#ifndef COOKIES_H
#define COOKIES_H

#include <QList>
#include <QNetworkCookie>
#include <QString>


QList<QNetworkCookie> loadCookiesFromFile(const QString &path);

QList<QNetworkCookie> loadCookiesFromJson(const QString &json);
QList<QNetworkCookie> loadCookiesFromHeaders(const QString &txt);
QList<QNetworkCookie> loadCookiesFromTxt(const QString &txt);
QList<QNetworkCookie> loadCookiesFromNetscape(const QString &txt);

#endif // COOKIES_H
