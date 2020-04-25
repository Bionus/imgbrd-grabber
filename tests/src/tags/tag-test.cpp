#include <QJsonObject>
#include <algorithm>
#include "tags/tag.h"
#include "catch.h"


TEST_CASE("Tag")
{
	SECTION("DefaultConstructor")
	{
		Tag tag;

		REQUIRE(tag.text() == QString());
	}

	SECTION("Id")
	{
		Tag tag(123, "tag_text", TagType("artist"), 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.id() == 123);
	}
	SECTION("IdDefault")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.id() == 0);
	}
	SECTION("SetId")
	{
		Tag tag(123, "tag_not_text", TagType("artist"), 123, QStringList() << "related1" << "related2" << "related3");
		tag.setId(456);

		REQUIRE(tag.id() == 456);
	}

	SECTION("Text")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.text() == QString("tag_text"));
	}
	SECTION("SetText")
	{
		Tag tag("tag_not_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
		tag.setText("tag_text");

		REQUIRE(tag.text() == QString("tag_text"));
	}

	SECTION("Type")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.type().name() == QString("artist"));
	}
	SECTION("TypeArtistEnding")
	{
		Tag tag("tag_text (artist)", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.type().name() == QString("artist"));
		REQUIRE(tag.text() == QString("tag_text"));
	}
	SECTION("TypePrefix")
	{
		Tag tag("artist:tag_text", "", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.type().name() == QString("artist"));
		REQUIRE(tag.text() == QString("tag_text"));
	}
	SECTION("TypePrefixSpecies")
	{
		Tag tag("species:tag_text", "unknown", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.type().name() == QString("species"));
		REQUIRE(tag.text() == QString("tag_text"));
	}
	SECTION("ShortType")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.type().number() == 1);
	}
	SECTION("SetType")
	{
		Tag tag("tag_not_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
		tag.setType(TagType("copyright"));

		REQUIRE(tag.type().name() == QString("copyright"));
	}

	SECTION("Count")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.count() == 123);
	}
	SECTION("SetCount")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");
		tag.setCount(1234);

		REQUIRE(tag.count() == 1234);
	}

	SECTION("Related")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.related() == QStringList() << "related1" << "related2" << "related3");
	}
	SECTION("SetRelated")
	{
		Tag tag("tag_text", "artist", 123, QStringList() << "related1" << "related2");
		tag.setRelated(QStringList() << "related1" << "related2" << "related3");

		REQUIRE(tag.related() == QStringList() << "related1" << "related2" << "related3");
	}

	SECTION("Compare")
	{
		Tag tag1("artist1", "artist", 1, QStringList() << "tag1");
		Tag tag2("artist1", "artist", 2, QStringList() << "tag2");
		Tag tag3("artist2", "artist", 3, QStringList() << "tag3");
		Tag tag4("artist1", "character", 4, QStringList() << "tag4");
		Tag tag5("artist1", "unknown", 5, QStringList() << "tag5");

		REQUIRE(tag1 == tag1);
		REQUIRE(tag1 == tag2);
		REQUIRE(tag1 != tag3);
		REQUIRE(tag1 != tag4);
		REQUIRE(tag1 == tag5);
	}

	SECTION("SortTagsByType")
	{
		QList<Tag> tagList;
		tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
		tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
		tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
		tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
		tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

		std::sort(tagList.begin(), tagList.end(), sortTagsByType);

		REQUIRE(tagList[0].text() == QString("third"));
		REQUIRE(tagList[1].text() == QString("second"));
		REQUIRE(tagList[2].text() == QString("last"));
		REQUIRE(tagList[3].text() == QString("first"));
		REQUIRE(tagList[4].text() == QString("fourth"));
	}
	SECTION("SortTagsByName")
	{
		QList<Tag> tagList;
		tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
		tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
		tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
		tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
		tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

		std::sort(tagList.begin(), tagList.end(), sortTagsByName);

		REQUIRE(tagList[0].text() == QString("first"));
		REQUIRE(tagList[1].text() == QString("fourth"));
		REQUIRE(tagList[2].text() == QString("last"));
		REQUIRE(tagList[3].text() == QString("second"));
		REQUIRE(tagList[4].text() == QString("third"));
	}
	SECTION("SortTagsByCount")
	{
		QList<Tag> tagList;
		tagList.append(Tag("last", "artist", 1, QStringList() << "tag1"));
		tagList.append(Tag("fourth", "general", 2, QStringList() << "tag2"));
		tagList.append(Tag("third", "copyright", 3, QStringList() << "tag3"));
		tagList.append(Tag("second", "character", 4, QStringList() << "tag4"));
		tagList.append(Tag("first", "unknown", 5, QStringList() << "tag5"));

		std::sort(tagList.begin(), tagList.end(), sortTagsByCount);

		REQUIRE(tagList[0].text() == QString("first"));
		REQUIRE(tagList[1].text() == QString("second"));
		REQUIRE(tagList[2].text() == QString("third"));
		REQUIRE(tagList[3].text() == QString("fourth"));
		REQUIRE(tagList[4].text() == QString("last"));
	}

	SECTION("TypeSpaced")
	{
		Tag tag("artist1", "artist with spaces", 1, QStringList() << "tag1");

		REQUIRE(tag.type().name() == QString("artist"));
	}

	SECTION("GetType")
	{
		auto ids = QMap<int, QString>
		{
			{ 0, "type1" },
			{ 1, "type2" },
			{ 2, "type3" },
		};

		// Basic types
		REQUIRE(Tag::GetType("copyright", ids) == QString("copyright"));
		REQUIRE(Tag::GetType("artist", ids) == QString("artist"));

		// Type IDs
		REQUIRE(Tag::GetType("0", ids) == QString("type1"));
		REQUIRE(Tag::GetType("1", ids) == QString("type2"));
		REQUIRE(Tag::GetType("5", ids) == QString("5"));
		REQUIRE(Tag::GetType("-1", ids) == QString("-1"));

		// Replacements
		REQUIRE(Tag::GetType("series", ids) == QString("copyright"));
		REQUIRE(Tag::GetType("mangaka", ids) == QString("artist"));
		REQUIRE(Tag::GetType("game", ids) == QString("copyright"));
		REQUIRE(Tag::GetType("studio", ids) == QString("circle"));
		REQUIRE(Tag::GetType("source", ids) == QString("general"));
		REQUIRE(Tag::GetType("character group", ids) == QString("general"));

		// Mixed types
		REQUIRE(Tag::GetType("copyright, character", ids) == QString("copyright"));
	}

	SECTION("Serialization")
	{
		Tag original(123, "tag", TagType("type"), 456, QStringList() << "rel 1" << "rel 2");

		QJsonObject json;
		original.write(json);

		Tag dest;
		dest.read(json);

		REQUIRE(dest.id() == original.id());
		REQUIRE(dest.text() == original.text());
		REQUIRE(dest.type() == original.type());
		REQUIRE(dest.count() == original.count());
		REQUIRE(dest.related() == original.related());
	}
}
