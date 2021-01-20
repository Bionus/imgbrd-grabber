#include "filename/filename-execution-visitor.h"
#include <QDateTime>
#include <QJSEngine>
#include <QJSValue>
#include <QSet>
#include <QSettings>
#include <QSharedPointer>
#include <QStringList>
#include <QVariant>
#include <algorithm>
#include "filename/ast/filename-node-condition-ignore.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-javascript.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"
#include "filename/filename-condition-visitor.h"
#include "loader/token.h"
#include "logger.h"
#include "models/image.h"


FilenameExecutionVisitor::FilenameExecutionVisitor(const QMap<QString, Token> &tokens, QSettings *settings)
	: FilenameVisitorJavaScript(settings), m_tokens(tokens), m_settings(settings)
{}

void FilenameExecutionVisitor::setEscapeMethod(QString (*escapeMethod)(const QVariant &))
{
	m_escapeMethod = escapeMethod;
}

void FilenameExecutionVisitor::setKeepInvalidTokens(bool keepInvalidTokens)
{
	m_keepInvalidTokens = keepInvalidTokens;
}


QString FilenameExecutionVisitor::run(const FilenameNodeRoot &node)
{
	m_result.clear();

	node.accept(*this);

	return m_result;
}


void FilenameExecutionVisitor::visit(const FilenameNodeConditional &node)
{
	FilenameConditionVisitor conditionVisitor(m_tokens, m_settings);
	bool valid = conditionVisitor.run(*node.condition);

	if (valid && node.ifTrue != nullptr) {
		node.ifTrue->accept(*this);
	} else if (!valid && node.ifFalse != nullptr) {
		node.ifFalse->accept(*this);
	}
}

void FilenameExecutionVisitor::visit(const FilenameNodeConditionIgnore &node)
{
	Q_UNUSED(node);

	// No-op
}

void FilenameExecutionVisitor::visit(const FilenameNodeConditionTag &node)
{
	m_result += cleanVariable(node.tag.text());
}

void FilenameExecutionVisitor::visit(const FilenameNodeConditionToken &node)
{
	visitVariable(node.token);
}

void FilenameExecutionVisitor::visit(const FilenameNodeJavaScript &node)
{
	QJSEngine engine;
	setJavaScriptVariables(engine, m_tokens, engine.globalObject());

	QJSValue result = engine.evaluate(node.script);
	if (result.isError()) {
		log("Error in Javascript evaluation:<br/>" + result.toString());
		return;
	}

	m_result += result.toString();
}

void FilenameExecutionVisitor::visit(const FilenameNodeText &node)
{
	m_result += node.text;
}

void FilenameExecutionVisitor::visit(const FilenameNodeVariable &node)
{
	visitVariable(node.name, node.opts);
}


void FilenameExecutionVisitor::visitVariable(const QString &fullName, const QMap<QString, QString> &options)
{
	// Contexts "obj.var"
	bool found = true;
	QStringList var = fullName.split('.');
	QString name = var.takeFirst();
	QMap<QString, Token> context = m_tokens;
	while (found && !var.isEmpty()) {
		if (context.contains(name)) {
			const QVariant &val = context[name].value();
			if (val.canConvert<QMap<QString, Token>>()) {
				context = val.value<QMap<QString, Token>>();
				name = var.takeFirst();
				continue;
			}
			break;
		}
		found = false;
	}

	// Variable not found
	if (!found || !context.contains(name)) {
		static const QSet<QString> keptTokens { "path", "num" };
		if (m_keepInvalidTokens || keptTokens.contains(name)) {
			QString strOpts;
			for (auto it = options.constBegin(); it != options.constEnd(); ++it) {
				strOpts += it.key();
				if (!it.value().isEmpty()) {
					strOpts += "=" + it.value();
				}
			}
			m_result += "%" + name + (!strOpts.isEmpty() ? ":" + strOpts : "") + "%"; // FIXME: get original value/proper toString()
		}
		return;
	}

	QVariant val = context[name].value();
	QString res;
	bool clean = false;

	// Convert value to a basic string using the given options
	if (val.type() == QVariant::DateTime) {
		res = variableToString(name, val.toDateTime(), options);
	} else if (val.type() == QVariant::Int) {
		res = variableToString(name, val.toInt(), options);
	} else if (val.type() == QVariant::StringList) {
		res = variableToString(name, val.toStringList(), options);
		clean = true;
	} else {
		res = val.toString();
	}

	// String options
	if (options.contains("maxlength")) {
		res = res.left(options["maxlength"].toInt());
	}
	if (options.contains("htmlescape")) {
		res = res.toHtmlEscaped();
	}

	// Forbidden characters and spaces replacement settings
	if (name != "allo" && !name.startsWith("url_") && name != "filename" && name != "directory" && !clean) {
		res = cleanVariable(res, options);
	}

	// Escape if necessary
	if (m_escapeMethod != nullptr && options.contains("escape")) {
		res = m_escapeMethod(res);
	}

	m_result += res;
}

QString FilenameExecutionVisitor::variableToString(const QString &name, QDateTime val, const QMap<QString, QString> &options)
{
	Q_UNUSED(name);

	const QString timeZone = options.value("timezone", "");
	if (!timeZone.isEmpty() && timeZone != QLatin1String("server")) {
		if (timeZone == QLatin1String("local")) {
			val = val.toLocalTime();
		} else {
			QTimeZone tz(timeZone.toLatin1());
			if (tz.isValid()) {
				val = val.toTimeZone(tz);
			} else {
				log(QString("Unknown timeZone '%1'").arg(timeZone), Logger::Error);
			}
		}
	}

	const QString format = options.value("format", "MM-dd-yyyy HH.mm");
	return val.toString(format);
}

QString FilenameExecutionVisitor::variableToString(const QString &name, int val, const QMap<QString, QString> &options)
{
	Q_UNUSED(name);

	return options.contains("length")
		? QString("%1").arg(val, options["length"].toInt(), 10, QChar('0'))
		: QString::number(val);
}

QString FilenameExecutionVisitor::variableToString(const QString &name, QStringList val, const QMap<QString, QString> &options)
{
	// Count
	if (options.contains("count")) {
		return variableToString(name, val.count(), options);
	}

	// Namespaces
	bool ignoreNamespace = options.contains("ignorenamespace");
	bool includeNamespace = options.contains("includenamespace");
	if (ignoreNamespace || includeNamespace) {
		QStringList namespaces = m_tokens["all_namespaces"].value().toStringList();

		if (options.contains("ignorenamespace")) {
			QStringList ignored = options["ignorenamespace"].split(' ');
			QStringList filtered, filteredNamespaces;
			for (int i = 0; i < val.count(); ++i) {
				const QString &nspace = name == "all" ? namespaces[i] : name;
				if (!ignored.contains(nspace)) {
					filtered.append(val[i]);
					filteredNamespaces.append(namespaces[i]);
				}
			}
			val = filtered;
			namespaces = filteredNamespaces;
		}
		if (options.contains("includenamespace")) {
			QStringList excluded;
			if (options.contains("excludenamespace")) {
				excluded = options["excludenamespace"].split(' ');
			}

			QStringList namespaced;
			for (int i = 0; i < val.count(); ++i) {
				const QString nspace = name == "all" ? namespaces[i] : name;
				namespaced.append((!excluded.contains(nspace) ? nspace + ":" : QString()) + val[i]);
			}
			val = namespaced;
		}
	}

	if (options.contains("sort")) {
		std::sort(val.begin(), val.end());
	}

	// Clean each value separately
	if (!name.startsWith("source")) {
		for (QString &t : val) {
			t = cleanVariable(t, options);
		}
	}

	// Separator
	QString mainSeparator = m_settings->value("Save/separator", " ").toString();
	QString tagSeparator = m_settings->value("Save/" + name + "_sep", mainSeparator).toString();
	QString separator = options.value("separator", tagSeparator);
	separator.replace("\\n", "\n").replace("\\r", "\r");

	return val.join(separator);
}


QString FilenameExecutionVisitor::cleanVariable(QString res, const QMap<QString, QString> &options) const
{
	// Forbidden characters
	if (!options.contains("unsafe")) {
		res = res.replace("\\", "_").replace("%", "_").replace("/", "_").replace(":", "_").replace("|", "_").replace("*", "_").replace("?", "_").replace("\"", "_").replace("<", "_").replace(">", "_").replace("__", "_").replace("__", "_").replace("__", "_").trimmed();
	}

	// Replace underscores by spaces
	if (!options.contains("underscores") && (!m_settings->value("Save/replaceblanks", false).toBool() || options.contains("spaces"))) {
		res = res.replace("_", " ");
	}

	return res;
}
