#ifndef SEARCH_META_FIELD_H
#define SEARCH_META_FIELD_H

#include <QString>
#include <QVariant>


class SearchMetaField
{
	public:
		SearchMetaField(QString label);
		virtual ~SearchMetaField() = default;

		QString label() const;

	private:
		QString m_label;
};

#endif // SEARCH_META_FIELD_H
