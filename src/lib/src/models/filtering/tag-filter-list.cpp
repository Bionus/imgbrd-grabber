#include "tag-filter-list.h"
#include <QRegularExpression>
#include "tags/tag.h"


void TagFilterList::add(const QString &word)
{
	if (word.contains('*')) {
		m_starTags.append(QRegularExpression::fromWildcard(word, Qt::CaseInsensitive));
	} else {
		m_rawTags.append(word);
	}
}

void TagFilterList::add(const QStringList &words)
{
	for (const QString &word : words) {
		add(word);
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
