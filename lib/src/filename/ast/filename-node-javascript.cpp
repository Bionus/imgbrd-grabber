#include "filename/ast/filename-node-javascript.h"
#include <utility>
#include "filename/ast/filename-visitor.h"


FilenameNodeJavaScript::FilenameNodeJavaScript(QString script)
	: script(std::move(script))
{}

void FilenameNodeJavaScript::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
