#include <QLatin1String>
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
}
