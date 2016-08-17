#include <QtTest>
#include "filename-test.h"


void FilenameTest::init()
{
    QMap<QString,QString> details;
    details["md5"] = "1bc29b36f623ba82aaf6724fd3b16718";
    details["ext"] = "jpg";

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


void FilenameTest::assertPath(QString format, QString expected, QString path)
{
    assertPath(format, QStringList() << expected, path);
}

void FilenameTest::assertPath(QString format, QStringList expected, QString path)
{
    Filename fn(format);
    QCOMPARE(fn.path(*m_img, path), expected);
}

static FilenameTest instance;
