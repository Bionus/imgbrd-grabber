#ifndef FILENAME_NODE_VARIABLE_H
#define FILENAME_NODE_VARIABLE_H

#include <QMap>
#include <QString>
#include "filename/ast/filename-node.h"


struct FilenameNodeVariable : public FilenameNode
{
	QString name;
	QMap<QString, QString> opts;

	explicit FilenameNodeVariable(QString name, QMap<QString, QString> opts);
	void accept(FilenameVisitor &v) const override;
};

#endif // FILENAME_NODE_VARIABLE_H
