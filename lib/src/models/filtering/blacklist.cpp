#include "blacklist.h"
#include "filter.h"
#include "filter-factory.h"


Blacklist::Blacklist(const QStringList &tags)
{
	for (const QString &tag : tags)
		add(tag);
}

Blacklist::~Blacklist()
{
	for (const QList<Filter*> &filters : qAsConst(m_filters))
		qDeleteAll(filters);
}

int Blacklist::indexOf(const QString &tag) const
{
	for (int i = 0; i < m_filters.count(); ++i)
	{
		const QList<Filter*> &filters = m_filters[i];
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
	Filter *filter = FilterFactory::build(tag);
	if (filter != Q_NULLPTR)
		m_filters.append(QList<Filter*>() << filter);
}

void Blacklist::add(const QStringList &tags)
{
	QList<Filter*> filters;
	for (const QString &tag : tags)
	{
		Filter *filter = FilterFactory::build(tag);
		if (filter != Q_NULLPTR)
			filters.append(filter);
	}

	if (!filters.isEmpty())
		m_filters.append(filters);
}

bool Blacklist::remove(const QString &tag)
{
	int index = indexOf(tag);
	if (index == -1)
		return false;

	qDeleteAll(m_filters[index]);
	m_filters.removeAt(index);
	return true;
}

QString Blacklist::toString() const
{
	QString ret;
	for (const QList<Filter*> &filters : qAsConst(m_filters))
	{
		for (int i = 0; i < filters.count(); ++i)
		{
			if (i != 0)
			{ ret.append(' '); }
			ret.append(filters[i]->toString());
		}
		ret.append("\n");
	}
	return ret;
}

QStringList Blacklist::match(const QMap<QString, Token> &tokens, bool invert) const
{
	QStringList detected;
	for (const QList<Filter*> &filters : qAsConst(m_filters))
	{
		bool allDetected = true;
		QStringList res;
		for (Filter *filter : filters)
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
