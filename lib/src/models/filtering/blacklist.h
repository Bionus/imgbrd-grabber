#ifndef BLACKLIST_H
#define BLACKLIST_H

#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QString>


class Filter;
class Token;

class Blacklist
{
	public:
		Blacklist() = default;
		explicit Blacklist(const QStringList &tags);

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
