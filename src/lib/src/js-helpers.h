#ifndef JS_HELPERS_H
#define JS_HELPERS_H

#include <QString>
#include <QStringList>

class QJSEngine;
class QJSValue;
class QUrl;


QJSEngine *buildJsEngine(const QString &helperFile);

void getProperty(const QJSValue &val, const QString &key, int &out);
void getProperty(const QJSValue &val, const QString &key, double &out);
void getProperty(const QJSValue &val, const QString &key, QString &out);
void getProperty(const QJSValue &val, const QString &key, QUrl &out);
void getProperty(const QJSValue &val, const QString &key, bool &out);
void getProperty(const QJSValue &val, const QString &key, QStringList &out);

template <class T>
T getPropertyOr(const QJSValue &val, const QString &key, T def)
{
	getProperty(val, key, def);
	return def;
}

QStringList jsToStringList(const QJSValue &val);
QVariantMap jsToMap(const QJSValue &val);

#endif // JS_HELPERS_H
