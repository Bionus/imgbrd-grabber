#include "tag-filter.h"
#include <QRegExp>
#include <QStringBuilder>
#include "loader/token.h"


TagFilter::TagFilter(QString tag, bool invert)
	: Filter(invert), m_tag(std::move(tag))
{
	if (m_tag.contains('*'))
	{ m_regexp.reset(new QRegExp(m_tag, Qt::CaseInsensitive, QRegExp::Wildcard)); }
}

QString TagFilter::toString() const
{
	return QString(m_invert ? "-" : "") % m_tag;
}

bool TagFilter::compare(const Filter& rhs) const
{
	const auto other = dynamic_cast<const TagFilter*>(&rhs);
	if (other == nullptr)
		return false;

	return m_tag == other->m_tag;
}

QString TagFilter::match(const QMap<QString, Token> &tokens, bool invert) const
{
	if (m_invert)
	{ invert = !invert; }

	const QStringList &tags = tokens["allos"].value().toStringList();

	// Check if any tag match the filter (case insensitive plain text with wildcards allowed)
	bool cond = false;
	for (const QString &tag : tags)
	{
		const bool match = m_regexp.isNull() ? tag == m_tag : m_regexp->exactMatch(tag);
		if (match)
		{
			cond = true;
			break;
		}
	}

	if (!cond && !invert)
	{ return QObject::tr("image does not contains \"%1\"").arg(m_tag); }
	if (cond && invert)
	{ return QObject::tr("image contains \"%1\"").arg(m_tag); }

	return QString();
}
