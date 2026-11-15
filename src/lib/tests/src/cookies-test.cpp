#include <QString>
#include <QTimeZone>
#include "catch.h"
#include "cookies.h"


TEST_CASE("Cookies")
{
	SECTION("Load cookies from JSON")
	{
		SECTION("Empty file")
		{
			const auto result = loadCookiesFromJson("");
			REQUIRE(result.isEmpty());
		}

		SECTION("Empty JSON array")
		{
			const auto result = loadCookiesFromJson("[]");
			REQUIRE(result.isEmpty());
		}

		SECTION("Invalid type")
		{
			const QString json = GENERATE("{}", "123", "\"test\"", "null");

			const auto result = loadCookiesFromJson(json);
			REQUIRE(result.isEmpty());
		}

		SECTION("Valid file")
		{
			const QString json =
				"["
				"{\"name\":\"Foo\", \"value\":\"Bar\", \"expirationDate\": 1796405411.837574},"
				"{\"name\":\"Key\", \"value\":\"Value\", \"domain\": \"example.com\"}"
				"]";

			const auto result = loadCookiesFromJson(json);

			REQUIRE(result.count() == 2);

			REQUIRE(result[0].name() == "Foo");
			REQUIRE(result[0].value() == "Bar");
			REQUIRE(result[0].domain().isEmpty());
			REQUIRE(result[0].expirationDate() == QDateTime(QDate(2026, 12, 4), QTime(17, 30, 11), QTimeZone::UTC));

			REQUIRE(result[1].name() == "Key");
			REQUIRE(result[1].value() == "Value");
			REQUIRE(result[1].domain() == "example.com");
			REQUIRE(result[1].expirationDate().isNull());
		}
	}

	SECTION("Load cookies from headers")
	{
		SECTION("Empty file")
		{
			const auto result = loadCookiesFromHeaders("");
			REQUIRE(result.isEmpty());
		}

		SECTION("Valid file")
		{
			const QString txt =
				"Foo=Bar; Secure; Domain=example.com\n"
				"Key=Value; Path=/path/\n";

			const auto result = loadCookiesFromHeaders(txt);

			REQUIRE(result.count() == 2);

			REQUIRE(result[0].name() == "Foo");
			REQUIRE(result[0].value() == "Bar");
			REQUIRE(result[0].domain() == "example.com");
			REQUIRE(result[0].path().isEmpty());
			REQUIRE(result[0].isSecure());

			REQUIRE(result[1].name() == "Key");
			REQUIRE(result[1].value() == "Value");
			REQUIRE(!result[1].isSecure());
			REQUIRE(result[1].domain().isEmpty());
			REQUIRE(result[1].path() == "/path/");
		}
	}

	SECTION("Load cookies from TXT")
	{
		SECTION("Empty file")
		{
			const auto result = loadCookiesFromTxt("");
			REQUIRE(result.isEmpty());
		}

		SECTION("Single-line key-value file")
		{
			const auto result = loadCookiesFromTxt("Foo=Bar; Key=Value;");

			REQUIRE(result.count() == 2);
			REQUIRE(result[0].name() == "Foo");
			REQUIRE(result[0].value() == "Bar");
			REQUIRE(result[1].name() == "Key");
			REQUIRE(result[1].value() == "Value");
		}

		SECTION("Multi-line key-value file")
		{
			const auto result = loadCookiesFromTxt("Foo=Bar\nKey=Value\n");

			REQUIRE(result.count() == 2);
			REQUIRE(result[0].name() == "Foo");
			REQUIRE(result[0].value() == "Bar");
			REQUIRE(result[1].name() == "Key");
			REQUIRE(result[1].value() == "Value");
		}
	}

	SECTION("Load cookies from Netscape")
	{
		SECTION("Empty file")
		{
			const auto result = loadCookiesFromNetscape("");
			REQUIRE(result.isEmpty());
		}

		SECTION("Valid file")
		{
			const QString txt =
				"# Netscape HTTP Cookie File\n\n"
				".test.com	TRUE	/	TRUE	1777397402	name	value\n"
				".domain.test.com	TRUE	/	FALSE	1796405412		no_value\n"
				"domain.test.com	FALSE	/	TRUE	1764437405	foo	bar\n";

			const auto result = loadCookiesFromNetscape(txt);

			REQUIRE(result.count() == 3);
			REQUIRE(result[0].domain() == ".test.com");
			REQUIRE(result[0].name() == "name");
			REQUIRE(result[0].value() == "value");
			REQUIRE(result[0].isSecure());
			REQUIRE(result[0].expirationDate() == QDateTime(QDate(2026, 4, 28), QTime(17, 30, 2), QTimeZone::UTC));

			REQUIRE(result[1].name() == "no_value");
			REQUIRE(result[1].value() == "");
			REQUIRE(!result[1].isSecure());
		}
	}
}
