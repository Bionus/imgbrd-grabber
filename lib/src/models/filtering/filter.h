#ifndef FILTER_H
#define FILTER_H

#include <QMap>
#include <QString>
#include "loader/token.h"


class Filter
{
	public:
		explicit Filter(bool invert = false);
		virtual QString match(const QMap<QString, Token> &tokens, bool invert = false) const = 0;
		virtual QString toString() const = 0;

	protected:
		bool m_invert;
};

#endif // FILTER_H
