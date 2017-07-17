#include "tag-type.h"


TagType::TagType(QString name)
	: m_name(name)
{ }

QString TagType::name() const
{
	return m_name;
}

bool operator==(const TagType &a, const TagType &b)
{
	return a.name() == b.name();
}
