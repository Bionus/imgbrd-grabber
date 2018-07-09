#include "tag-filter-test.h"
#include <QtTest>
#include "models/filtering/tag-filter.h"
#include "loader/token.h"


void TagFilterTest::testToString()
{
	QCOMPARE(TagFilter("test").toString(), QString("test"));
	QCOMPARE(TagFilter("test", true).toString(), QString("-test"));
}

void TagFilterTest::testMatch()
{
	QMap<QString, Token> tokens;
	tokens.insert("allos", Token(QStringList() << "ok" << "ok2"));

	// Basic
	QCOMPARE(TagFilter("ok").match(tokens), QString());
	QCOMPARE(TagFilter("nok").match(tokens), QString("image does not contains \"nok\""));

	// Invert
	QCOMPARE(TagFilter("ok", true).match(tokens), QString("image contains \"ok\""));
	QCOMPARE(TagFilter("nok", true).match(tokens), QString());
}


static TagFilterTest instance;
