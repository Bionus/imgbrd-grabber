#include <QDateTime>
#include <QLatin1String>
#include <QSize>
#include <QString>
#include <QUrl>
#include <string>
#include "vendor/catch/single_include/catch2/catch.hpp"


namespace Catch
{
	template<>
	struct StringMaker<QLatin1String>
	{
		static std::string convert(QLatin1String const& value)
		{
			return QString("\"" + value + "\"").toStdString();
		}
	};

	template<>
	struct StringMaker<QString>
	{
		static std::string convert(QString const& value)
		{
			return ("\"" + value + "\"").toStdString();
		}
	};

	template<>
	struct StringMaker<QChar>
	{
		static std::string convert(QChar const& value)
		{
			return std::string(1, value.toLatin1());
		}
	};

	template<>
	struct StringMaker<QUrl>
	{
		static std::string convert(QUrl const &value)
		{
			return value.toDisplayString().toStdString();
		}
	};

	template<>
	struct StringMaker<QSize>
	{
		static std::string convert(QSize const &value)
		{
			if (!value.isValid())
				return "{invalid size}";
			return QString("(%1 x %2)").arg(value.width()).arg(value.height()).toStdString();
		}
	};

	template<>
	struct StringMaker<QDateTime>
	{
		static std::string convert(QDateTime const &value)
		{
			if (!value.isValid())
				return "{invalid date}";
			return value.toString(Qt::ISODate).toStdString();
		}
	};
}
