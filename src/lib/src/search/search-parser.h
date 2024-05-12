#ifndef SEARCH_PARSER_H
#define SEARCH_PARSER_H

#include <QList>
#include <QString>
#include "search/search-format.h"
#include "tags/tag-name-format.h"


class QChar;
struct SearchNode;
struct SearchNodeOp;
struct SearchNodeTag;

class SearchParser
{
	public:
		explicit SearchParser(QString str);
		SearchParser(QString str, SearchFormat searchFormat, TagNameFormat tagNameFormat);

		// Getters
		const QString &error() const;
		const QString &str() const;

		SearchNode *parse();

	protected:
		SearchNodeTag *makeTagNode(const QString &tag) const;

	private:
		QString m_str;
		SearchFormat m_searchFormat;
		TagNameFormat m_tagNameFormat;
		QString m_error;
};

#endif // SEARCH_PARSER_H
