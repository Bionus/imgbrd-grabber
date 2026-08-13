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
		bool contains(const QStringList &tags) const;
		void clear();
		void add(const QString &tag);
		void add(const QStringList &tags);
		bool remove(const QString &tag);
		bool remove(const QStringList &tags);

		QString toString() const;
		QStringList match(const QMap<QString, Token> &tokens, bool invert = true) const;

	protected:
		int indexOf(const QStringList &tags) const;

	private:
		QList<QList<QSharedPointer<Filter>>> m_filters;
};

#endif // BLACKLIST_H
