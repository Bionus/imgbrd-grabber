#ifndef SEARCH_META_FIELD_FACTORY_H
#define SEARCH_META_FIELD_FACTORY_H

#include <QJSValue>


class SearchMetaField;

class SearchMetaFieldFactory
{
	public:
		static SearchMetaField *build(const QJSValue &jsValue);
};

#endif // SEARCH_META_FIELD_FACTORY_H
