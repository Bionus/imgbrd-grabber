#ifndef TAG_FILTER_H
#define TAG_FILTER_H

#include <QMap>
#include <QRegularExpression>
#include <QScopedPointer>
#include <QString>
#include "filter.h"


class TagFilter : public Filter
{
	public:
		explicit TagFilter(QString tag, bool invert = false);
		QString match(const QMap<QString, Token> &tokens, bool invert = false) const override;
		QString toString(bool escape = true) const override;
		bool compare(const Filter &rhs) const override;

	private:
		QString m_tag;
		QScopedPointer<QRegularExpression> m_regexp;
};

#endif // TAG_FILTER_H
