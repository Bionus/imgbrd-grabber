#include "filename/ast/filename-node-condition-token.h"
#include <utility>
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionToken::FilenameNodeConditionToken(QString name, QMap<QString, QString> opts)
	: name(std::move(name)), opts(std::move(opts))
{}

void FilenameNodeConditionToken::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
