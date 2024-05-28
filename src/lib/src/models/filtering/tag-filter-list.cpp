#include "tag-filter-list.h"
#include <QRegularExpression>
#include "tags/tag.h"


void TagFilterList::add(const QString &word)
{
	if (word.contains('*')) {
		m_starTags.insert(QRegularExpression::fromWildcard(word, Qt::CaseInsensitive));
	} else {
		m_rawTags.insert(word);
	}
}

void TagFilterList::add(const QStringList &words)
{
	for (const QString &word : words) {
		add(word);
	}
}

void TagFilterList::remove(const QString &word)
{
	if (word.contains('*')) {
		m_starTags.remove(QRegularExpression::fromWildcard(word, Qt::CaseInsensitive));
	} else {
		m_rawTags.remove(word);
	}
}

void TagFilterList::clear()
{
	m_rawTags.clear();
	m_starTags.clear();
}

QList<Tag> TagFilterList::filterTags(const QList<Tag> &tags) const
{
	QList<Tag> ret;

	for (const Tag &tag : tags) {
		if (m_rawTags.contains(tag.text())) {
			continue;
		}

		bool removed = false;
		for (const QRegularExpression &reg : m_starTags) {
			if (reg.match(tag.text()).hasMatch()) {
				removed = true;
				break;
			}
		}
		if (!removed) {
			ret.append(tag);
		}
	}

	return ret;
}

bool TagFilterList::contains(const QString &word) const
{
	return m_rawTags.contains(word);
}
