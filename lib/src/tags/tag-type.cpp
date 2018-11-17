#include "tags/tag-type.h"
#include <QMap>


TagType::TagType()
	: m_isUnknown(true), m_name("unknown")
{}
TagType::TagType(const QString &name)
	: m_isUnknown(name.isEmpty() || name == "unknown"), m_name(name.isEmpty() ? "unknown" : name)
{
	// Sometimes a type is found with multiple words, only the first is relevant
	if (!m_isUnknown)
	{
		const int typeSpace = m_name.indexOf(' ');
		if (typeSpace != -1)
		{ m_name = m_name.left(typeSpace); }
	}
}

bool TagType::isUnknown() const
{
	return m_isUnknown;
}

const QString &TagType::name() const
{
	return m_name;
}

int TagType::number() const
{
	static QMap<QString, int> shortTypes
	{
		{ "general", 0 },
		{ "artist", 1 },
		// { "general", 2 },
		{ "copyright", 3 },
		{ "character", 4 },
		{ "model", 5 },
		{ "photo_set", 6 },
	};

	return !m_isUnknown && shortTypes.contains(m_name) ? shortTypes[m_name] : -1;
}

bool operator==(const TagType &a, const TagType &b)
{
	return (a.isUnknown() && b.isUnknown())
		|| a.name() == b.name();
}
