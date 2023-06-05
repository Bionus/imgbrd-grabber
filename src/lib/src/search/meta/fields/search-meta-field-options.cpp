#include "search-meta-field-options.h"
#include <utility>


SearchMetaFieldOptions::SearchMetaFieldOptions(QString label, QString defaultValue, QMap<QString, QString> options)
	: SearchMetaFieldBase(std::move(label), std::move(defaultValue)), m_options(std::move(options))
{}


QString SearchMetaFieldOptions::fromString(const QString &value) const
{
	return m_options.values().contains(value) ? value : defaultValue();
}


QMap<QString, QString> SearchMetaFieldOptions::options() const
{
	return m_options;
}
