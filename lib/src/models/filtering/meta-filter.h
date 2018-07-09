#ifndef META_FILTER_H
#define META_FILTER_H

#include "filter.h"


class MetaFilter : public Filter
{
	public:
		MetaFilter(QString type, QString val, bool invert = false);
		QString match(const QMap<QString, Token> &tokens, bool invert = false) const override;
		QString toString() const override;

	private:
		QString m_type;
		QString m_val;
};

#endif // META_FILTER_H
