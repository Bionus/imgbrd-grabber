#include "search-meta-field-boolean.h"
#include <utility>


SearchMetaFieldBoolean::SearchMetaFieldBoolean(QString label, bool defaultValue)
	: SearchMetaFieldBase(std::move(label), defaultValue)
{}


bool SearchMetaFieldBoolean::fromString(const QString &value) const
{
	if (value == "true" || value == "yes" || value == "1") {
		return true;
	}
	if (value == "false" || value == "no" || value == "0") {
		return false;
	}
	return defaultValue();
}
