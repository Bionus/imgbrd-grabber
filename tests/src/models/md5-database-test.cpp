#include "md5-database-test.h"
#include <QFile>
#include <QSignalSpy>
#include <QtTest>
#include "models/md5-database.h"


void Md5DatabaseTest::init()
{
	QFile f("tests/resources/md5s.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png\r\n").toUtf8());
	f.write(QString("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png\r\n").toUtf8());
	f.close();

	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);
}

void Md5DatabaseTest::cleanup()
{
	QFile::remove("tests/resources/md5s.txt");

	m_settings->deleteLater();
}


void Md5DatabaseTest::testLoad()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	QCOMPARE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a"), QString("tests/resources/image_1x1.png"));
	QCOMPARE(md5s.exists("ad0234829205b9033196ba818f7a872b"), QString("tests/resources/image_1x1.png"));
}

void Md5DatabaseTest::testAddSync()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
	QCOMPARE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98"), QString("tests/resources/image_1x1.png"));

	md5s.sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 3);
	QVERIFY(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	QVERIFY(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));
}

void Md5DatabaseTest::testAddFlush()
{
	m_settings->setValue("md5_flush_interval", 100);

	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	QSignalSpy spy(&md5s, SIGNAL(flushed()));
	md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
	QCOMPARE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98"), QString("tests/resources/image_1x1.png"));
	QVERIFY(spy.wait());

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 3);
	QVERIFY(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	QVERIFY(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));

	m_settings->remove("md5_flush_interval");
}

void Md5DatabaseTest::testAddFlushOnlyOnce()
{
	m_settings->setValue("md5_flush_interval", 100);

	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	QSignalSpy spy(&md5s, SIGNAL(flushed()));
	md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
	md5s.add("8ad8757baa8564dc136c1e07507f4a99", "tests/resources/image_1x1.png");
	QVERIFY(spy.wait());
	QVERIFY(!spy.wait(500));

	QCOMPARE(spy.count(), 1);

	m_settings->remove("md5_flush_interval");
}

void Md5DatabaseTest::testUpdate()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	md5s.set("5a105e8b9d40e1329780d62ea2265d8a", "newpath.png");
	md5s.sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 2);
	QVERIFY(lines.contains("5a105e8b9d40e1329780d62ea2265d8anewpath.png"));
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
}

void Md5DatabaseTest::testRemove()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	md5s.remove("5a105e8b9d40e1329780d62ea2265d8a");
	QVERIFY(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").isEmpty());

	md5s.sync();

	QFile f("tests/resources/md5s.txt");
	f.open(QFile::ReadOnly | QFile::Text);
	QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
	f.close();

	QCOMPARE(lines.count(), 1);
	QVERIFY(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
}


void Md5DatabaseTest::testActionDontKeepDeleted()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	m_settings->setValue("Save/md5Duplicates", "move");
	m_settings->setValue("Save/keepDeletedMd5", false);

	QPair<QString, QString> action;

	action = md5s.action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	md5s.add("new", "tests/resources/image_1x1.png");

	action = md5s.action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString("tests/resources/image_1x1.png"));

	md5s.remove("new");

	action = md5s.action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	// Restore state
	m_settings->setValue("Save/md5Duplicates", "save");
}

void Md5DatabaseTest::testActionKeepDeleted()
{
	Md5Database md5s("tests/resources/md5s.txt", m_settings);
	m_settings->setValue("Save/md5Duplicates", "move");
	m_settings->setValue("Save/keepDeletedMd5", true);

	QPair<QString, QString> action;

	action = md5s.action("new");
	QCOMPARE(action.first, QString("move"));
	QCOMPARE(action.second, QString(""));

	md5s.add("new", "NON_EXISTING_FILE");

	action = md5s.action("new");
	QCOMPARE(action.first, QString("ignore"));
	QCOMPARE(action.second, QString("NON_EXISTING_FILE"));

	// Restore state
	md5s.remove("new");
	m_settings->setValue("Save/md5Duplicates", "save");
	m_settings->setValue("Save/keepDeletedMd5", false);
}



QTEST_MAIN(Md5DatabaseTest)
