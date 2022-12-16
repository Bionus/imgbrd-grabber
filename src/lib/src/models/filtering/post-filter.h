#ifndef POST_FILTER_H
#define POST_FILTER_H

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>


struct FilenameNodeCondition;
class Filter;
class QString;
class Token;

class PostFilter
{
	public:
		explicit PostFilter(const QString &filters);
		explicit PostFilter(const QStringList &filters = QStringList());

		QStringList match(const QMap<QString, Token> &tokens) const;

	private:
		QList<QSharedPointer<Filter>> m_filters;
		FilenameNodeCondition *m_ast = nullptr;
};

#endif // POST_FILTER_H
