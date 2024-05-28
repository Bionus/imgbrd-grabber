#include <QScopedPointer>
#include "auth/http-auth.h"
#include "auth/http-basic-auth.h"
#include "auth/oauth1-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"
#include "catch.h"
#include "auth/auth-const-field.h"


TEST_CASE("Auth")
{
	SECTION("URL auth")
	{
		QList<AuthField*> fields;
		UrlAuth auth("url", fields, 50);

		REQUIRE(auth.type() == QString("url"));
		REQUIRE(auth.name() == QString("url"));
		REQUIRE(auth.fields() == fields);
		REQUIRE(auth.maxPage() == 50);
		REQUIRE(auth.settingFields().isEmpty());
	}

	SECTION("HTTP auth")
	{
		QScopedPointer<AuthField> textField(new AuthField("id", "key", AuthField::FieldType::Text));
		QScopedPointer<AuthConstField> constField(new AuthConstField("key", "val"));
		QList<AuthField*> fields { textField.data(), constField.data() };

		HttpAuth auth("post", "https://www.google.com", fields, "cookie", "/index.php", "/login", {"csrf"}, {{"User-Agent", "Test"}});

		REQUIRE(auth.type() == QString("post"));
		REQUIRE(auth.name() == QString("post"));
		REQUIRE(auth.url() == QString("https://www.google.com"));
		REQUIRE(auth.fields() == fields);
		REQUIRE(auth.cookie() == QString("cookie"));
		REQUIRE(auth.redirectUrl() == QString("/index.php"));
		REQUIRE(auth.csrfUrl() == QString("/login"));
		REQUIRE(auth.csrfFields() == QStringList("csrf"));
		REQUIRE(auth.headers() == QMap<QString, QString>{{"User-Agent", "Test"}});

		REQUIRE(auth.settingFields().count() == 1);
		REQUIRE(auth.settingFields().first().id == textField->id());
	}

	SECTION("HTTP Basic auth")
	{
		HttpBasicAuth auth("http_basic", 25, "apiKey", "");

		REQUIRE(auth.type() == QString("http_basic"));
		REQUIRE(auth.name() == QString("http_basic"));
		REQUIRE(auth.maxPage() == 25);
		REQUIRE(auth.passwordType() == QString("apiKey"));
		REQUIRE(auth.tokenType() == QString("Basic"));

		QStringList settingFieldsIds;
		for (const auto &settingField : auth.settingFields()) {
			settingFieldsIds.append(settingField.id);
		}
		REQUIRE(settingFieldsIds == QStringList { "pseudo", "apiKey" });
	}

	SECTION("OAuth1 auth")
	{
		OAuth1Auth auth("oauth1", "https://www.google.com/credentials", "https://www.google.com/authorization", "https://www.google.com/token", "hmac-sha1");

		REQUIRE(auth.type() == QString("oauth1"));
		REQUIRE(auth.name() == QString("oauth1"));
		REQUIRE(auth.temporaryCredentialsUrl() == QString("https://www.google.com/credentials"));
		REQUIRE(auth.authorizationUrl() == QString("https://www.google.com/authorization"));
		REQUIRE(auth.tokenCredentialsUrl() == QString("https://www.google.com/token"));
		REQUIRE(auth.signatureMethod() == QString("hmac-sha1"));

		QStringList settingFieldsIds;
		for (const auto &settingField : auth.settingFields()) {
			settingFieldsIds.append(settingField.id);
		}
		REQUIRE(settingFieldsIds == QStringList { "consumerKey", "consumerSecret" });
	}

	SECTION("OAuth2 password auth")
	{
		OAuth2Auth auth("oauth2", "password", "https://www.google.com/token", "https://www.google.com/authorization", "https://www.google.com/redirect", "google");

		REQUIRE(auth.type() == QString("oauth2"));
		REQUIRE(auth.name() == QString("oauth2_password"));
		REQUIRE(auth.authType() == QString("password"));
		REQUIRE(auth.tokenUrl() == QString("https://www.google.com/token"));
		REQUIRE(auth.authorizationUrl() == QString("https://www.google.com/authorization"));
		REQUIRE(auth.redirectUrl() == QString("https://www.google.com/redirect"));
		REQUIRE(auth.urlProtocol() == QString("google"));

		QStringList settingFieldsIds;
		for (const auto &settingField : auth.settingFields()) {
			settingFieldsIds.append(settingField.id);
		}
		REQUIRE(settingFieldsIds == QStringList { "consumerKey", "consumerSecret", "pseudo", "password" });
	}

	SECTION("OAuth2 refresh token auth")
	{
		OAuth2Auth auth("oauth2", "refresh_token", "https://www.google.com/token", "https://www.google.com/authorization", "https://www.google.com/redirect", "google");

		REQUIRE(auth.type() == QString("oauth2"));
		REQUIRE(auth.name() == QString("oauth2_refresh_token"));
		REQUIRE(auth.authType() == QString("refresh_token"));
		REQUIRE(auth.tokenUrl() == QString("https://www.google.com/token"));
		REQUIRE(auth.authorizationUrl() == QString("https://www.google.com/authorization"));
		REQUIRE(auth.redirectUrl() == QString("https://www.google.com/redirect"));
		REQUIRE(auth.urlProtocol() == QString("google"));

		QStringList settingFieldsIds;
		for (const auto &settingField : auth.settingFields()) {
			settingFieldsIds.append(settingField.id);
		}
		REQUIRE(settingFieldsIds == QStringList { "consumerKey", "consumerSecret", "accessToken", "refreshToken" });
	}
}
