#ifndef TAG_FILTER_LIST_H
#define TAG_FILTER_LIST_H

#include <QList>
#include <QSet>


class QRegularExpression;
class QString;
class Tag;

/**
 * Simple class allowing to filter tags based on a list of words.
 * The word filters support wildcards using the star character (*).
 */
class TagFilterList
{
	public:
		void add(const QString &word);
		void add(const QStringList &words);
		void remove(const QString &word);
		void clear();

		QList<Tag> filterTags(const QList<Tag> &tags) const;
		bool contains(const QString &word) const;

	private:
		QSet<QString> m_rawTags;
		QSet<QRegularExpression> m_starTags;
};

#endif // TAG_FILTER_LIST_H
