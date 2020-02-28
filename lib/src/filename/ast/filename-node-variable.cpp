#include "filename/ast/filename-node-variable.h"
#include <utility>
#include "filename/ast/filename-visitor.h"


FilenameNodeVariable::FilenameNodeVariable(QString name, QMap<QString, QString> opts)
	: name(std::move(name)), opts(std::move(opts))
{}

void FilenameNodeVariable::accept(FilenameVisitor &v) const
{
	v.visit(*this);
}
