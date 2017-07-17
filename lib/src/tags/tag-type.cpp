#include "tag-type.h"


TagType::TagType()
	: TagType("unknown")
{}
TagType::TagType(QString name)
	: m_name(name)
{}

QString TagType::name() const
{
	return m_name;
}

int TagType::number() const
{
	static QMap<QString, int> shortTypes
	{
		{ "general", 0 },
		{ "artist", 1 },
		//{ "general", 2 },
		{ "copyright", 3 },
		{ "character", 4 },
		{ "model", 5 },
		{ "photo_set", 6 },
	};

	return shortTypes.contains(m_name) ? shortTypes[m_name] : -1;
}

bool operator==(const TagType &a, const TagType &b)
{
	return a.name() == b.name();
}
