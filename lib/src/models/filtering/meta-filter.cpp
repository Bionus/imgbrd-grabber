#include "meta-filter.h"
#include <QDateTime>
#include <QRegExp>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QTimeZone>
#include <utility>
#include "loader/token.h"


MetaFilter::MetaFilter(QString type, QString val, bool invert)
	: Filter(invert), m_type(std::move(type)), m_val(std::move(val))
{}

QString MetaFilter::toString() const
{
	return QString(m_invert ? "-" : "") % m_type % ":" % m_val;
}

bool MetaFilter::compare(const Filter& rhs) const
{
	const auto other = dynamic_cast<const MetaFilter*>(&rhs);
	if (other == nullptr) {
		return false;
	}

	return m_type == other->m_type && m_val == other->m_val;
}

static QDateTime stringToDate(const QString &text)
{
	QDateTime date = QDateTime::fromString(text, "yyyy-MM-dd");
	if (date.isValid()) {
		return date;
	}
	date = QDateTime::fromString(text, "MM/dd/yyyy");
	if (date.isValid()) {
		return date;
	}
	return QDateTime();
}

static int stringToInt(const QString &text)
{ return text.toInt(); }

// FIXME(Bionus): remove globals
static QDateTime ageToDateImage;
static QDateTime ageToDateTestNow;
static QDateTime ageToDate(const QString &text)
{
	static QRegularExpression rx("^(\\d+)(\\w+)$");
	auto match = rx.match(text);
	if (!match.hasMatch()) {
		return QDateTime();
	}

	const int count = match.captured(1).toInt();
	const QString type = match.captured(2);

	// Define "now" with the correct timezone
	QDateTime base;
	if (ageToDateTestNow.isValid()) {
		base = ageToDateTestNow;
	} else {
		base = QDateTime::currentDateTimeUtc();
		base.setTimeZone(ageToDateImage.timeZone());
	}

	if (type.startsWith("y")) {
		return base.addYears(-count);
	}
	if (type.startsWith("mo")) {
		return base.addMonths(-count);
	}
	if (type.startsWith("w")) {
		return base.addDays(-(count * 7));
	}
	if (type.startsWith("d")) {
		return base.addDays(-count);
	}
	if (type.startsWith("h")) {
		return base.addSecs(-(count * 60 * 60));
	}
	if (type.startsWith("mi")) {
		return base.addSecs(-(count * 60));
	}
	if (type.startsWith("s")) {
		return base.addSecs(-count);
	}

	return QDateTime();
}

template <typename T>
static bool rangeCheck(T (*converter)(const QString &), T input, const QString &val)
{
	if (val.startsWith("..") || val.startsWith("<=")) {
		return input <= converter(val.right(val.size() - 2));
	}
	if (val.endsWith("..")) {
		return input >= converter(val.left(val.size() - 2));
	}
	if (val.startsWith(">=")) {
		return input >= converter(val.right(val.size() - 2));
	}
	if (val.startsWith("<")) {
		return input < converter(val.right(val.size() - 1));
	}
	if (val.startsWith(">")) {
		return input > converter(val.right(val.size() - 1));
	}
	if (val.contains("..")) {
		return input >= converter(val.left(val.indexOf(".."))) && input <= converter(val.right(val.size() - val.indexOf("..") - 2));
	}
	return input == converter(val);
}

QString MetaFilter::match(const QMap<QString, Token> &tokens, bool invert) const
{
	if (m_invert) {
		invert = !invert;
	}

	// Grabber specials
	if (m_type == QStringLiteral("grabber")) {
		const QStringList &vals = tokens[m_type].value().toStringList();
		const bool cond = vals.contains(m_val, Qt::CaseInsensitive);

		if (!cond && !invert) {
			return QObject::tr("image is not \"%1\"").arg(m_val);
		}
		if (cond && invert) {
			return QObject::tr("image is \"%1\"").arg(m_val);
		}

		return QString();
	}

	// Non-token metas
	if (m_type == "age") {
		if (!tokens.contains("date")) {
			return QObject::tr("An image needs a date to be filtered by age");
		}

		const QDateTime &date = tokens["date"].value().toDateTime();
		ageToDateImage = date;
		ageToDateTestNow = tokens["TESTS_now"].value().toDateTime();
		const bool cond = rangeCheck(ageToDate, date, m_val);

		if (cond && !invert) {
			return QObject::tr("image's %1 does not match").arg(m_type);
		}
		if (!cond && invert) {
			return QObject::tr("image's %1 match").arg(m_type);
		}

		return QString();
	}

	// Meta tokens
	if (!tokens.contains(m_type)) {
		QStringList keys = tokens.keys();
		return QObject::tr(R"(unknown type "%1" (available types: "%2"))").arg(m_type, keys.join("\", \""));
	}

	const QVariant &token = tokens[m_type].value();
	if (token.type() == QVariant::Int || token.type() == QVariant::DateTime || token.type() == QVariant::ULongLong) {
		int input = 0;
		if (token.type() == QVariant::Int) {
			input = token.toInt();
		} else if (token.type() == QVariant::ULongLong) {
			input = token.toULongLong();
		}

		bool cond;
		if (token.type() == QVariant::DateTime) {
			cond = rangeCheck(stringToDate, token.toDateTime(), m_val);
		} else {
			cond = rangeCheck(stringToInt, input, m_val);
		}

		if (!cond && !invert) {
			return QObject::tr("image's %1 does not match").arg(m_type);
		}
		if (cond && invert) {
			return QObject::tr("image's %1 match").arg(m_type);
		}
	} else {
		if (m_type == "rating") {
			QMap<QString, QString> assoc;
			assoc["s"] = "safe";
			assoc["q"] = "questionable";
			assoc["e"] = "explicit";

			const QString val = assoc.contains(m_val) ? assoc[m_val] : m_val;

			const bool cond = !val.isEmpty() && token.toString().toLower().startsWith(val.at(0));
			if (!cond && !invert) {
				return QObject::tr("image is not \"%1\"").arg(val);
			}
			if (cond && invert) {
				return QObject::tr("image is \"%1\"").arg(val);
			}
		} else if (m_type == "source") {
			QRegExp rx(m_val + "*", Qt::CaseInsensitive, QRegExp::Wildcard);
			const bool cond = rx.exactMatch(token.toString());
			if (!cond && !invert) {
				return QObject::tr("image's source does not starts with \"%1\"").arg(m_val);
			}
			if (cond && invert) {
				return QObject::tr("image's source starts with \"%1\"").arg(m_val);
			}
		} else {
			const QString input = token.toString();

			const bool cond = input == m_val;

			if (!cond && !invert) {
				return QObject::tr("image's %1 does not match").arg(m_type);
			}
			if (cond && invert) {
				return QObject::tr("image's %1 match").arg(m_type);
			}
		}
	}

	return QString();
}
