#ifndef FILTER_FACTORY_H
#define FILTER_FACTORY_H

class Filter;
class QString;


class FilterFactory
{
	public:
		static Filter *build(QString filter);
};

#endif // FILTER_FACTORY_H
