#ifndef FILENAME_NODE_CONDITION_TOKEN_H
#define FILENAME_NODE_CONDITION_TOKEN_H

#include <QString>
#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionToken : public FilenameNodeCondition
{
	QString token;

	explicit FilenameNodeConditionToken(QString token);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_TOKEN_H
