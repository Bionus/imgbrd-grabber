#include "post-filter.h"
#include "functions.h"
#include "filter.h"
#include "filter-factory.h"


PostFilter::PostFilter(const QStringList &filters)
{
	for (const QString &filter : filters)
	{
		Filter *fil = FilterFactory::build(filter);
		if (fil != Q_NULLPTR)
			m_filters.append(fil);
	}
}

PostFilter::~PostFilter()
{
	qDeleteAll(m_filters);
}

int PostFilter::count() const
{
	return m_filters.count();
}

QStringList PostFilter::match(const QMap<QString, Token> &tokens) const
{
	QStringList ret;
	for (Filter *filter : m_filters)
	{
		QString err = filter->match(tokens);
		if (!err.isEmpty())
			ret.append(err);
	}
	return ret;
}
