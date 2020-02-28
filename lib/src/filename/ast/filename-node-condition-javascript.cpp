#include "filename/ast/filename-node-condition-javascript.h"
#include <utility>
#include "filename/ast/filename-visitor.h"


FilenameNodeConditionJavaScript::FilenameNodeConditionJavaScript(QString script)
	: script(std::move(script))
{}

void FilenameNodeConditionJavaScript::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
