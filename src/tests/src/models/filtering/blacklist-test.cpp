#include "loader/token.h"
#include "models/filtering/blacklist.h"
#include "catch.h"


TEST_CASE("Blacklist")
{
	SECTION("ToString")
	{
		Blacklist blacklist;
		blacklist.add("tag1");
		blacklist.add(QStringList() << "tag2" << "tag3");
		blacklist.add("tag4");

		REQUIRE(blacklist.toString() == QString("tag1\ntag2 tag3\ntag4"));
	}

	SECTION("Contains")
	{
		Blacklist blacklist(QStringList() << "tag1" << "tag2");

		REQUIRE(blacklist.contains("tag1") == true);
		REQUIRE(blacklist.contains("tag2") == true);
		REQUIRE(blacklist.contains("not_found") == false);
	}

	SECTION("Remove")
	{
		Blacklist blacklist(QStringList() << "tag1" << "tag2");

		// Remove should only work once
		REQUIRE(blacklist.remove("tag2") == true);
		REQUIRE(blacklist.remove("tag2") == false);

		// The list should not contain "tag2" anymore
		REQUIRE(blacklist.contains("tag1") == true);
		REQUIRE(blacklist.contains("tag2") == false);
		REQUIRE(blacklist.contains("not_found") == false);
	}

	SECTION("Match")
	{
		QMap<QString, Token> tokens;
		tokens.insert("allos", Token(QStringList() << "tag1" << "tag2" << "tag3" << "artist1" << "copyright1" << "copyright2" << "character1" << "character2" << "model1"));

		// Basic
		REQUIRE(Blacklist(QStringList() << "tag8" << "tag7").match(tokens) == QStringList());
		REQUIRE(Blacklist(QStringList() << "tag1" << "tag7").match(tokens) == QStringList() << "tag1");
		REQUIRE(Blacklist(QStringList() << "character1" << "artist1").match(tokens) == QStringList() << "character1" << "artist1");

		// Invert
		REQUIRE(Blacklist(QStringList() << "tag8" << "tag7").match(tokens, false) == QStringList() << "tag8" << "tag7");
		REQUIRE(Blacklist(QStringList() << "tag1" << "tag7").match(tokens, false) == QStringList() << "tag7");
		REQUIRE(Blacklist(QStringList() << "character1" << "artist1").match(tokens, false) == QStringList());
	}

	SECTION("Escaping colon in tags")
	{
		Blacklist blacklist(QStringList() << "re::zero");

		REQUIRE(blacklist.contains("re:zero") == true);
		REQUIRE(blacklist.contains("re::zero") == false);

		QMap<QString, Token> tokensWith;
		tokensWith.insert("allos", Token(QStringList() << "tag1" << "re:zero"));
		QMap<QString, Token> tokensWithout;
		tokensWithout.insert("allos", Token(QStringList() << "tag1" << "tag2"));

		REQUIRE(blacklist.match(tokensWith) == QStringList("re:zero"));
		REQUIRE(blacklist.match(tokensWithout) == QStringList());
	}
}
