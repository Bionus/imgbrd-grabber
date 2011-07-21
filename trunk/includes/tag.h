#ifndef TAG_H
#define TAG_H

#include <QtGui>
#include "tag.h"



class Tag
{
	public:
		explicit Tag(QString text, QString type = "general", int count = 1);
		~Tag();
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

#endif // TAG_H
