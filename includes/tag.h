#ifndef TAG_H
#define TAG_H

#include <QString>



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
		QString		text() const;
		QString		type() const;
		int			shortType() const;
		int			count() const;
		QStringList	related() const;

	private:
		QString		m_text, m_type;
		int			m_count;
		QStringList	m_related;
};

bool sortByFrequency(Tag, Tag);

#endif // TAG_H
