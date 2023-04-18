#ifndef TAG_FILTER_LIST_H
#define TAG_FILTER_LIST_H

#include <QList>
#include <QStringList>


class QRegularExpression;
class QString;
class Tag;

class TagFilterList
{
	public:
		void add(const QString &word);
		void add(const QStringList &words);
		void clear();

		QList<Tag> filterTags(const QList<Tag> &tags) const;

	private:
		QStringList m_rawTags;
		QList<QRegularExpression> m_starTags;
};

#endif // TAG_FILTER_LIST_H
