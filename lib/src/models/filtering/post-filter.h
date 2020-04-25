#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>


class Filter;
class QString;
class Token;

class PostFilter
{
	public:
		explicit PostFilter(const QStringList &filters = QStringList());
		int count() const;
		QStringList match(const QMap<QString, Token> &tokens) const;

	private:
		QList<QSharedPointer<Filter>> m_filters;
};

#endif // POST_FILTER_H
