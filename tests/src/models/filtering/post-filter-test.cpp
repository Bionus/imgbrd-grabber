#include "post-filter-test.h"
#include <QtTest>
#include "loader/token.h"
#include "models/filtering/post-filter.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "models/source.h"
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


void PostFilterTest::testCount()
{
	QCOMPARE(PostFilter(QStringList() << "id:<=10000" << "width:>100" << "date:<2017-01-01").count(), 3);
	QCOMPARE(PostFilter(QStringList() << "" << "id:<=10000").count(), 1);
}

void PostFilterTest::testFilterNumeric()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter(QStringList() << "id:<=10000" << "width:>100" << "date:<2017-01-01").match(tokens);
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter(QStringList() << "id:>10000" << "width:<=100" << "date:>=2017-01-01").match(tokens);
	QCOMPARE(filters, QStringList() << "image's id does not match" << "image's width does not match" << "image's date does not match");
}

void PostFilterTest::testFilterSpecial()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter(QStringList() << "rating:s" << "rating:safe" << "source:http://google.com").match(tokens);
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter(QStringList() << "rating:e" << "rating:explicit" << "source:http://test.com").match(tokens);
	QCOMPARE(filters, QStringList() << "image is not \"explicit\"" << "image is not \"explicit\"" << "image's source does not starts with \"http://test.com\"");
}

void PostFilterTest::testFilterInvert()
{
	auto tokens = m_img->tokens(m_profile);

	QStringList filters;

	// No match
	filters = PostFilter(QStringList() << "-id:>10000" << "-width:<=100" << "-date:>=2017-01-01").match(tokens);
	QCOMPARE(filters, QStringList());

	// All match
	filters = PostFilter(QStringList() << "-id:<=10000" << "-width:>100" << "-date:<2017-01-01").match(tokens);
	QCOMPARE(filters, QStringList() << "image's id match" << "image's width match" << "image's date match");
}


QTEST_MAIN(PostFilterTest)
