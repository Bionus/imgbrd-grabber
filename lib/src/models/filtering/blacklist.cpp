#include "blacklist.h"
#include "filter.h"
#include "filter-factory.h"
#include "functions.h"


Blacklist::Blacklist(const QStringList &tags)
{
	for (const QString &tag : tags)
		add(tag);
}

int Blacklist::indexOf(const QString &tag) const
{
	for (int i = 0; i < m_filters.count(); ++i)
	{
		const auto &filters = m_filters[i];
		if (filters.count() == 1 && QString::compare(filters[0]->toString(), tag, Qt::CaseInsensitive) == 0)
			return i;
	}
	return -1;
}

bool Blacklist::contains(const QString &tag) const
{
	return indexOf(tag) != -1;
}

void Blacklist::add(const QString &tag)
{
	auto filter = QSharedPointer<Filter>(FilterFactory::build(tag));
	if (!filter.isNull())
		m_filters.append(QList<QSharedPointer<Filter>>() << filter);
}

void Blacklist::add(const QStringList &tags)
{
	QList<QSharedPointer<Filter>> filters;
	for (const QString &tag : tags)
	{
		auto filter = QSharedPointer<Filter>(FilterFactory::build(tag));
		if (!filter.isNull())
			filters.append(filter);
	}

	if (!filters.isEmpty())
		m_filters.append(filters);
}

bool Blacklist::remove(const QString &tag)
{
	const int index = indexOf(tag);
	if (index == -1)
		return false;

	m_filters.removeAt(index);
	return true;
}

QString Blacklist::toString() const
{
	QString ret;
	for (const auto &filters : qAsConst(m_filters))
	{
		if (!ret.isEmpty())
		{ ret.append("\n"); }
		for (int i = 0; i < filters.count(); ++i)
		{
			if (i != 0)
			{ ret.append(' '); }
			ret.append(filters[i]->toString());
		}
	}
	return ret;
}

QStringList Blacklist::match(const QMap<QString, Token> &tokens, bool invert) const
{
	QStringList detected;
	for (const auto &filters : qAsConst(m_filters))
	{
		bool allDetected = true;
		QStringList res;
		for (const QSharedPointer<Filter> &filter : filters)
		{
			if (filter->match(tokens, invert).isEmpty())
			{
				allDetected = false;
				break;
			}
			res.append(filter->toString());
		}
		if (allDetected)
			detected.append(res.join(' '));
	}
	return detected;
}
