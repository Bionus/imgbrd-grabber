#ifndef FILENAME_PARSER_H
#define FILENAME_PARSER_H

#include <QChar>
#include <QList>
#include <QString>


struct FilenameNode;
struct FilenameNodeCondition;
struct FilenameNodeConditional;
struct FilenameNodeConditionInvert;
struct FilenameNodeConditionTag;
struct FilenameNodeConditionToken;
struct FilenameNodeRoot;
struct FilenameNodeVariable;

class FilenameParser
{
	public:
		explicit FilenameParser(QString str);

		FilenameNodeRoot *parseRoot();
		FilenameNodeCondition *parseCondition();

	protected:
		QChar peek();
		bool finished();
		void skipSpaces();
		int indexOf(const QList<QChar> &chars, int max = -1);
		QString readUntil(const QList<QChar> &chars, bool allowEnd = false);

		FilenameNode *parseExpr(const QList<QChar> &addChars = {});
		FilenameNodeVariable *parseVariable();
		FilenameNodeConditional *parseConditional();
		FilenameNodeCondition *parseSingleCondition();
		FilenameNodeConditionInvert *parseConditionInvert();
		FilenameNodeConditionTag *parseConditionTag();
		FilenameNodeConditionToken *parseConditionToken();

	private:
		QString m_str;
		int m_index;
};

#endif // FILENAME_PARSER_H
