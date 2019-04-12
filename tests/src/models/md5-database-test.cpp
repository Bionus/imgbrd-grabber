#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include "models/md5-database.h"
#include "catch.h"


TEST_CASE("Md5Database")
{
	QFile f("tests/resources/md5s.txt");
	f.open(QFile::WriteOnly | QFile::Text);
	f.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png\r\n").toUtf8());
	f.write(QString("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png\r\n").toUtf8());
	f.close();

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);

	SECTION("Load")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a") == QString("tests/resources/image_1x1.png"));
		REQUIRE(md5s.exists("ad0234829205b9033196ba818f7a872b") == QString("tests/resources/image_1x1.png"));
	}

	SECTION("AddSync")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98") == QString("tests/resources/image_1x1.png"));

		md5s.sync();

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 3);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
		REQUIRE(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));
	}

	SECTION("AddFlush")
	{
		settings.setValue("md5_flush_interval", 100);

		Md5Database md5s("tests/resources/md5s.txt", &settings);
		QSignalSpy spy(&md5s, SIGNAL(flushed()));
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98") == QString("tests/resources/image_1x1.png"));
		REQUIRE(spy.wait());

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 3);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
		REQUIRE(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));

		settings.remove("md5_flush_interval");
	}

	SECTION("AddFlushOnlyOnce")
	{
		settings.setValue("md5_flush_interval", 100);

		Md5Database md5s("tests/resources/md5s.txt", &settings);
		QSignalSpy spy(&md5s, SIGNAL(flushed()));
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		md5s.add("8ad8757baa8564dc136c1e07507f4a99", "tests/resources/image_1x1.png");
		REQUIRE(spy.wait());
		REQUIRE(!spy.wait(500));

		REQUIRE(spy.count() == 1);

		settings.remove("md5_flush_interval");
	}

	SECTION("Update")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		md5s.set("5a105e8b9d40e1329780d62ea2265d8a", "newpath.png");
		md5s.sync();

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 2);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8anewpath.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	}

	SECTION("Remove")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		md5s.remove("5a105e8b9d40e1329780d62ea2265d8a");
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").isEmpty());

		md5s.sync();

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 1);
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	}


	SECTION("ActionDontKeepDeleted")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		settings.setValue("Save/md5Duplicates", "move");
		settings.setValue("Save/keepDeletedMd5", false);

		QPair<QString, QString> action;

		action = md5s.action("new");
		REQUIRE(action.first == QString("move"));
		REQUIRE(action.second == QString(""));

		md5s.add("new", "tests/resources/image_1x1.png");

		action = md5s.action("new");
		REQUIRE(action.first == QString("move"));
		REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

		md5s.remove("new");

		action = md5s.action("new");
		REQUIRE(action.first == QString("move"));
		REQUIRE(action.second == QString(""));

		// Restore state
		settings.setValue("Save/md5Duplicates", "save");
	}

	SECTION("ActionKeepDeleted")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		settings.setValue("Save/md5Duplicates", "move");
		settings.setValue("Save/keepDeletedMd5", true);

		QPair<QString, QString> action;

		action = md5s.action("new");
		REQUIRE(action.first == QString("move"));
		REQUIRE(action.second == QString(""));

		md5s.add("new", "NON_EXISTING_FILE");

		action = md5s.action("new");
		REQUIRE(action.first == QString("ignore"));
		REQUIRE(action.second == QString("NON_EXISTING_FILE"));

		// Restore state
		md5s.remove("new");
		settings.setValue("Save/md5Duplicates", "save");
		settings.setValue("Save/keepDeletedMd5", false);
	}
}
