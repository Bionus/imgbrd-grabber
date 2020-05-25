#ifndef FILENAME_PRINT_VISITOR_H
#define FILENAME_PRINT_VISITOR_H

#include <QString>
#include "filename/ast/filename-visitor-base.h"


class FilenamePrintVisitor : public FilenameVisitorBase
{
	public:
		QString run(const FilenameNodeRoot &node);

		void visit(const FilenameNodeConditional &node) override;
		void visit(const FilenameNodeConditionIgnore &node) override;
		void visit(const FilenameNodeConditionInvert &node) override;
		void visit(const FilenameNodeConditionJavaScript &node) override;
		void visit(const FilenameNodeConditionOp &node) override;
		void visit(const FilenameNodeConditionTag &node) override;
		void visit(const FilenameNodeConditionToken &node) override;
		void visit(const FilenameNodeJavaScript &node) override;
		void visit(const FilenameNodeRoot &node) override;
		void visit(const FilenameNodeText &node) override;
		void visit(const FilenameNodeVariable &node) override;

	private:
		QString m_result;
};

#endif // FILENAME_PRINT_VISITOR_H
