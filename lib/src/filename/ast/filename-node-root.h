#ifndef FILENAME_NODE_ROOT_H
#define FILENAME_NODE_ROOT_H

#include <QList>
#include "filename/ast/filename-node.h"


struct FilenameNodeRoot : public FilenameNode
{
	QList<FilenameNode*> exprs;

	explicit FilenameNodeRoot(QList<FilenameNode*> exprs);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_ROOT_H
