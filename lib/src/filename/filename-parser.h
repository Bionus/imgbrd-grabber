#ifndef FILENAME_PARSER_H
#define FILENAME_PARSER_H

#include <QString>


class FilenameNodeCondition;
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
		FilenameNodeConditionTag *parseConditionTag();
		FilenameNodeConditionToken *parseConditionToken();

	private:
		QString m_str;
		int m_index;
};

#endif // FILENAME_PARSER_H
