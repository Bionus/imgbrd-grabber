#ifndef FILENAME_NODE_TEXT_H
#define FILENAME_NODE_TEXT_H

#include <QString>
#include "filename/ast/filename-node.h"


struct FilenameNodeText : public FilenameNode
{
	QString text;

	explicit FilenameNodeText(QString text);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_TEXT_H
