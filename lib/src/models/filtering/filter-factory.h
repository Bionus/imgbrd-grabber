#ifndef FILTER_FACTORY_H
#define FILTER_FACTORY_H

#include <QString>


class Filter;

class FilterFactory
{
	public:
		static Filter *build(QString filter);
};

#endif // FILTER_FACTORY_H
