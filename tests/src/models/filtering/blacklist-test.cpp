#include "blacklist-test.h"
#include <QtTest>
#include "loader/token.h"
#include "models/filtering/blacklist.h"


void BlacklistTest::testToString()
{
	Blacklist blacklist;
	blacklist.add("tag1");
	blacklist.add(QStringList() << "tag2" << "tag3");
	blacklist.add("tag4");

	QCOMPARE(blacklist.toString(), QString("tag1\ntag2 tag3\ntag4"));
}

void BlacklistTest::testContains()
{
	Blacklist blacklist(QStringList() << "tag1" << "tag2");

	QCOMPARE(blacklist.contains("tag1"), true);
	QCOMPARE(blacklist.contains("tag2"), true);
	QCOMPARE(blacklist.contains("not_found"), false);
}

void BlacklistTest::testRemove()
{
	Blacklist blacklist(QStringList() << "tag1" << "tag2");

	// Remove should only work once
	QCOMPARE(blacklist.remove("tag2"), true);
	QCOMPARE(blacklist.remove("tag2"), false);

	// The list should not contain "tag2" anymore
	QCOMPARE(blacklist.contains("tag1"), true);
	QCOMPARE(blacklist.contains("tag2"), false);
	QCOMPARE(blacklist.contains("not_found"), false);
}

void BlacklistTest::testMatch()
{
	QMap<QString, Token> tokens;
	tokens.insert("allos", Token(QStringList() << "tag1" << "tag2" << "tag3" << "artist1" << "copyright1" << "copyright2" << "character1" << "character2" << "model1"));

	// Basic
	QCOMPARE(Blacklist(QStringList() << "tag8" << "tag7").match(tokens), QStringList());
	QCOMPARE(Blacklist(QStringList() << "tag1" << "tag7").match(tokens), QStringList() << "tag1");
	QCOMPARE(Blacklist(QStringList() << "character1" << "artist1").match(tokens), QStringList() << "character1" << "artist1");

	// Invert
	QCOMPARE(Blacklist(QStringList() << "tag8" << "tag7").match(tokens, false), QStringList() << "tag8" << "tag7");
	QCOMPARE(Blacklist(QStringList() << "tag1" << "tag7").match(tokens, false), QStringList() << "tag7");
	QCOMPARE(Blacklist(QStringList() << "character1" << "artist1").match(tokens, false), QStringList());
}


QTEST_MAIN(BlacklistTest)
