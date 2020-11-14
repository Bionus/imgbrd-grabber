#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include "models/md5-database/md5-database-sqlite.h"
#include "catch.h"
#include "raii-helpers.h"


TEST_CASE("Md5DatabaseSqlite")
{
	FileDeleter settingsDeleter("tests/resources/md5s.sqlite", true);

	QSettings settings("tests/resources/settings.ini", QSettings::IniFormat);
	{
		Md5DatabaseSqlite init("tests/resources/md5s.sqlite", &settings);
		init.add("5a105e8b9d40e1329780d62ea2265d8a", "tests/resources/image_1x1.png");
		init.add("5a105e8b9d40e1329780d62ea2265d8a", "tests/resources/image_200x200.png");
		init.add("ad0234829205b9033196ba818f7a872b", "tests/resources/image_1x1.png");
	}

	SECTION("The constructor should load all the MD5s in memory")
	{
		Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").count() == 2);
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").contains("tests/resources/image_1x1.png"));
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").contains("tests/resources/image_200x200.png"));
		REQUIRE(md5s.exists("ad0234829205b9033196ba818f7a872b") == QStringList("tests/resources/image_1x1.png"));
	}

	SECTION("It can count the number of entries")
	{
		Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
		REQUIRE(md5s.count() == 3);
	}

	SECTION("Can remove an MD5 using remove()")
	{
		Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
		md5s.remove("5a105e8b9d40e1329780d62ea2265d8a");
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a").isEmpty());
	}

	SECTION("Can remove a single MD5 path using remove()")
	{
		Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
		md5s.remove("5a105e8b9d40e1329780d62ea2265d8a", "tests/resources/image_1x1.png");
		REQUIRE(md5s.exists("5a105e8b9d40e1329780d62ea2265d8a") == QStringList("tests/resources/image_200x200.png"));
	}

	SECTION("action()")
	{
		SECTION("when 'keep deleted' is set to false")
		{
			Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
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
			Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
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
			Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
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
			Md5DatabaseSqlite md5s("tests/resources/md5s.sqlite", &settings);
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
