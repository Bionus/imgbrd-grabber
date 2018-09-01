#include "post-filter.h"
#include "filter.h"
#include "filter-factory.h"


PostFilter::PostFilter(const QStringList &filters)
{
	for (const QString &filter : filters)
	{
		auto fil = QSharedPointer<Filter>(FilterFactory::build(filter));
		if (!fil.isNull())
			m_filters.append(fil);
	}
}

int PostFilter::count() const
{
	return m_filters.count();
}

QStringList PostFilter::match(const QMap<QString, Token> &tokens) const
{
	QStringList ret;
	for (const auto &filter : m_filters)
	{
		QString err = filter->match(tokens);
		if (!err.isEmpty())
			ret.append(err);
	}
	return ret;
}
