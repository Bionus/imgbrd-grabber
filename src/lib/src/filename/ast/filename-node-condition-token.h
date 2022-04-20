#ifndef FILENAME_NODE_CONDITION_TOKEN_H
#define FILENAME_NODE_CONDITION_TOKEN_H

#include <QMap>
#include <QString>
#include "filename/ast/filename-node-condition.h"


struct FilenameNodeConditionToken : public FilenameNodeCondition
{
	QString name;
	QMap<QString, QString> opts;

	explicit FilenameNodeConditionToken(QString name, QMap<QString, QString> opts = {});
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_CONDITION_TOKEN_H
