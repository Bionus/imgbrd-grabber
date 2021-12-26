#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryFile>
#include "catch.h"
#include "utils/file-utils.h"


TEST_CASE("File utils")
{
	SECTION("copyRecursively")
	{
		QString from = QDir::toNativeSeparators("tests/resources/recurse/");
		QString to = QDir::toNativeSeparators("tests/resources/tmp/recurse/");

		QDir(to).removeRecursively();

		REQUIRE(copyRecursively(from, to));
		REQUIRE(QFile::exists(to + "test.txt"));
		REQUIRE(QFile::exists(to + "test/test1.txt"));
		REQUIRE(QFile::exists(to + "test/test2.txt"));
	}

	SECTION("safeWriteFile")
	{
		const QString file = "tests/resources/tmp/safe.txt";
		QFile::remove(file);
		QFile::remove(file + ".bak");

		SECTION("Basic usage")
		{
			REQUIRE(!QFile::exists(file));
			REQUIRE(safeWriteFile(file, "test"));
			REQUIRE(QFile::exists(file));
		}

		SECTION("Overwrite without backup")
		{
			REQUIRE(safeWriteFile(file, "test", false));
			REQUIRE(QFile::exists(file));

			REQUIRE(safeWriteFile(file, "test", false));
			REQUIRE(QFile::exists(file));
			REQUIRE(!QFile::exists(file + ".bak"));
		}

		SECTION("Overwrite with backup")
		{
			REQUIRE(safeWriteFile(file, "test", false));
			REQUIRE(QFile::exists(file));

			REQUIRE(safeWriteFile(file, "test", true));
			REQUIRE(QFile::exists(file));
			REQUIRE(QFile::exists(file + ".bak"));
		}
	}
}
