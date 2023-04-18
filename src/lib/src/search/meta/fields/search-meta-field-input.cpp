#include "search-meta-field-input.h"
#include <utility>


SearchMetaFieldInput::SearchMetaFieldInput(QString label, QString defaultValue)
	: SearchMetaFieldBase(std::move(label), std::move(defaultValue))
{}


QString SearchMetaFieldInput::fromString(const QString &value) const
{
	return value;
}