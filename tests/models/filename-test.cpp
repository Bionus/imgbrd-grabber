#include <QtTest>
#include "filename-test.h"


void FilenameTest::init()
{
    m_details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
    m_details["ext"] = "jpg";
    m_details["author"] = "superauthor";
    m_details["status"] = "tested";
    m_details["filename"] = "oldfilename";
    m_details["search"] = "testing well";
    m_details["id"] = "7331";
    m_details["score"] = "21";
    m_details["parent_id"] = "1337";
    m_details["file_size"] = "1234567";
    m_details["creator_id"] = "1234";
    m_details["has_children"] = "true";
    m_details["has_note"] = "true";
    m_details["has_comments"] = "true";
    m_details["file_url"] = "http://test.com/img/oldfilename.jpg";
    m_details["sample_url"] = "http://test.com/sample/oldfilename.jpg";
    m_details["preview_url"] = "http://test.com/preview/oldfilename.jpg";
    m_details["width"] = "800";
    m_details["height"] = "600";
    m_details["source"] = "http://google.com/";
    m_details["tags_general"] = "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3";
    m_details["tags_artist"] = "artist1";
    m_details["tags_copyright"] = "copyright1 copyright2";
    m_details["tags_character"] = "character1 character2";
    m_details["created_at"] = "1471513944";

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
	m_settings->clear();
	m_settings->setValue("ignoredtags", "");
	m_settings->setValue("Save/separator", " ");
	m_settings->setValue("Save/character_value", "group");
	m_settings->setValue("Save/character_multiple", "replaceAll");
	m_settings->setValue("Save/copyright_value", "crossover");
	m_settings->setValue("Save/copyright_multiple", "replaceAll");
	m_settings->setValue("Save/replaceblanks", true);

	m_site = new Site(m_settings, "release/sites/Danbooru (2.0)", "danbooru.donmai.us");
	m_img = new Image(m_site, m_details);
}

void FilenameTest::cleanup()
{
    m_settings->deleteLater();
    m_site->deleteLater();
    m_img->deleteLater();
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
void FilenameTest::testPathKeepAll()
{
    m_settings->setValue("Save/character_multiple", "keepAll");

    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathKeepN()
{
    m_settings->setValue("Save/character_multiple", "keepN");
    m_settings->setValue("Save/character_multiple_keepN", 1);

    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/character1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathKeepNThenAdd()
{
    m_settings->setValue("Save/character_multiple", "keepNThenAdd");
    m_settings->setValue("Save/character_multiple_keepNThenAdd_keep", 1);
    m_settings->setValue("Save/character_multiple_keepNThenAdd_add", " (and %count% of %total%)");
    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/character1 (and 1 of 2)/1bc29b36f623ba82aaf6724fd3b16718.jpg");

    m_settings->setValue("Save/character_multiple_keepNThenAdd_keep", 2);
    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/character1 character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathIgnoredTags()
{
    m_settings->setValue("ignoredtags", "character1");
    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/character2/1bc29b36f623ba82aaf6724fd3b16718.jpg");

    m_settings->setValue("ignoredtags", "character*");
    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%",
               "artist1/crossover/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathEmptyDirs()
{
    assertPath("%artist%/%test%/%md5%.%ext%",
               "artist1/1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testPathFull()
{
    assertPath("%md5%.%ext%",
               "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
               "tests/directory/",
               true, true);
    assertPath("%md5%.%ext%",
               "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
               "tests/directory",
               true, true);
    assertPath("/%md5%.%ext%",
               "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
               "tests/directory/",
               true, true);
    assertPath("/%md5%.%ext%",
               "tests/directory/1bc29b36f623ba82aaf6724fd3b16718.jpg",
               "tests/directory",
               true, true);
}

void FilenameTest::testPathSimpleJavascript()
{
    assertPath("javascript:md5 + '.' + ext", "1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testPathInvalidJavascript()
{
    assertPath("javascript:'", QStringList());
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
void FilenameTest::testPathOptionTagNamespace()
{
    m_settings->setValue("Save/character_multiple", "keepAll");

    assertPath("%character:includenamespace,unsafe%", "character:character1 character:character2", "", false);
}
void FilenameTest::testPathOptionTagSeparator()
{
    assertPath("%md5% (%count%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (7).jpg");
    assertPath("%md5% (%count:length=3%).%ext%", "1bc29b36f623ba82aaf6724fd3b16718 (007).jpg");
}
void FilenameTest::testPathOptionCount()
{
    assertPath("%md5:maxlength=8%.%ext%", "1bc29b36.jpg");
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
void FilenameTest::testGetReplacesCustom()
{
    QString format = "%md5%.%ext%";

    QMap<QString, QStringList> custom;
    custom.insert("custom1", QStringList() << "tag1 tag2");
    custom.insert("custom2", QStringList() << "tag3 tag4");

    Filename fn(format);
    QList<QMap<QString, QPair<QString, QString>>> replaces = fn.getReplaces(format, *m_img, m_settings, custom);

    QCOMPARE(replaces.first().contains("custom1"), true);
    QCOMPARE(replaces.first().contains("custom2"), true);
    QCOMPARE(replaces.first().contains("custom3"), false);
}

void FilenameTest::testIsValid()
{
    QCOMPARE(Filename("").isValid(), false);
    QCOMPARE(Filename("%md5%").isValid(), false);
    QCOMPARE(Filename("toto").isValid(), false);
    QCOMPARE(Filename("%toto% %md5%.%ext%").isValid(), false);
    QCOMPARE(Filename("%md5%.%ext%").isValid(), true);
    QCOMPARE(Filename("%id%.%ext%").isValid(), false);
    QCOMPARE(Filename("%website%/%id%.%ext%").isValid(), true);
    QCOMPARE(Filename("%artist%/%copyright%/%character%/%md5%.%ext%").isValid(), true);
    QCOMPARE(Filename("javascript:md5 + '.' + ext;").isValid(), true);

    QString out;
    Filename("%toto%.%ext%").isValid(&out);
    QCOMPARE(out.isEmpty(), false);
}

void FilenameTest::testUseShorterCopyright()
{
    m_img->deleteLater();

    m_details["tags_copyright"] = "test test_2";
    m_img = new Image(m_site, m_details);

    m_settings->setValue("Save/copyright_useshorter", true);
    assertPath("%copyright%", "test");
    m_settings->setValue("Save/copyright_multiple", "keepAll");
    m_settings->setValue("Save/copyright_useshorter", false);
    assertPath("%copyright%", "test test_2");

    m_img->deleteLater();

    m_details["tags_copyright"] = "test_2 test";
    m_img = new Image(m_site, m_details);

    m_settings->setValue("Save/copyright_useshorter", true);
    assertPath("%copyright%", "test");
}

void FilenameTest::testConditionalsTag()
{
    m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
    m_settings->setValue("Filenames/0_dir", QDir::homePath());
    m_settings->setValue("Filenames/0_cond", "tag4 tag7");
    m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
    m_settings->setValue("Filenames/1_dir", QDir::homePath());
    m_settings->setValue("Filenames/1_cond", "character1");

    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testConditionalsToken()
{
    m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
    m_settings->setValue("Filenames/0_dir", QDir::homePath());
    m_settings->setValue("Filenames/0_cond", "%model%");
    m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
    m_settings->setValue("Filenames/1_dir", QDir::homePath());
    m_settings->setValue("Filenames/1_cond", "%character%");

    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}
void FilenameTest::testConditionalsCustom()
{
    m_settings->setValue("Save/Customs/custom1", "tag4 tag7");
    m_settings->setValue("Save/Customs/custom2", "tag1");

    m_settings->setValue("Filenames/0_fn", "%md5%.%ext%");
    m_settings->setValue("Filenames/0_dir", QDir::homePath());
    m_settings->setValue("Filenames/0_cond", "%custom1%");
    m_settings->setValue("Filenames/1_fn", "%id% %md5%.%ext%");
    m_settings->setValue("Filenames/1_dir", QDir::homePath());
    m_settings->setValue("Filenames/1_cond", "%custom2%");

    assertPath("%artist%/%copyright%/%character%/%md5%.%ext%", "7331 1bc29b36f623ba82aaf6724fd3b16718.jpg");
}

void FilenameTest::testCustoms()
{
    m_settings->setValue("Save/Customs/custom1", "tag1 character1");
    m_settings->setValue("Save/Customs/custom2", "tag3 tag4");

    assertPath("%custom1%", "tag1 character1");
    assertPath("%custom2%", "tag3");
}

void FilenameTest::testReplaceBlanks()
{
    m_settings->setValue("Save/separator", "+");

    m_settings->setValue("Save/replaceblanks", false);
    assertPath("%all%", "tag1+tag2+tag3+test tag1+test tag2+test tag3+artist1+character1+character2+copyright1+copyright2", "", false);
    assertPath("%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);

    m_settings->setValue("Save/replaceblanks", true);
    assertPath("%all%", "tag1+tag2+tag3+test_tag1+test_tag2+test_tag3+artist1+character1+character2+copyright1+copyright2", "", false);
    assertPath("%allo%", "tag1 tag2 tag3 test_tag1 test_tag2 test_tag3 artist1 character1 character2 copyright1 copyright2", "", false);
}


void FilenameTest::assertPath(QString format, QString expected, QString path, bool shouldFixFilename, bool fullPath)
{
    assertPath(format, QStringList() << expected, path, shouldFixFilename, fullPath);
}

void FilenameTest::assertPath(QString format, QStringList expected, QString path, bool shouldFixFilename, bool fullPath)
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
    QStringList actual = fn.path(*m_img, m_settings, path, 7, true, true, shouldFixFilename, fullPath);
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
