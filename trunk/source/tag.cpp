#include "tag.h"



Tag::Tag(QString text, QString type, int count) : m_text(text), m_type(type), m_count(count)
{ }
Tag::~Tag()
{ }

void Tag::setText(QString text)	{ m_text = text;	}
void Tag::setType(QString type)	{ m_type = type;	}
void Tag::setCount(int count)	{ m_count = count;	}

QString	Tag::text()		{ return m_text;	}
QString	Tag::type()		{ return m_type;	}
int		Tag::count()	{ return m_count;	}
