#ifndef TAG_FILTER_H
#define TAG_FILTER_H

#include "filter.h"


class TagFilter : public Filter
{
	public:
		explicit TagFilter(QString tag, bool invert = false);
		QString match(const QMap<QString, Token> &tokens, bool invert = false) const override;
		QString toString() const override;

	private:
		QString m_tag;
};

#endif // TAG_FILTER_H
