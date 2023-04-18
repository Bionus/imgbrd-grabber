#ifndef BLACKLIST_H
#define BLACKLIST_H

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QStringList>


class Filter;
class QString;
class Token;

class Blacklist
{
	public:
		Blacklist() = default;
		explicit Blacklist(const QStringList &tags);

		bool isEmpty() const;
		bool contains(const QString &tag) const;
		void add(const QString &tag);
		void add(const QStringList &tags);
		bool remove(const QString &tag);

		QString toString() const;
		QStringList match(const QMap<QString, Token> &tokens, bool invert = true) const;

	protected:
		int indexOf(const QString &tag) const;

	private:
		QList<QList<QSharedPointer<Filter>>> m_filters;
};

#endif // BLACKLIST_H
