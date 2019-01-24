#ifndef FILENAME_PARSER_H
#define FILENAME_PARSER_H

#include <QString>


class FilenameNodeCondition;
class FilenameNodeConditionInvert;
class FilenameNodeConditionTag;
class FilenameNodeConditionToken;

class FilenameParser
{
	public:
		explicit FilenameParser(QString str);

		FilenameNodeCondition *parseCondition();

	protected:
		QChar peek();
		bool finished();

		FilenameNodeCondition *parseSingleCondition();
		FilenameNodeConditionInvert *parseConditionInvert();
		FilenameNodeConditionTag *parseConditionTag();
		FilenameNodeConditionToken *parseConditionToken();

	private:
		QString m_str;
		int m_index;
};

#endif // FILENAME_PARSER_H
