#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <QList>
#include <QStringList>


class Filter;
class Token;

class PostFilter
{
	public:
		explicit PostFilter(const QStringList &filters = QStringList());
		~PostFilter();
		int count() const;
		QStringList match(const QMap<QString, Token> &tokens) const;

	private:
		QList<Filter*> m_filters;
};

#endif // POST_FILTER_H
