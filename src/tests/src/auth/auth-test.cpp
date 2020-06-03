#include "auth/http-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "catch.h"


TEST_CASE("Auth")
{
	SECTION("URL auth")
	{
		QList<AuthField*> fields;
		UrlAuth auth("url", fields, 50);

		REQUIRE(auth.type() == QString("url"));
		REQUIRE(auth.fields() == fields);
		REQUIRE(auth.maxPage() == 50);
	}

	SECTION("HTTP auth")
	{
		QList<AuthField*> fields;
		HttpAuth auth("post", "https://www.google.com", fields, "cookie");

		REQUIRE(auth.type() == QString("post"));
		REQUIRE(auth.url() == QString("https://www.google.com"));
		REQUIRE(auth.fields() == fields);
		REQUIRE(auth.cookie() == QString("cookie"));
	}

	SECTION("OAuth2 auth")
	{
		OAuth2Auth auth("oauth2", "password", "https://www.google.com");

		REQUIRE(auth.type() == QString("oauth2"));
		REQUIRE(auth.authType() == QString("password"));
		REQUIRE(auth.tokenUrl() == QString("https://www.google.com"));
	}
}
