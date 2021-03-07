#ifndef JS_HELPERS_H
#define JS_HELPERS_H

#include <QString>
#include <QStringList>

class QJSEngine;
class QJSValue;


QJSEngine *buildJsEngine(const QString &helperFile);
QStringList jsToStringList(const QJSValue &val);

#endif // JS_HELPERS_H
