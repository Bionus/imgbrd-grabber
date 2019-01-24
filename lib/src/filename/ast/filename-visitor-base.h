#ifndef FILENAME_VISITOR_BASE_H
#define FILENAME_VISITOR_BASE_H

#include "filename/ast/filename-visitor.h"


class FilenameVisitorBase : public FilenameVisitor
{
	public:
		void visit(const FilenameNodeConditional &node) override;
		void visit(const FilenameNodeConditionInvert &node) override;
		void visit(const FilenameNodeConditionOp &node) override;
		void visit(const FilenameNodeConditionTag &node) override;
		void visit(const FilenameNodeConditionToken &node) override;
		void visit(const FilenameNodeRoot &node) override;
		void visit(const FilenameNodeText &node) override;
		void visit(const FilenameNodeVariable &node) override;
};

#endif // FILENAME_VISITOR_BASE_H
