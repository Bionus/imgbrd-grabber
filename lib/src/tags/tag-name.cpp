#include "tag-name.h"
#include <QMap>


TagName::TagName()
{}

TagName::TagName(QString name, TagNameFormat format)
	: m_name(name), m_format(format)
{
	m_normalized = normalized();
}

QString TagName::normalized() const
{
	return formatted(TagNameFormat::Normalized());
}

QString TagName::formatted(const TagNameFormat &format) const
{
	if (format == m_format)
		return m_name;

	if (m_words.isEmpty())
		m_words = m_name.split(m_format.wordSeparator());

	return format.formatted(m_words);
}

bool operator==(const TagName &a, const TagName &b)
{
	return a.normalized() == b.normalized();
}
