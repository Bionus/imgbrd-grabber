#include <QtTest>
#include "filename-test.h"


void FilenameTest::init()
{
    QMap<QString,QString> details;
    details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
    details["ext"] = "jpg";
    details["author"] = "superauthor";
    details["status"] = "tested";
    details["filename"] = "oldfilename";
    details["search"] = "testing well";
    details["id"] = "7331";
    details["score"] = "21";
    details["parent_id"] = "1337";
    details["file_size"] = "1234567";
    details["creator_id"] = "1234";
    details["has_children"] = "true";
    details["has_note"] = "true";
    details["has_comments"] = "true";
    details["file_url"] = "http://test.com/img/oldfilename.jpg";
    details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
    details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
    details["width"] = "800";
    details["height"] = "600";
    details["source"] = "http://google.com/";
    details["tags_general"] = "tag1 tag2 tag3";
    details["tags_artist"] = "artist1";
    details["tags_character"] = "character1 character2";
    details["tags_copyright"] = "copyright1";

	m_settings = new QSettings("tests/test_settings.ini", QSettings::IniFormat);
	m_site = new Site(m_settings, "release/sites/Danbooru (2.0)", "danbooru.donmai.us");
	m_img = new Image(m_site, details);
}

void FilenameTest::cleanup()
{
    delete m_settings;
    delete m_site;
    delete m_img;
}


void FilenameTest::testGetFormat()
{
    QString format = "%md5%.%ext%";
    Filename fn(format);

    QCOMPARE(fn.getFormat(), format);
}

void FilenameTest::testPathSimple()
{
    assertPath("%md5%.%ext%", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testPathSimpleJavascript()
{
    assertPath("javascript:md5 + '.' + ext", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testExpand()
{
    assertExpand("<image contains the tag \"tag1\"><\"unknown\" is one of the image tags> %md5%.%ext%",
                 "image contains the tag tag1 %md5%.%ext%");
    assertExpand("<image contains the tag \"unknown\"><\"tag2\" is one of the image tags> %md5%.%ext%",
                 "tag2 is one of the image tags %md5%.%ext%");
    assertExpand("<image contains the tag \"tag1\"><\"tag2\" is one of the image tags> %md5%.%ext%",
                 "image contains the tag tag1tag2 is one of the image tags %md5%.%ext%");
    assertExpand("<image contains the tag \"unknown1\"><\"unknown2\" is one of the image tags> %md5%.%ext%",
                 " %md5%.%ext%");
}


void FilenameTest::assertPath(QString format, QString expected, QString path)
{
    assertPath(format, QStringList() << expected, path);
}

void FilenameTest::assertPath(QString format, QStringList expected, QString path)
{
    Filename fn(format);
    QCOMPARE(fn.path(*m_img, path), expected);
}


void FilenameTest::assertExpand(QString format, QString expected)
{
    QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << "date" << "date:([^%]+)" << "count(:\\d+)?(:\\d+)?" << "search_(\\d+)" << "score" << "height" << "width" << "path" << "pool" << "url_file" << "url_page";

    Filename fn(format);
    QCOMPARE(fn.expandConditionals(format, tokens, m_img->tagsString()), expected);
}

static FilenameTest instance;
