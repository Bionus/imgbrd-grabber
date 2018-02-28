#include "post-filter-test.h"
#include <QtTest>
#include "models/post-filter.h"
#include "test-suite.h"


void PostFilterTest::init()
{
	// Make tmp dir if not already existing
	QDir tmp("tests/resources/");
	if (!tmp.exists("tmp"))
		tmp.mkdir("tmp");

	QFile::remove("tests/resources/md5s.txt");

	m_details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
	m_details["ext"] = "jpg";
	m_details["author"] = "superauthor";
	m_details["status"] = "tested";
	m_details["filename"] = "";
	m_details["folder"] = "";
	m_details["search"] = "testing well";
	m_details["id"] = "7331";
	m_details["score"] = "21";
	m_details["parent_id"] = "1337";
	m_details["file_size"] = "1234567";
	m_details["creator_id"] = "1234";
	m_details["has_children"] = "true";
	m_details["has_note"] = "true";
	m_details["has_comments"] = "true";
	m_details["file_url"] = "http://test.com/img/oldfilename.jpg?123456";
	m_details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
	m_details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
	m_details["page_url"] = "";
	m_details["width"] = "800";
	m_details["height"] = "600";
	m_details["source"] = "http://google.com/toto/toto.jpg";
	m_details["tags_general"] = "tag1 tag2 tag3 ";
	m_details["tags_artist"] = "artist1 ";
	m_details["tags_copyright"] = "copyright1 copyright2 ";
	m_details["tags_character"] = "character1 character2 ";
	m_details["tags_model"] = "model1 ";
	m_details["created_at"] = "1471513944";
	m_details["rating"] = "safe";
	m_details["file_size"] = "358400";
	m_details["file_size"] = "358400";

	m_profile = new Profile("tests/resources/");
	m_source = new Source(m_profile, "release/sites/Danbooru (2.0)");
	m_site = new Site("danbooru.donmai.us", m_source);
	m_img = new Image(m_site, m_details, m_profile);
}

void PostFilterTest::cleanup()
{
	m_profile->deleteLater();
	m_source->deleteLater();
	m_site->deleteLater();
	m_img->deleteLater();
}


void PostFilterTest::testBlacklisted()
{
	auto tokens = m_img->tokens(m_profile);

	// Basic
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "tag8" << "tag7"), QStringList());
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "tag1" << "tag7"), QStringList() << "tag1");
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "character1" << "artist1"), QStringList() << "character1" << "artist1");

	// Invert
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "tag8" << "tag7", false), QStringList() << "tag8" << "tag7");
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "tag1" << "tag7", false), QStringList() << "tag7");
	QCOMPARE(PostFilter::blacklisted(tokens, QStringList() << "character1" << "artist1", false), QStringList());
}

void PostFilterTest::testMatchToken()
{
	auto tokens = m_img->tokens(m_profile);

	// Basic
	QCOMPARE(PostFilter::match(tokens, "%copyright%"), QString());
	QCOMPARE(PostFilter::match(tokens, "%toto%"), QString("image does not have a \"toto\" token"));

	// Minus
	QCOMPARE(PostFilter::match(tokens, "-%copyright%"), QString("image has a \"copyright\" token"));
	QCOMPARE(PostFilter::match(tokens, "-%toto%"), QString());

	// Invert
	QCOMPARE(PostFilter::match(tokens, "%copyright%", true), QString("image has a \"copyright\" token"));
	QCOMPARE(PostFilter::match(tokens, "%toto%", true), QString());

	// Invert minus
	QCOMPARE(PostFilter::match(tokens, "-%copyright%", true), QString());
	QCOMPARE(PostFilter::match(tokens, "-%toto%", true), QString("image does not have a \"toto\" token"));
}

void PostFilterTest::testMatchTag()
{
	auto tokens = m_img->tokens(m_profile);

	// Basic
	QCOMPARE(PostFilter::match(tokens, "tag1"), QString());
	QCOMPARE(PostFilter::match(tokens, "character1"), QString());
	QCOMPARE(PostFilter::match(tokens, "tag7"), QString("image does not contains \"tag7\""));

	// Minus
	QCOMPARE(PostFilter::match(tokens, "-tag1"), QString("image contains \"tag1\""));
	QCOMPARE(PostFilter::match(tokens, "-character1"), QString("image contains \"character1\""));
	QCOMPARE(PostFilter::match(tokens, "-tag7"), QString());

	// Invert
	QCOMPARE(PostFilter::match(tokens, "tag1", true), QString("image contains \"tag1\""));
	QCOMPARE(PostFilter::match(tokens, "character1", true), QString("image contains \"character1\""));
	QCOMPARE(PostFilter::match(tokens, "tag7", true), QString());

	// Invert minus
	QCOMPARE(PostFilter::match(tokens, "-tag1", true), QString());
	QCOMPARE(PostFilter::match(tokens, "-character1", true), QString());
	QCOMPARE(PostFilter::match(tokens, "-tag7", true), QString("image does not contains \"tag7\""));
}

void PostFilterTest::testMatchUnknown()
{
	auto tokens = m_img->tokens(m_profile);

	QCOMPARE(PostFilter::match(tokens, "toto:test").startsWith("unknown type \"toto\""), true);
}

void PostFilterTest::testMatchMathematical()
{
	auto tokens = m_img->tokens(m_profile);

	// Basic
	QCOMPARE(PostFilter::match(tokens, "id:>1000"), QString());
	QCOMPARE(PostFilter::match(tokens, "id:<=1000"), QString("image's id does not match"));
	QCOMPARE(PostFilter::match(tokens, "id:>=0", true), QString("image's id match"));

	// Other types
	QCOMPARE(PostFilter::match(tokens, "width:..1000"), QString());
	QCOMPARE(PostFilter::match(tokens, "height:500.."), QString());
	QCOMPARE(PostFilter::match(tokens, "score:10..30"), QString());
	QCOMPARE(PostFilter::match(tokens, "mpixels:<1000000"), QString());
	QCOMPARE(PostFilter::match(tokens, "filesize:358400"), QString());
}

void PostFilterTest::testMatchDate()
{
	auto tokens = m_img->tokens(m_profile);

	QCOMPARE(PostFilter::match(tokens, "date:>08/16/2016"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:>=2016-08-16"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:<08/20/2016"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:<=2016-08-20"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:..08/20/2016"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:2016-08-16.."), QString());
	QCOMPARE(PostFilter::match(tokens, "date:08/16/2016..2016-08-20"), QString());
	QCOMPARE(PostFilter::match(tokens, "date:2016-08-18"), QString());

	// Invalid date
	QCOMPARE(PostFilter::match(tokens, "date:someday"), QString("image's date does not match"));
}

void PostFilterTest::testMatchRating()
{
	auto tokens = m_img->tokens(m_profile);

	// Basic
	QCOMPARE(PostFilter::match(tokens, "rating:safe"), QString());
	QCOMPARE(PostFilter::match(tokens, "rating:explicit"), QString("image is not \"explicit\""));

	// Short versions
	QCOMPARE(PostFilter::match(tokens, "rating:s"), QString());
	QCOMPARE(PostFilter::match(tokens, "rating:e"), QString("image is not \"explicit\""));

	// Invert
	QCOMPARE(PostFilter::match(tokens, "rating:safe", true), QString("image is \"safe\""));
	QCOMPARE(PostFilter::match(tokens, "rating:explicit", true), QString());
}

void PostFilterTest::testMatchSource()
{
	auto tokens = m_img->tokens(m_profile);

	// Full
	QCOMPARE(PostFilter::match(tokens, "source:http://google.com/toto/toto.jpg"), QString());
	QCOMPARE(PostFilter::match(tokens, "source:http://test.fr/toto/toto.jpg"), QString("image's source does not starts with \"http://test.fr/toto/toto.jpg\""));

	// Short
	QCOMPARE(PostFilter::match(tokens, "source:http://google.com"), QString());
	QCOMPARE(PostFilter::match(tokens, "source:http://test.fr"), QString("image's source does not starts with \"http://test.fr\""));

	// Invert
	QCOMPARE(PostFilter::match(tokens, "source:http://google.com", true), QString("image's source starts with \"http://google.com\""));
	QCOMPARE(PostFilter::match(tokens, "source:http://test.fr", true), QString());
}

void PostFilterTest::testFilterNumeric()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter::filter(tokens, QStringList() << "id:<=10000" << "width:>100" << "date:<2017-01-01");
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter::filter(tokens, QStringList() << "id:>10000" << "width:<=100" << "date:>=2017-01-01");
	QCOMPARE(filters, QStringList() << "image's id does not match" << "image's width does not match" << "image's date does not match");
}
void PostFilterTest::testFilterString()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter::filter(tokens, QStringList() << "filetype:jpg");
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter::filter(tokens, QStringList() << "filetype:png");
	QCOMPARE(filters, QStringList() << "image's filetype does not match");

	// Invert
	filters = PostFilter::filter(tokens, QStringList() << "-filetype:jpg");
	QCOMPARE(filters, QStringList() << "image's filetype match");
}
void PostFilterTest::testFilterSpecial()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter::filter(tokens, QStringList() << "rating:s" << "rating:safe" << "source:http://google.com");
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter::filter(tokens, QStringList() << "rating:e" << "rating:explicit" << "source:http://test.com");
	QCOMPARE(filters, QStringList() << "image is not \"explicit\"" << "image is not \"explicit\"" << "image's source does not starts with \"http://test.com\"");
}
void PostFilterTest::testFilterInvert()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter::filter(tokens, QStringList() << "-id:>10000" << "-width:<=100" << "-date:>=2017-01-01");
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter::filter(tokens, QStringList() << "-id:<=10000" << "-width:>100" << "-date:<2017-01-01");
	QCOMPARE(filters, QStringList() << "image's id match" << "image's width match" << "image's date match");
}


static PostFilterTest instance;
