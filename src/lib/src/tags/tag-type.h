#ifndef TAG_TYPE_H
#define TAG_TYPE_H

#include <QMetaType>
#include <QString>


class Site;

class TagType
{
	public:
		TagType();
		explicit TagType(const QString &name);
		bool isUnknown() const;
		const QString &name() const;
		int number(Site *site) const;

	private:
		bool m_isUnknown;
		QString m_name;
};

bool operator==(const TagType &a, const TagType &b);

Q_DECLARE_METATYPE(TagType)

#endif // TAG_TYPE_H
