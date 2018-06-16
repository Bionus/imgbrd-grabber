#ifndef TAG_NAME_H
#define TAG_NAME_H

#include <QMetaType>
#include "tags/tag-name-format.h"


class TagName
{
	public:
		TagName();
		explicit TagName(const QString &name, const TagNameFormat &format = TagNameFormat::Normalized());
		QString normalized() const;
		QString formatted(const TagNameFormat &format) const;

	private:
		QString m_normalized;
		QString m_name;
		TagNameFormat m_format;
		mutable QStringList m_words;
};

bool operator==(const TagName &a, const TagName &b);

Q_DECLARE_METATYPE(TagName)

#endif // TAG_NAME_H
