#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include "models/md5-database.h"
#include "catch.h"


TEST_CASE("Md5Database")
{
	QFile f("tests/resources/md5s.txt");
	f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);
	f.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png\r\n").toUtf8());
	f.write(QString("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_200x200.png\r\n").toUtf8());
	f.write(QString("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png\r\n").toUtf8());
	f.close();

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);

	SECTION("The constructor should load all the MD5s in memory")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").count() == 2);
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").contains("tests/resources/image_1x1.png"));
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").contains("tests/resources/image_200x200.png"));
		REQUIRE(md5s.exists("ad0234829205b9033196ba818f7a872b") == QStringList("tests/resources/image_1x1.png"));
	}

	SECTION("add() followed by sync() should correctly flush the data to the file")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98") == QStringList("tests/resources/image_1x1.png"));

		md5s.sync();

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 4);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_200x200.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
		REQUIRE(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));
	}

	SECTION("add() should automatically trigger a flush to the disk after a while")
	{
		settings.setValue("md5_flush_interval", 100);

		Md5Database md5s("tests/resources/md5s.txt", &settings);
		QSignalSpy spy(&md5s, SIGNAL(flushed()));
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("8ad8757baa8564dc136c1e07507f4a98") == QStringList("tests/resources/image_1x1.png"));
		REQUIRE(spy.wait());

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 4);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_1x1.png"));
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_200x200.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
		REQUIRE(lines.contains("8ad8757baa8564dc136c1e07507f4a98tests/resources/image_1x1.png"));

		settings.remove("md5_flush_interval");
	}

	SECTION("add() should only trigger one flush")
	{
		settings.setValue("md5_flush_interval", 100);

		Md5Database md5s("tests/resources/md5s.txt", &settings);
		QSignalSpy spy(&md5s, SIGNAL(flushed()));
		md5s.add("8ad8757baa8564dc136c1e07507f4a98", "tests/resources/image_1x1.png");
		md5s.add("8ad8757baa8564dc136c1e07507f4a99", "tests/resources/image_200x200.png");
		REQUIRE(spy.wait());
		REQUIRE(!spy.wait(500));

		REQUIRE(spy.count() == 1);

		settings.remove("md5_flush_interval");
	}

	SECTION("Can remove an MD5 using remove()")
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

	SECTION("Can remove a single MD5 path using remove()")
	{
		Md5Database md5s("tests/resources/md5s.txt", &settings);
		md5s.remove("5a105e8b9d40e1329780d62ea2265d8a", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a") == QStringList("tests/resources/image_200x200.png"));

		md5s.sync();

		QFile f("tests/resources/md5s.txt");
		f.open(QFile::ReadOnly | QFile::Text);
		QStringList lines = QString(f.readAll()).split("\n", QString::SkipEmptyParts);
		f.close();

		REQUIRE(lines.count() == 2);
		REQUIRE(lines.contains("5a105e8b9d40e1329780d62ea2265d8atests/resources/image_200x200.png"));
		REQUIRE(lines.contains("ad0234829205b9033196ba818f7a872btests/resources/image_1x1.png"));
	}


	SECTION("action()")
	{
		SECTION("when 'keep deleted' is set to false")
		{
			Md5Database md5s("tests/resources/md5s.txt", &settings);
			settings.setValue("Save/md5Duplicates", "move");
			settings.setValue("Save/keepDeletedMd5", false);

			QPair<QString, QString> action;

			action = md5s.action("new", "");
			REQUIRE(action.first == QString("save")); // The MD5 is not found so the default is "save"
			REQUIRE(action.second == QString(""));

			md5s.add("new", "tests/resources/image_1x1.png");

			action = md5s.action("new", "");
			REQUIRE(action.first == QString("move"));
			REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

			md5s.remove("new");

			action = md5s.action("new", "");
			REQUIRE(action.first == QString("save"));
			REQUIRE(action.second == QString(""));

			// Restore state
			settings.setValue("Save/md5Duplicates", "save");
		}

		SECTION("when 'keep deleted' is set to true")
		{
			Md5Database md5s("tests/resources/md5s.txt", &settings);
			settings.setValue("Save/md5Duplicates", "move");
			settings.setValue("Save/keepDeletedMd5", true);

			QPair<QString, QString> action;

			action = md5s.action("new", "");
			REQUIRE(action.first == QString("save"));
			REQUIRE(action.second == QString(""));

			md5s.add("new", "NON_EXISTING_FILE");

			action = md5s.action("new", "");
			REQUIRE(action.first == QString("save")); // You can't "move" a non-existing file
			REQUIRE(action.second == QString("NON_EXISTING_FILE"));

			// Restore state
			md5s.remove("new");
			settings.setValue("Save/md5Duplicates", "save");
			settings.setValue("Save/keepDeletedMd5", false);
		}

		SECTION("for files in the same directory")
		{
			Md5Database md5s("tests/resources/md5s.txt", &settings);
			md5s.add("new", "tests/resources/image_1x1.png");

			settings.setValue("Save/md5Duplicates", "save");
			settings.setValue("Save/md5DuplicatesSameDir", "move");
			settings.setValue("Save/keepDeletedMd5", false);

			QPair<QString, QString> action;

			action = md5s.action("new", "tests/resources/different/different.png");
			REQUIRE(action.first == QString("save"));
			REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

			action = md5s.action("new", "tests/resources/same.png");
			REQUIRE(action.first == QString("move"));
			REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

			// Restore state
			md5s.remove("new");
			settings.setValue("Save/md5Duplicates", "save");
			settings.setValue("Save/md5DuplicatesSameDir", "save");
		}

		SECTION("prioritize actions for files in the same directory")
		{
			Md5Database md5s("tests/resources/md5s.txt", &settings);
			md5s.add("new", "same_dir/image.png"); // Doesn't exist
			md5s.add("new", "tests/resources/image_1x1.png"); // Exists

			settings.setValue("Save/md5Duplicates", "copy");
			settings.setValue("Save/md5DuplicatesSameDir", "ignore");
			settings.setValue("Save/keepDeletedMd5", true);

			QPair<QString, QString> action;

			action = md5s.action("new", "tests/resources/different/different.png");
			REQUIRE(action.first == QString("copy")); // None exists in this directory, but there is elsewhere
			REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

			action = md5s.action("new", "tests/resources/same.png");
			REQUIRE(action.first == QString("ignore")); // One exists in the same directory
			REQUIRE(action.second == QString("tests/resources/image_1x1.png"));

			action = md5s.action("new", "same_dir/same.png");
			REQUIRE(action.first == QString("ignore")); // One used to exist in the same directory
			REQUIRE(action.second == QString("same_dir/image.png"));

			// Restore state
			md5s.remove("new");
			settings.setValue("Save/md5Duplicates", "save");
			settings.setValue("Save/md5DuplicatesSameDir", "save");
		}
	}
}
