#include "filename/ast/filename-node-condition-token.h"
#include <utility>
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionToken::FilenameNodeConditionToken(QString token)
	: token(std::move(token))
{}

void FilenameNodeConditionToken::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
