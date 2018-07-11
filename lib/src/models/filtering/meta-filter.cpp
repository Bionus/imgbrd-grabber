#include "meta-filter.h"
#include <QDateTime>
#include <QRegExp>
#include <QStringBuilder>


MetaFilter::MetaFilter(QString type, QString val, bool invert)
	: Filter(invert), m_type(std::move(type)), m_val(std::move(val))
{}

QString MetaFilter::toString() const
{
	return QString(m_invert ? "-" : "") % m_type % ":" % m_val;
}

bool MetaFilter::compare(const Filter& rhs) const
{
	auto other = dynamic_cast<const MetaFilter*>(&rhs);
	if (other == Q_NULLPTR)
		return false;

	return m_type == other->m_type && m_val == other->m_val;
}

static int toDate(const QString &text)
{
	QDateTime date = QDateTime::fromString(text, "yyyy-MM-dd");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	date = QDateTime::fromString(text, "MM/dd/yyyy");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	return 0;
}

QString MetaFilter::match(const QMap<QString, Token> &tokens, bool invert) const
{
	if (m_invert)
	{ invert = !invert; }

	// Grabber specials
	if (m_type == QStringLiteral("grabber"))
	{
		const QStringList &vals = tokens[m_type].value().toStringList();
		const bool cond = vals.contains(m_val, Qt::CaseInsensitive);

		if (!cond && !invert)
		{ return QObject::tr("image is not \"%1\"").arg(m_val); }
		if (cond && invert)
		{ return QObject::tr("image is \"%1\"").arg(m_val); }

		return QString();
	}

	// Meta tokens
	if (!tokens.contains(m_type))
	{
		QStringList keys = tokens.keys();
		return QObject::tr("unknown type \"%1\" (available types: \"%2\")").arg(m_type, keys.join("\", \""));
	}

	const QVariant &token = tokens[m_type].value();
	if (token.type() == QVariant::Int || token.type() == QVariant::DateTime || token.type() == QVariant::ULongLong)
	{
		int input = 0;
		if (token.type() == QVariant::Int)
		{ input = token.toInt(); }
		else if (token.type() == QVariant::DateTime)
		{ input = token.toDateTime().toString("yyyyMMdd").toInt(); }
		else if (token.type() == QVariant::ULongLong)
		{ input = token.toULongLong(); }

		bool cond;
		if (token.type() == QVariant::DateTime)
		{
			if (m_val.startsWith("..") || m_val.startsWith("<="))
			{ cond = input <= toDate(m_val.right(m_val.size()-2)); }
			else if (m_val.endsWith(".."))
			{ cond = input >= toDate(m_val.left(m_val.size()-2)); }
			else if (m_val.startsWith(">="))
			{ cond = input >= toDate(m_val.right(m_val.size()-2)); }
			else if (m_val.startsWith("<"))
			{ cond = input < toDate(m_val.right(m_val.size()-1)); }
			else if (m_val.startsWith(">"))
			{ cond = input > toDate(m_val.right(m_val.size()-1)); }
			else if (m_val.contains(".."))
			{ cond = input >= toDate(m_val.left(m_val.indexOf(".."))) && input <= toDate(m_val.right(m_val.size()-m_val.indexOf("..")-2));	}
			else
			{ cond = input == toDate(m_val); }
		}
		else
		{
			if (m_val.startsWith("..") || m_val.startsWith("<="))
			{ cond = input <= m_val.rightRef(m_val.size()-2).toInt(); }
			else if (m_val.endsWith(".."))
			{ cond = input >= m_val.leftRef(m_val.size()-2).toInt(); }
			else if (m_val.startsWith(">="))
			{ cond = input >= m_val.rightRef(m_val.size()-2).toInt(); }
			else if (m_val.startsWith("<"))
			{ cond = input < m_val.rightRef(m_val.size()-1).toInt(); }
			else if (m_val.startsWith(">"))
			{ cond = input > m_val.rightRef(m_val.size()-1).toInt(); }
			else if (m_val.contains(".."))
			{ cond = input >= m_val.leftRef(m_val.indexOf("..")).toInt() && input <= m_val.rightRef(m_val.size()-m_val.indexOf("..")-2).toInt();	}
			else
			{ cond = input == m_val.toInt(); }
		}

		if (!cond && !invert)
		{ return QObject::tr("image's %1 does not match").arg(m_type); }
		if (cond && invert)
		{ return QObject::tr("image's %1 match").arg(m_type); }
	}
	else
	{
		if (m_type == "rating")
		{
			QMap<QString, QString> assoc;
			assoc["s"] = "safe";
			assoc["q"] = "questionable";
			assoc["e"] = "explicit";

			const QString val = assoc.contains(m_val) ? assoc[m_val] : m_val;

			const bool cond = !val.isEmpty() && token.toString().toLower().startsWith(val.at(0));
			if (!cond && !invert)
			{ return QObject::tr("image is not \"%1\"").arg(val); }
			if (cond && invert)
			{ return QObject::tr("image is \"%1\"").arg(val); }
		}
		else if (m_type == "source")
		{
			QRegExp rx(m_val + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
			const bool cond = rx.exactMatch(token.toString());
			if (!cond && !invert)
			{ return QObject::tr("image's source does not starts with \"%1\"").arg(m_val); }
			if (cond && invert)
			{ return QObject::tr("image's source starts with \"%1\"").arg(m_val); }
		}
		else
		{
			const QString input = token.toString();

			const bool cond = input == m_val;

			if (!cond && !invert)
			{ return QObject::tr("image's %1 does not match").arg(m_type); }
			if (cond && invert)
			{ return QObject::tr("image's %1 match").arg(m_type); }
		}
	}

	return QString();
}
