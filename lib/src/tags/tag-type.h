#ifndef TAG_TYPE_H
#define TAG_TYPE_H

#include <QString>


class TagType
{
	public:
		TagType();
		explicit TagType(QString name);
		QString name() const;
		int number() const;

	private:
		QString m_name;
};

bool operator==(const TagType &a, const TagType &b);

Q_DECLARE_METATYPE(TagType)

#endif // TAG_TYPE_H
