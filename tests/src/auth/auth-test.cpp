#include "auth-test.h"
#include <QtTest>
#include "auth/http-auth.h"
#include "auth/oauth2-auth.h"
#include "auth/url-auth.h"


void AuthTest::testUrlAuth()
{
	QList<AuthField*> fields;
	UrlAuth auth("url", fields, 50);

	QCOMPARE(auth.type(), QString("url"));
	QCOMPARE(auth.fields(), fields);
	QCOMPARE(auth.maxPage(), 50);
}

void AuthTest::testHttpAuth()
{
	QList<AuthField*> fields;
	HttpAuth auth("post", "https://www.google.com", fields, "cookie");

	QCOMPARE(auth.type(), QString("post"));
	QCOMPARE(auth.url(), QString("https://www.google.com"));
	QCOMPARE(auth.fields(), fields);
	QCOMPARE(auth.cookie(), QString("cookie"));
}

void AuthTest::testOAuth2Auth()
{
	OAuth2Auth auth("oauth2", "password", "https://www.google.com");

	QCOMPARE(auth.type(), QString("oauth2"));
	QCOMPARE(auth.authType(), QString("password"));
	QCOMPARE(auth.tokenUrl(), QString("https://www.google.com"));
}


QTEST_MAIN(AuthTest)
