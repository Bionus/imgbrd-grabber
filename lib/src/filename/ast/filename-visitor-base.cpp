#include "filename/ast/filename-visitor-base.h"
#include <QJSEngine>
#include <QJSValue>
#include <QVariant>
#include "filename/ast/filename-node-condition.h"
#include "filename/ast/filename-node-conditional.h"
#include "filename/ast/filename-node-condition-ignore.h"
#include "filename/ast/filename-node-condition-invert.h"
#include "filename/ast/filename-node-condition-javascript.h"
#include "filename/ast/filename-node-condition-op.h"
#include "filename/ast/filename-node-condition-tag.h"
#include "filename/ast/filename-node-condition-token.h"
#include "filename/ast/filename-node-javascript.h"
#include "filename/ast/filename-node-root.h"
#include "filename/ast/filename-node-text.h"
#include "filename/ast/filename-node-variable.h"
#include "loader/token.h"


void FilenameVisitorBase::visit(const FilenameNodeConditional &node)
{
	node.condition->accept(*this);
	node.ifTrue->accept(*this);

	if (node.ifFalse != nullptr) {
		node.ifFalse->accept(*this);
	}
}

void FilenameVisitorBase::visit(const FilenameNodeConditionIgnore &node)
{
	node.node->accept(*this);
}

void FilenameVisitorBase::visit(const FilenameNodeConditionInvert &node)
{
	node.node->accept(*this);
}

void FilenameVisitorBase::visit(const FilenameNodeConditionJavaScript &node)
{
	Q_UNUSED(node); // No-op
}

void FilenameVisitorBase::visit(const FilenameNodeConditionOp &node)
{
	node.left->accept(*this);
	node.right->accept(*this);
}

void FilenameVisitorBase::visit(const FilenameNodeConditionTag &node)
{
	Q_UNUSED(node); // No-op
}

void FilenameVisitorBase::visit(const FilenameNodeConditionToken &node)
{
	Q_UNUSED(node); // No-op
}

void FilenameVisitorBase::visit(const FilenameNodeJavaScript &node)
{
	Q_UNUSED(node); // No-op
}

void FilenameVisitorBase::visit(const FilenameNodeRoot &node)
{
	for (auto expr : node.exprs) {
		expr->accept(*this);
	}
}

void FilenameVisitorBase::visit(const FilenameNodeText &node)
{
	Q_UNUSED(node); // No-op
}

void FilenameVisitorBase::visit(const FilenameNodeVariable &node)
{
	Q_UNUSED(node); // No-op
}
