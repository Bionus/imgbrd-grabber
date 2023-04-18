#ifndef SEARCH_META_FIELD_INPUT_H
#define SEARCH_META_FIELD_INPUT_H

#include "search-meta-field-base.h"


struct SearchMetaFieldInput : public SearchMetaFieldBase<QString>
{
	public:
		SearchMetaFieldInput(QString label, QString defaultValue);

		QString fromString(const QString &value) const override;
};

#endif // SEARCH_META_FIELD_INPUT_H
