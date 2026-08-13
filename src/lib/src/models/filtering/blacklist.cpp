#include "blacklist.h"
#include <QStringList>
#include "filter.h"
#include "filter-factory.h"
#include "functions.h"


Blacklist::Blacklist(const QStringList &tags)
{
	for (const QString &tag : tags) {
		add(tag);
	}
}

int Blacklist::indexOf(const QStringList &tags) const
{
	for (int i = 0; i < m_filters.count(); ++i) {
		const auto &filters = m_filters[i];
		if (filters.count() != tags.count()) {
			continue;
		}
		bool allMatch = true;
		for (int j = 0; j < tags.count(); ++j) {
			if (QString::compare(filters[j]->toString(false), tags[j], Qt::CaseInsensitive) != 0) {
				allMatch = false;
				break;
			}
		}
		if (allMatch) {
			return i;
		}
	}
	return -1;
}

bool Blacklist::isEmpty() const
{
	return m_filters.isEmpty();
}

bool Blacklist::contains(const QString &tag) const
{
	return contains(QStringList(tag));
}
bool Blacklist::contains(const QStringList &tags) const
{
	return indexOf(tags) != -1;
}

void Blacklist::clear()
{
	m_filters.clear();
}

void Blacklist::add(const QString &tag)
{
	add(QStringList(tag));
}

void Blacklist::add(const QStringList &tags)
{
	QList<QSharedPointer<Filter>> filters;
	for (const QString &tag : tags) {
		auto filter = QSharedPointer<Filter>(FilterFactory::build(tag));
		if (!filter.isNull()) {
			filters.append(filter);
		}
	}

	if (!filters.isEmpty()) {
		m_filters.append(filters);
	}
}

bool Blacklist::remove(const QString &tag)
{
	return remove(QStringList(tag));
}
bool Blacklist::remove(const QStringList &tags)
{
	const int index = indexOf(tags);
	if (index == -1) {
		return false;
	}

	m_filters.removeAt(index);
	return true;
}

QString Blacklist::toString() const
{
	QString ret;
	for (const auto &filters : qAsConst(m_filters)) {
		if (!ret.isEmpty()) {
			ret.append("\n");
		}
		for (int i = 0; i < filters.count(); ++i) {
			if (i != 0) {
				ret.append(' ');
			}
			ret.append(filters[i]->toString());
		}
	}
	return ret;
}

QStringList Blacklist::match(const QMap<QString, Token> &tokens, bool invert) const
{
	QStringList detected;
	for (const auto &filters : qAsConst(m_filters)) {
		bool allDetected = true;
		QStringList res;
		for (const QSharedPointer<Filter> &filter : filters) {
			if (filter->match(tokens, invert).isEmpty()) {
				allDetected = false;
				break;
			}
			res.append(filter->toString(false));
		}
		if (allDetected) {
			detected.append(res.join(' '));
		}
	}
	return detected;
}
