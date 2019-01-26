#include "filename/filename-execution-visitor.h"
#include <algorithm>
#include <QDateTime>
#include <QStringList>
#include <QVariant>
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"
#include "filename/filename-condition-visitor.h"
#include "loader/token.h"


FilenameExecutionVisitor::FilenameExecutionVisitor(const QMap<QString, Token> &tokens)
	: m_tokens(tokens)
{}

QString FilenameExecutionVisitor::run(const FilenameNodeRoot &node)
{
	m_result.clear();

	node.accept(*this);

	return m_result;
}


void FilenameExecutionVisitor::visit(const FilenameNodeConditional &node)
{
	FilenameConditionVisitor conditionVisitor(m_tokens);
	bool valid = conditionVisitor.run(*node.condition);

	if (valid && node.ifTrue != nullptr) {
		node.ifTrue->accept(*this);
	} else if (!valid && node.ifFalse != nullptr) {
		node.ifFalse->accept(*this);
	}
}

void FilenameExecutionVisitor::visit(const FilenameNodeConditionTag &node)
{
	m_result += node.tag.text();
}

void FilenameExecutionVisitor::visit(const FilenameNodeConditionToken &node)
{
	visitVariable(node.token);
}

void FilenameExecutionVisitor::visit(const FilenameNodeText &node)
{
	m_result += node.text;
}

void FilenameExecutionVisitor::visit(const FilenameNodeVariable &node)
{
	visitVariable(node.name, node.opts);
}


void FilenameExecutionVisitor::visitVariable(const QString &name, const QMap<QString, QString> &options)
{
	if (!m_tokens.contains(name)) {
		return;
	}

	QVariant val = m_tokens[name].value();
	QString res;

	// Convert value to a basic string using the given options
	if (val.type() == QVariant::DateTime) {
		res = variableToString(val.toDateTime(), options);
	} else if (val.type() == QVariant::Int) {
		res = variableToString(val.toInt(), options);
	} else if (val.type() == QVariant::StringList) {
		res = variableToString(val.toStringList(), options);
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

	m_result += res;
}

QString FilenameExecutionVisitor::variableToString(const QDateTime &val, const QMap<QString, QString> &options)
{
	const QString format = options.value("format", "MM-dd-yyyy HH.mm");
	return val.toString(format);
}

QString FilenameExecutionVisitor::variableToString(int val, const QMap<QString, QString> &options)
{
	return options.contains("length")
		? QString("%1").arg(val, options["length"].toInt(), 10, QChar('0'))
		: QString::number(val);
}

QString FilenameExecutionVisitor::variableToString(QStringList val, const QMap<QString, QString> &options)
{
	if (options.contains("sort")) {
		std::sort(val.begin(), val.end());
	}

	// Separator
	QString separator = options.value("separator", " ");
	separator.replace("\\n", "\n").replace("\\r", "\r");

	return val.join(separator);
}
