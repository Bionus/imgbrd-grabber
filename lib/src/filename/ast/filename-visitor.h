#ifndef FILENAME_VISITOR_H
#define FILENAME_VISITOR_H

struct FilenameNodeConditionInvert;
struct FilenameNodeConditionOp;
struct FilenameNodeConditionTag;
struct FilenameNodeConditionToken;


class FilenameVisitor
{
	public:
		virtual ~FilenameVisitor() = default;

		virtual void visit(const FilenameNodeConditionInvert &node) = 0;
		virtual void visit(const FilenameNodeConditionOp &node) = 0;
		virtual void visit(const FilenameNodeConditionTag &node) = 0;
		virtual void visit(const FilenameNodeConditionToken &node) = 0;
};

#endif // FILENAME_VISITOR_H
