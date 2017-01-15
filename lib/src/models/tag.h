#ifndef TAG_H
#define TAG_H

#include <QString>
#include <QStringList>
#include "models/favorite.h"
#include "profile.h"


class Tag
{
	public:
		Tag();
		explicit Tag(QString text, QString type = "unknown", int count = 1, QStringList related = QStringList());
		~Tag();
		static Tag FromCapture(QStringList caps, QStringList order = QStringList());
		static QList<Tag> FromRegexp(QString rx, QStringList order, const QString &source);
		static QString GetType(QString type, QStringList ids);
		QString stylished(Profile *profile, QStringList ignored = QStringList(), QStringList blacklisted = QStringList(), bool count = false, bool nounderscores = false) const;
		void setText(QString);
		void setType(QString);
		void setCount(int);
		void setRelated(QStringList);
		QString		text()		const;
		QString		type()		const;
		int			shortType()	const;
		int			count()		const;
		QStringList	related()	const;
		QString		typedText()	const;

	protected:
		QString qFontToCss(QFont font) const;

	private:
		QString		m_text, m_type;
		int			m_count;
		QStringList	m_related;
};

bool sortByFrequency(Tag, Tag);
bool operator==(const Tag &t1, const Tag &t2);

Q_DECLARE_METATYPE(Tag)

#endif // TAG_H
