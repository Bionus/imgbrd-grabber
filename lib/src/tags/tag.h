#ifndef TAG_H
#define TAG_H

#include <QString>
#include <QStringList>
#include <QFont>
#include "tag-type.h"


class Profile;

class Tag
{
	public:
		Tag();
		explicit Tag(QString text, QString type = "unknown", int count = 0, QStringList related = QStringList());
		explicit Tag(QString text, TagType type, int count = 0, QStringList related = QStringList());
		explicit Tag(int id, QString text, TagType type, int count = 0, QStringList related = QStringList());
		static Tag FromCapture(const QRegularExpressionMatch &match, const QStringList &groups);
		static QList<Tag> FromRegexp(QString rx, const QString &source);
		static QString GetType(QString type, QStringList ids);
		void setId(int id);
		void setText(const QString &text);
		void setType(const TagType &type);
		void setCount(int count);
		void setRelated(const QStringList &related);
		int			id()		const;
		QString		text()		const;
		TagType		type()		const;
		int			count()		const;
		QStringList	related()	const;

	private:
		int			m_id;
		QString		m_text;
		TagType		m_type;
		int			m_count;
		QStringList	m_related;
};

bool sortTagsByType(Tag, Tag);
bool sortTagsByName(Tag, Tag);
bool sortTagsByCount(Tag, Tag);
bool operator==(const Tag &t1, const Tag &t2);

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
