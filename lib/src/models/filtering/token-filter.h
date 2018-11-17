#ifndef TOKEN_FILTER_H
#define TOKEN_FILTER_H

#include <QMap>
#include <QString>
#include "filter.h"


class Token;

class TokenFilter : public Filter
{
	public:
		explicit TokenFilter(QString token, bool invert = false);
		QString match(const QMap<QString, Token> &tokens, bool invert = false) const override;
		QString toString() const override;
		bool compare(const Filter &rhs) const override;

	private:
		QString m_token;
};

#endif // TOKEN_FILTER_H
