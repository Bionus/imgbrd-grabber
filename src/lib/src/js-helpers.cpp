#include "js-helpers.h"
#include <QFile>
#include <QJSEngine>
#include <QJSValue>
#include <QStringList>
#include "logger.h"
#include "models/api/javascript-console-helper.h"
#include "models/api/javascript-grabber-helper.h"


QJSEngine *buildJsEngine(const QString &helperFile)
{
    auto *engine = new QJSEngine();
    engine->globalObject().setProperty("Grabber", engine->newQObject(new JavascriptGrabberHelper(*engine)));
    engine->globalObject().setProperty("console", engine->newQObject(new JavascriptConsoleHelper("[JavaScript] ", engine)));

    // JavaScript helper file
	QFile jsHelper(helperFile);
	if (jsHelper.open(QFile::ReadOnly | QFile::Text)) {
		QJSValue helperResult = engine->evaluate(jsHelper.readAll(), jsHelper.fileName());
		jsHelper.close();

		if (helperResult.isError()) {
			log(QStringLiteral("Uncaught exception at line %1: %2").arg(helperResult.property("lineNumber").toInt()).arg(helperResult.toString()), Logger::Error);
		}
	} else {
		log(QStringLiteral("JavaScript helper file could not be opened"), Logger::Error);
	}

	return engine;
}

QStringList jsToStringList(const QJSValue &val)
{
	QStringList ret;

	const quint32 length = val.property("length").toUInt();
	for (quint32 i = 0; i < length; ++i) {
		ret.append(val.property(i).toString());
	}

	return ret;
}
