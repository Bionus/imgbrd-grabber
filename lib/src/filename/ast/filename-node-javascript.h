#ifndef FILENAME_NODE_JAVASCRIPT_H
#define FILENAME_NODE_JAVASCRIPT_H

#include <QString>
#include "filename/ast/filename-node.h"


struct FilenameNodeJavaScript : public FilenameNode
{
	QString script;

	explicit FilenameNodeJavaScript(QString script);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_JAVASCRIPT_H
