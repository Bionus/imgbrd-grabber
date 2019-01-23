#ifndef FILENAME_CONDITION_VISITOR_H
#define FILENAME_CONDITION_VISITOR_H

#include <QMap>
#include <QString>
#include <QStringList>
#include "filename/ast/filename-visitor.h"


struct FilenameNodeCondition;
class Token;

class FilenameConditionVisitor : public FilenameVisitor
{
	public:
		explicit FilenameConditionVisitor(QMap<QString, Token> tokens);
		bool run(const FilenameNodeCondition &node);

		void visit(const FilenameNodeConditionInvert &node) override;
		void visit(const FilenameNodeConditionOp &node) override;
		void visit(const FilenameNodeConditionTag &node) override;
		void visit(const FilenameNodeConditionToken &node) override;

	private:
		QMap<QString, Token> m_tokens;
		QStringList m_tags;

		bool m_result = false;
};

#endif // FILENAME_CONDITION_VISITOR_H
