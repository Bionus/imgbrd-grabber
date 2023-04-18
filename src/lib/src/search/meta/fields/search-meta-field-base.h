#ifndef SEARCH_META_FIELD_BASE_H
#define SEARCH_META_FIELD_BASE_H

#include "search-meta-field.h"
#include <QString>
#include <QVariant>


template <typename T>
class SearchMetaFieldBase : public SearchMetaField
{
	public:
		SearchMetaFieldBase(QString label, T defaultValue);

		virtual T fromString(const QString &value) const = 0;

		T defaultValue() const;

	private:
		T m_defaultValue;
};

#include "search-meta-field-base.hpp"

#endif // SEARCH_META_FIELD_BASE_H
