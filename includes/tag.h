#ifndef TAG_H
#define TAG_H

#include <QString>
#include <QStringList>



class Tag
{
	public:
		explicit Tag(QString text, QString type = "unknown", int count = 1, QStringList related = QStringList());
		~Tag();
		QString stylished(QStringList) const;
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

	private:
		QString		m_text, m_type;
		int			m_count;
		QStringList	m_related;
};

bool sortByFrequency(Tag, Tag);
bool operator==(const Tag &t1, const Tag &t2);

#endif // TAG_H
