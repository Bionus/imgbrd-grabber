#include "filename/filename-execution-visitor.h"
#include <QDateTime>
#include <QVariant>
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"
#include "filename/filename-condition-visitor.h"
#include "loader/token.h"


FilenameExecutionVisitor::FilenameExecutionVisitor(QMap<QString, Token> tokens)
    : m_tokens(std::move(tokens))
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


void FilenameExecutionVisitor::visitVariable(const QString &name, QMap<QString, QString> options)
{
	Q_UNUSED(options);

	if (!m_tokens.contains(name)) {
        return;
    }

	QVariant val = m_tokens[name].value();
	QString res;

	if (val.type() == QVariant::DateTime) {
		res = val.toDateTime().toString("MM-dd-yyyy HH.mm");
	} else if (val.type() == QVariant::Int) {
		res = QString::number(val.toInt());
	} else if (val.type() == QVariant::StringList) {
		QStringList vals = val.toStringList();
		res = val.toStringList().join(' ');
	} else {
		res = val.toString();
	}

	m_result += res;
}
