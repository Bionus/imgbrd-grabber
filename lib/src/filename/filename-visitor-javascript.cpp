#include "filename/filename-visitor-javascript.h"
#include <QJSEngine>
#include <QJSValue>
#include <QSettings>
#include <QVariant>
#include "loader/token.h"
#include "models/image.h"
#include "models/profile.h"


FilenameVisitorJavaScript::FilenameVisitorJavaScript(QSettings *settings)
	: m_settings(settings)
{}


void FilenameVisitorJavaScript::setJavaScriptVariables(QJSEngine &engine, const QMap<QString, Token> &tokens, QJSValue obj) const
{
	for (auto it = tokens.constBegin(); it != tokens.constEnd(); ++it) {
		const QString &name = it.key();
		QVariant val = it.value().value();

		if (val.type() == QVariant::StringList || val.type() == QVariant::String) {
			QString res;

			if (val.type() == QVariant::StringList) {
				QStringList vals = val.toStringList();
				if (name != "all" && name != "tags") {
					obj.setProperty(name + "s", engine.toScriptValue(vals));
				}
				res = vals.join(separator(name, QString()));
			} else {
				res = val.toString();
			}

			if (name != "allo") {
				res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
				if (!m_settings->value("Save/replaceblanks", false).toBool()) {
					res.replace("_", " ");
				}
			}

			obj.setProperty(name, res);
		} else if (val.canConvert<QMap<QString, Token>>()) {
			QJSValue v = engine.newObject();
			QMap<QString, Token> subTokens = val.value<QMap<QString, Token>>();
			setJavaScriptVariables(engine, subTokens, v);
			obj.setProperty(name, v);
		} else {
			obj.setProperty(name, engine.toScriptValue(val));
		}
	}
}

QString FilenameVisitorJavaScript::separator(const QString &key, const QString &override) const
{
	QString separator;

	if (!override.isEmpty()) {
		separator = override;
	} else {
		QString mainSeparator = m_settings->value("Save/separator", " ").toString();
		separator = m_settings->value("Save/" + key + "_sep", mainSeparator).toString();
	}

	separator.replace("\\n", "\n").replace("\\r", "\r");
	return separator;
}
