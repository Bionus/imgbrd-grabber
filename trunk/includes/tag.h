#ifndef TAG_H
#define TAG_H

#include <QString>



class Tag
{
    public:
        explicit Tag(QString text, QString type = "unknown", int count = 1);
        ~Tag();
        QString stylished(QStringList);
        void setText(QString);
        void setType(QString);
        void setCount(int);
        QString text();
        QString type();
        int		count();

    private:
        QString	m_text, m_type;
        int		m_count;
};

bool sortByFrequency(Tag, Tag);

#endif // TAG_H
