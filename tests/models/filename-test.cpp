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
    details["tags_copyright"] = "copyright1 copyright2";
    details["tags_character"] = "character1 character2";
    details["created_at"] = "1471513944";

	m_settings = new QSettings("tests/test_settings.ini", QSettings::IniFormat);
	m_settings->setValue("Save/character_value", "group");
	m_settings->setValue("Save/character_multiple", "replaceAll");
	m_settings->setValue("Save/copyright_value", "crossover");
	m_settings->setValue("Save/copyright_multiple", "replaceAll");

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
void FilenameTest::testPathComplex()
{
    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/group/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testPathSimpleJavascript()
{
    assertPath("javascript:md5 + '.' + ext", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testExpandTag()
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
void FilenameTest::testExpandTokenSimple()
{
    assertExpand("image - <%artist% some text><text %nothing%> %md5%.%ext%",
                 "image - %artist% some text %md5%.%ext%");
}
void FilenameTest::testExpandTokenComplex()
{
    /*assertExpand("image - <%artist% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
                 "image - %artist% some text  test %md5%.%ext%");
    assertExpand("image - <%model% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%",
                 "image -  %md5%.%ext%");*/
}

void FilenameTest::testPathOptionMax()
{
    assertPath("%md5:maxlength=8%.%ext%", "1bc29b36.jpg");
}
void FilenameTest::testPathOptionMaxDouble()
{
    assertPath("%md5:maxlength=16,maxlength=8%.%ext%", "1bc29b36.jpg");
}
void FilenameTest::testPathOptionDateFormat()
{
    assertPath("%date:format=yyyy-MM-dd%.%ext%", "2016-08-18.jpg");
}

void FilenameTest::testGetReplacesSimple()
{
    QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

    Filename fn(format);
    m_settings->setValue("Save/character_multiple", "replaceAll");
    QList<QMap<QString, QPair<QString, QString>>> replaces = fn.getReplaces(format, *m_img, m_settings, QMap<QString, QStringList>());

    QCOMPARE(replaces.count(), 1);
    QCOMPARE(replaces[0]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[0]["copyright"].first, QString("crossover"));
    QCOMPARE(replaces[0]["character"].first, QString("group"));
}
void FilenameTest::testGetReplacesMultiple()
{
    QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

    Filename fn(format);
    m_settings->setValue("Save/character_multiple", "multiple");
    QList<QMap<QString, QPair<QString, QString>>> replaces = fn.getReplaces(format, *m_img, m_settings, QMap<QString, QStringList>());

    QCOMPARE(replaces.count(), 2);
    QCOMPARE(replaces[0]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[0]["copyright"].first, QString("crossover"));
    QCOMPARE(replaces[0]["character"].first, QString("character1"));
    QCOMPARE(replaces[1]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[1]["copyright"].first, QString("crossover"));
    QCOMPARE(replaces[1]["character"].first, QString("character2"));
}
void FilenameTest::testGetReplacesMatrix()
{
    QString format = "%artist%/%copyright%/%character%/%md5%.%ext%";

    Filename fn(format);
    m_settings->setValue("Save/character_multiple", "multiple");
    m_settings->setValue("Save/copyright_multiple", "multiple");
    QList<QMap<QString, QPair<QString, QString>>> replaces = fn.getReplaces(format, *m_img, m_settings, QMap<QString, QStringList>());

    QCOMPARE(replaces.count(), 4);
    QCOMPARE(replaces[0]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[0]["copyright"].first, QString("copyright1"));
    QCOMPARE(replaces[0]["character"].first, QString("character1"));
    QCOMPARE(replaces[1]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[1]["copyright"].first, QString("copyright2"));
    QCOMPARE(replaces[1]["character"].first, QString("character1"));
    QCOMPARE(replaces[2]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[2]["copyright"].first, QString("copyright1"));
    QCOMPARE(replaces[2]["character"].first, QString("character2"));
    QCOMPARE(replaces[3]["artist"].first, QString("artist1"));
    QCOMPARE(replaces[3]["copyright"].first, QString("copyright2"));
    QCOMPARE(replaces[3]["character"].first, QString("character2"));
}


void FilenameTest::assertPath(QString format, QString expected, QString path)
{
    assertPath(format, QStringList() << expected, path);
}

void FilenameTest::assertPath(QString format, QStringList expected, QString path)
{
    if (path.isEmpty())
        path = QDir::homePath();

    // Convert directory separators
    QStringList expectedNative;
    for (QString exp : expected)
    {
        expectedNative.append(QDir::toNativeSeparators(exp));
    }

    Filename fn(format);
    QStringList actual = fn.path(*m_img, m_settings, path);
    QCOMPARE(actual, expectedNative);
}


void FilenameTest::assertExpand(QString format, QString expected)
{
    QStringList tokens = QStringList() << "artist" << "general" << "copyright" << "character" << "model" << "model|artist" << "filename" << "rating" << "md5" << "website" << "ext" << "all" << "id" << "search" << "allo" << "date" << "date:([^%]+)" << "count(:\\d+)?(:\\d+)?" << "search_(\\d+)" << "score" << "height" << "width" << "path" << "pool" << "url_file" << "url_page";

    Filename fn(format);
    QMap<QString, QPair<QString, QString>> replaces = fn.getReplaces(format, *m_img, m_settings, QMap<QString, QStringList>()).first();
    QString actual = fn.expandConditionals(format, tokens, m_img->tagsString(), replaces);
    QCOMPARE(actual, expected);
}

static FilenameTest instance;
