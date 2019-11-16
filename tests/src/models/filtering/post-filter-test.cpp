#include <QDir>
#include <QFile>
#include <QPointer>
#include "loader/token.h"
#include "models/filtering/post-filter.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
#include "catch.h"
#include "source-helpers.h"


TEST_CASE("PostFilter")
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp")) {
		tmp.mkdir("tmp");
	}

	QFile::remove("tests/resources/md5s.txt");

	QMap<QString, QString> details;
	details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	details["ext"] = "jpg";
	details["author"] = "superauthor";
	details["status"] = "tested";
	details["filename"] = "";
	details["folder"] = "";
	details["search"] = "testing well";
	details["id"] = "7331";
	details["score"] = "21";
	details["parent_id"] = "1337";
	details["file_size"] = "1234567";
	details["creator_id"] = "1234";
	details["has_children"] = "true";
	details["has_note"] = "true";
	details["has_comments"] = "true";
	details["file_url"] = "http://test.com/img/oldfilename.jpg?123456";
	details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	details["page_url"] = "";
	details["width"] = "800";
	details["height"] = "600";
	details["source"] = "http://google.com/toto/toto.jpg";
	details["tags_general"] = "tag1 tag2 tag3 ";
	details["tags_artist"] = "artist1 ";
	details["tags_copyright"] = "copyright1 copyright2 ";
	details["tags_character"] = "character1 character2 ";
	details["tags_model"] = "model1 ";
	details["created_at"] = "1471513944";
	details["rating"] = "safe";
	details["file_size"] = "358400";
	details["file_size"] = "358400";

	auto profile = QPointer<Profile>(makeProfile());
	Site *site = profile->getSites().value("danbooru.donmai.us");
	REQUIRE(site != nullptr);

	Image *img = new Image(site, details, profile);

	SECTION("Count")
	{
		REQUIRE(PostFilter(QStringList() << "id:<=10000" << "width:>100" << "date:<2017-01-01").count() == 3);
		REQUIRE(PostFilter(QStringList() << "" << "id:<=10000").count() == 1);
	}

	SECTION("FilterNumeric")
	{
		auto tokens = img->tokens(profile);

		QStringList filters;

		// No match
		filters = PostFilter(QStringList() << "id:<=10000" << "width:>100" << "date:<2017-01-01").match(tokens);
		REQUIRE(filters == QStringList());

		// All match
		filters = PostFilter(QStringList() << "id:>10000" << "width:<=100" << "date:>=2017-01-01").match(tokens);
		REQUIRE(filters == QStringList() << "image's id does not match" << "image's width does not match" << "image's date does not match");
	}

	SECTION("FilterSpecial")
	{
		auto tokens = img->tokens(profile);

		QStringList filters;

		// No match
		filters = PostFilter(QStringList() << "rating:s" << "rating:safe" << "source:http://google.com").match(tokens);
		REQUIRE(filters == QStringList());

		// All match
		filters = PostFilter(QStringList() << "rating:e" << "rating:explicit" << "source:http://test.com").match(tokens);
		REQUIRE(filters == QStringList() << "image is not \"explicit\"" << "image is not \"explicit\"" << "image's source does not starts with \"http://test.com\"");
	}

	SECTION("FilterInvert")
	{
		auto tokens = img->tokens(profile);

		QStringList filters;

		// No match
		filters = PostFilter(QStringList() << "-id:>10000" << "-width:<=100" << "-date:>=2017-01-01").match(tokens);
		REQUIRE(filters == QStringList());

		// All match
		filters = PostFilter(QStringList() << "-id:<=10000" << "-width:>100" << "-date:<2017-01-01").match(tokens);
		REQUIRE(filters == QStringList() << "image's id match" << "image's width match" << "image's date match");
	}
}
