#ifndef FILENAME_NODE_CONDITION_JAVASCRIPT_H
#define FILENAME_NODE_CONDITION_JAVASCRIPT_H

#include <QString>
#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionJavaScript : public FilenameNodeCondition
{
	QString script;

	explicit FilenameNodeConditionJavaScript(QString script);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_JAVASCRIPT_H
