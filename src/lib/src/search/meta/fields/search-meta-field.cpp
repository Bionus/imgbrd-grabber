#include "search-meta-field.h"
#include <utility>


SearchMetaField::SearchMetaField(QString label)
	: m_label(std::move(label))
{}


QString SearchMetaField::label() const
{
	return m_label;
}
