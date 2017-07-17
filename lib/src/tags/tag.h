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
		~Tag();
		static Tag FromCapture(QStringList caps, QStringList order = QStringList());
		static QList<Tag> FromRegexp(QString rx, QStringList order, const QString &source);
		static QString GetType(QString type, QStringList ids);
		QString stylished(Profile *profile, QStringList ignored = QStringList(), QStringList blacklisted = QStringList(), bool count = false, bool nounderscores = false) const;
		void setText(QString);
		void setCount(int);
		void setRelated(QStringList);
		QString		text()		const;
		TagType		type()		const;
		int			count()		const;
		QStringList	related()	const;
		QString		typedText()	const;

		static QString qFontToCss(QFont font);
		static QStringList Stylished(QList<Tag>, Profile *profile, bool count = false, bool nounderscores = false, bool sort = true);

	private:
		QString		m_text;
		TagType		m_type;
		int			m_count;
		QStringList	m_related;
};

bool sortByFrequency(Tag, Tag);
bool operator==(const Tag &t1, const Tag &t2);

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
