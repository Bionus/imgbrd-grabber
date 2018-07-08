#include "post-filter.h"
#include "functions.h"
#include "loader/token.h"


int toDate(const QString &text)
{
	QDateTime date = QDateTime::fromString(text, "yyyy-MM-dd");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	date = QDateTime::fromString(text, "MM/dd/yyyy");
	if (date.isValid())
	{ return date.toString("yyyyMMdd").toInt(); }
	return 0;
}

QString PostFilter::match(const QMap<QString, Token> &tokens, QString filter, bool invert)
{
	// Invert the filter by prepending '-'
	if (filter.startsWith('-'))
	{
		filter = filter.right(filter.length() - 1);
		invert = !invert;
	}

	// Tokens
	if (filter.startsWith('%') && filter.endsWith('%'))
	{
		const QString key = filter.mid(1, filter.length() - 2);
		const bool cond = tokens.contains(key) && !isVariantEmpty(tokens[key].value());

		if (cond && invert)
		{ return QObject::tr("image has a \"%1\" token").arg(key); }
		else if (!cond && !invert)
		{ return QObject::tr("image does not have a \"%1\" token").arg(key); }
	}

	// Meta-tags
	else if (filter.contains(":"))
	{
		const QString type = filter.section(':', 0, 0).toLower();
		filter = filter.section(':', 1).toLower();

		// Grabber specials
		if (type == QStringLiteral("grabber"))
		{
			const QStringList &vals = tokens[type].value().toStringList();
			const bool cond = vals.contains(filter, Qt::CaseInsensitive);

			if (!cond && !invert)
			{ return QObject::tr("image is not \"%1\"").arg(filter); }
			if (cond && invert)
			{ return QObject::tr("image is \"%1\"").arg(filter); }

			return QString();
		}

		// Meta tokens
		if (!tokens.contains(type))
		{
			QStringList keys = tokens.keys();
			return QObject::tr("unknown type \"%1\" (available types: \"%2\")").arg(type, keys.join("\", \""));
		}

		const QVariant &token = tokens[type].value();
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
				if (filter.startsWith("..") || filter.startsWith("<="))
				{ cond = input <= toDate(filter.right(filter.size()-2)); }
				else if (filter.endsWith(".."))
				{ cond = input >= toDate(filter.left(filter.size()-2)); }
				else if (filter.startsWith(">="))
				{ cond = input >= toDate(filter.right(filter.size()-2)); }
				else if (filter.startsWith("<"))
				{ cond = input < toDate(filter.right(filter.size()-1)); }
				else if (filter.startsWith(">"))
				{ cond = input > toDate(filter.right(filter.size()-1)); }
				else if (filter.contains(".."))
				{ cond = input >= toDate(filter.left(filter.indexOf(".."))) && input <= toDate(filter.right(filter.size()-filter.indexOf("..")-2));	}
				else
				{ cond = input == toDate(filter); }
			}
			else
			{
				if (filter.startsWith("..") || filter.startsWith("<="))
				{ cond = input <= filter.rightRef(filter.size()-2).toInt(); }
				else if (filter.endsWith(".."))
				{ cond = input >= filter.leftRef(filter.size()-2).toInt(); }
				else if (filter.startsWith(">="))
				{ cond = input >= filter.rightRef(filter.size()-2).toInt(); }
				else if (filter.startsWith("<"))
				{ cond = input < filter.rightRef(filter.size()-1).toInt(); }
				else if (filter.startsWith(">"))
				{ cond = input > filter.rightRef(filter.size()-1).toInt(); }
				else if (filter.contains(".."))
				{ cond = input >= filter.leftRef(filter.indexOf("..")).toInt() && input <= filter.rightRef(filter.size()-filter.indexOf("..")-2).toInt();	}
				else
				{ cond = input == filter.toInt(); }
			}

			if (!cond && !invert)
			{ return QObject::tr("image's %1 does not match").arg(type); }
			if (cond && invert)
			{ return QObject::tr("image's %1 match").arg(type); }
		}
		else
		{
			if (type == "rating")
			{
				QMap<QString, QString> assoc;
				assoc["s"] = "safe";
				assoc["q"] = "questionable";
				assoc["e"] = "explicit";

				if (assoc.contains(filter))
					filter = assoc[filter];

				const bool cond = !filter.isEmpty() && token.toString().toLower().startsWith(filter.at(0));
				if (!cond && !invert)
				{ return QObject::tr("image is not \"%1\"").arg(filter); }
				if (cond && invert)
				{ return QObject::tr("image is \"%1\"").arg(filter); }
			}
			else if (type == "source")
			{
				QRegExp rx(filter + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
				const bool cond = rx.exactMatch(token.toString());
				if (!cond && !invert)
				{ return QObject::tr("image's source does not starts with \"%1\"").arg(filter); }
				if (cond && invert)
				{ return QObject::tr("image's source starts with \"%1\"").arg(filter); }
			}
			else
			{
				const QString input = token.toString();

				const bool cond = input == filter;

				if (!cond && !invert)
				{ return QObject::tr("image's %1 does not match").arg(type); }
				if (cond && invert)
				{ return QObject::tr("image's %1 match").arg(type); }
			}
		}
	}
	else if (!filter.isEmpty())
	{
		QStringList tags = tokens["allos"].value().toStringList();

		// Check if any tag match the filter (case insensitive plain text with wildcards allowed)
		bool cond = false;
		for (const QString &tag : tags)
		{
			QRegExp reg(filter.trimmed(), Qt::CaseInsensitive, QRegExp::Wildcard);
			if (reg.exactMatch(tag))
			{
				cond = true;
				break;
			}
		}

		if (!cond && !invert)
		{ return QObject::tr("image does not contains \"%1\"").arg(filter); }
		if (cond && invert)
		{ return QObject::tr("image contains \"%1\"").arg(filter); }
	}

	return QString();
}

QStringList PostFilter::filter(const QMap<QString, Token> &tokens, const QStringList &filters)
{
	QStringList ret;
	for (const QString &filter : filters)
	{
		QString err = match(tokens, filter);
		if (!err.isEmpty())
			ret.append(err);
	}
	return ret;
}

QStringList PostFilter::blacklisted(const QMap<QString, Token> &tokens, const QList<QStringList> &blacklistedTags, bool invert)
{
	QStringList detected;
	for (const QStringList &tags : blacklistedTags)
	{
		bool allDetected = true;
		for (const QString &tag : tags)
		{
			if (match(tokens, tag, invert).isEmpty())
				allDetected = false;
		}
		if (allDetected)
			detected.append(tags.join(' '));
	}
	return detected;
}
