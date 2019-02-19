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
		const QString &error() const;

		FilenameNodeRoot *parseRoot();
		FilenameNodeCondition *parseCondition();
		FilenameNodeVariable *parseVariable();

	protected:
		QChar peek();
		bool finished();
		void skipSpaces();
		int indexOf(const QList<QChar> &chars, int max = -1);
		QString readUntil(const QList<QChar> &chars, bool allowEnd = false);

		FilenameNodeRoot *parseRootNode();
		FilenameNode *parseExpr(const QList<QChar> &addChars = {});
		FilenameNodeConditional *parseConditional();
		FilenameNodeCondition *parseConditionNode();
		FilenameNodeCondition *parseSingleCondition();
		FilenameNodeConditionInvert *parseConditionInvert();
		FilenameNodeConditionTag *parseConditionTag();
		FilenameNodeConditionToken *parseConditionToken();

	private:
		QString m_str;
		int m_index;
		QString m_error;
};

#endif // FILENAME_PARSER_H
