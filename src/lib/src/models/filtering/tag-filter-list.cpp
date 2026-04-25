#include "tag-filter-list.h"
#include <QRegularExpression>
#include <QStringList>
#include "tags/tag.h"


void TagFilterList::add(const QString &word)
{
	if (word.contains('*')) {
		m_starTagStrings.insert(word);
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
		m_starTagStrings.remove(word);
		m_starTags.remove(QRegularExpression::fromWildcard(word, Qt::CaseInsensitive));
	} else {
		for (auto it = m_rawTags.begin(); it != m_rawTags.end(); ) {
			if (it->compare(word, Qt::CaseInsensitive) == 0) {
				it = m_rawTags.erase(it);
			} else {
				++it;
			}
		}
	}
}

void TagFilterList::clear()
{
	m_rawTags.clear();
	m_starTagStrings.clear();
	m_starTags.clear();
}

QList<Tag> TagFilterList::filterTags(const QList<Tag> &tags) const
{
	QList<Tag> ret;

	for (const Tag &tag : tags) {
		bool removed = false;

		for (const QString &raw : m_rawTags) {
			if (raw.compare(tag.text(), Qt::CaseInsensitive) == 0) {
				removed = true;
				break;
			}
		}
		if (removed) {
			continue;
		}

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
	for (const QString &raw : m_rawTags) {
		if (raw.compare(word, Qt::CaseInsensitive) == 0) {
			return true;
		}
	}

	for (const QRegularExpression &reg : m_starTags) {
		if (reg.match(word).hasMatch()) {
			return true;
		}
	}

	return false;
}

QStringList TagFilterList::toStringList() const
{
	QStringList ret;

	for (const QString &raw : m_rawTags) {
		ret.append(raw);
	}
	for (const QString &wildcard : m_starTagStrings) {
		ret.append(wildcard);
	}

	ret.removeDuplicates();
	ret.sort();
	return ret;
}

QString TagFilterList::toString() const
{
	return toStringList().join("\n");
}
