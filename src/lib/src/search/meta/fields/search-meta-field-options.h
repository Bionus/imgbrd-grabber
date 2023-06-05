#ifndef SEARCH_META_FIELD_OPTIONS_H
#define SEARCH_META_FIELD_OPTIONS_H

#include "search-meta-field-base.h"


struct SearchMetaFieldOptions : public SearchMetaFieldBase<QString>
{
	public:
		SearchMetaFieldOptions(QString label, QString defaultValue, QMap<QString, QString> options);

		QString fromString(const QString &value) const override;

		QMap<QString, QString> options() const;

	private:
		QMap<QString, QString> m_options;
};

#endif // SEARCH_META_FIELD_OPTIONS_H
