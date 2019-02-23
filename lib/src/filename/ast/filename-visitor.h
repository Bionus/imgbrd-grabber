#ifndef FILENAME_VISITOR_H
#define FILENAME_VISITOR_H

struct FilenameNodeConditional;
struct FilenameNodeConditionIgnore;
struct FilenameNodeConditionInvert;
struct FilenameNodeConditionJavaScript;
struct FilenameNodeConditionOp;
struct FilenameNodeConditionTag;
struct FilenameNodeConditionToken;
struct FilenameNodeJavaScript;
struct FilenameNodeRoot;
struct FilenameNodeText;
struct FilenameNodeVariable;


class FilenameVisitor
{
	public:
		virtual ~FilenameVisitor() = default;

		virtual void visit(const FilenameNodeConditional &node) = 0;
		virtual void visit(const FilenameNodeConditionIgnore &node) = 0;
		virtual void visit(const FilenameNodeConditionInvert &node) = 0;
		virtual void visit(const FilenameNodeConditionJavaScript &node) = 0;
		virtual void visit(const FilenameNodeConditionOp &node) = 0;
		virtual void visit(const FilenameNodeConditionTag &node) = 0;
		virtual void visit(const FilenameNodeConditionToken &node) = 0;
		virtual void visit(const FilenameNodeJavaScript &node) = 0;
		virtual void visit(const FilenameNodeRoot &node) = 0;
		virtual void visit(const FilenameNodeText &node) = 0;
		virtual void visit(const FilenameNodeVariable &node) = 0;
};

#endif // FILENAME_VISITOR_H
