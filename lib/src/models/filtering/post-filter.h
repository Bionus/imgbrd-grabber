#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <QStringList>


class Token;

class PostFilter
{
	public:
		static QStringList filter(const QMap<QString, Token> &tokens, const QStringList &filters);
		static QString match(const QMap<QString, Token> &tokens, QString filter, bool invert = false);
};

#endif // POST_FILTER_H
