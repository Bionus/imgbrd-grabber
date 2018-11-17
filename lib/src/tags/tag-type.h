#ifndef TAG_TYPE_H
#define TAG_TYPE_H

#include <QMetaType>
#include <QString>


class TagType
{
	public:
		TagType();
		explicit TagType(const QString &name);
		bool isUnknown() const;
		const QString &name() const;
		int number() const;

	private:
		bool m_isUnknown;
		QString m_name;
};

bool operator==(const TagType &a, const TagType &b);

Q_DECLARE_METATYPE(TagType)

#endif // TAG_TYPE_H
