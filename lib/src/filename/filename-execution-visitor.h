#ifndef FILENAME_EXECUTION_VISITOR_H
#define FILENAME_EXECUTION_VISITOR_H

#include <QMap>
#include <QString>
#include "filename/ast/filename-visitor-base.h"


class QDateTime;
class QStringList;
class Token;

class FilenameExecutionVisitor : public FilenameVisitorBase
{
	public:
		explicit FilenameExecutionVisitor(const QMap<QString, Token> &tokens);
		QString run(const FilenameNodeRoot &node);

		void visit(const FilenameNodeConditional &node) override;
		void visit(const FilenameNodeConditionTag &node) override;
		void visit(const FilenameNodeConditionToken &node) override;
		void visit(const FilenameNodeText &node) override;
		void visit(const FilenameNodeVariable &node) override;

	protected:
		void visitVariable(const QString &name, const QMap<QString, QString> &options = {});
		QString variableToString(const QDateTime &val, const QMap<QString, QString> &options);
		QString variableToString(int val, const QMap<QString, QString> &options);
		QString variableToString(QStringList val, const QMap<QString, QString> &options);
		QString cleanVariable(QString val, const QMap<QString, QString> &options);

	private:
		const QMap<QString, Token> &m_tokens;

		QString m_result;
};

#endif // FILENAME_EXECUTION_VISITOR_H
