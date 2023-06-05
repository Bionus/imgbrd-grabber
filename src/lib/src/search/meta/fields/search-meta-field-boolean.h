#ifndef SEARCH_META_FIELD_BOOLEAN_H
#define SEARCH_META_FIELD_BOOLEAN_H

#include "search-meta-field-base.h"


struct SearchMetaFieldBoolean : public SearchMetaFieldBase<bool>
{
	public:
		SearchMetaFieldBoolean(QString label, bool defaultValue);

		bool fromString(const QString &value) const override;
};

#endif // SEARCH_META_FIELD_BOOLEAN_H
