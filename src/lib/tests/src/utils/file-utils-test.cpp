#include <QDir>
#include <QFile>
#include <QString>
#include <QTemporaryFile>
#include "catch.h"
#include "raii-helpers.h"
#include "utils/file-utils.h"


TEST_CASE("File utils")
{
	SECTION("copyRecursively")
	{
		QString from = QDir::toNativeSeparators("tests/resources/recurse/");
		QString to = QDir::toNativeSeparators("tests/resources/tmp/recurse/");

		DirectoryDeleter deleter(to, false, true);

		SECTION("Basic usage")
		{
			REQUIRE(copyRecursively(from, to));
			REQUIRE(QFile::exists(to + "test.txt"));
			REQUIRE(QFile::exists(to + "test/test1.txt"));
			REQUIRE(QFile::exists(to + "test/test2.txt"));
		}

		SECTION("Already exists")
		{
			REQUIRE(copyRecursively(from, to));
			REQUIRE(!copyRecursively(from, to));
		}

		SECTION("Overwrite")
		{
			REQUIRE(copyRecursively(from, to));
			REQUIRE(copyRecursively(from, to, true));
		}
	}

	SECTION("safeCopyFile")
	{
		const QString from = "tests/resources/image_1x1.png";
		const QString file = "tests/resources/tmp/safeCopy.txt";
		FileDeleter deleter(file, true);
		FileDeleter deleterBak(file + ".bak", true);

		SECTION("Basic usage")
		{
			REQUIRE(!QFile::exists(file));
			REQUIRE(safeCopyFile(from, file));
			REQUIRE(QFile::exists(file));
		}

		SECTION("Overwrite without backup")
		{
			REQUIRE(safeCopyFile(from, file, false));
			REQUIRE(QFile::exists(file));

			REQUIRE(safeCopyFile(from, file, false));
			REQUIRE(QFile::exists(file));
			REQUIRE(!QFile::exists(file + ".bak"));
		}

		SECTION("Overwrite with backup")
		{
			REQUIRE(safeCopyFile(from, file, false));
			REQUIRE(QFile::exists(file));

			REQUIRE(safeCopyFile(from, file, true));
			REQUIRE(QFile::exists(file));
			REQUIRE(QFile::exists(file + ".bak"));
		}
	}

	SECTION("safeWriteFile")
	{
		const QString file = "tests/resources/tmp/safe.txt";
		FileDeleter deleter(file, true);
		FileDeleter deleterBak(file + ".bak", true);

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

	SECTION("ensureFileParent")
	{
		SECTION("Parent doesn't exist")
		{
			const QString dir = "tests/resources/tmp/parent/";
			const QString file = dir + "ensure-parent.txt";
			DirectoryDeleter deleter(dir, false, true);

			REQUIRE(!QDir().exists(dir));
			REQUIRE(ensureFileParent(file));
			REQUIRE(QDir().exists(dir));
		}

		SECTION("Parent already exists")
		{
			const QString dir = "tests/resources/tmp/";
			const QString file = dir + "ensure-parent.txt";
			FileDeleter deleter(file, true);

			REQUIRE(QDir().exists(dir));
			REQUIRE(ensureFileParent(file));
			REQUIRE(QDir().exists(dir));
		}
	}

	SECTION("writeFile")
	{
		const QString file = "tests/resources/tmp/write.txt";
		FileDeleter deleter(file, true);

		REQUIRE(!QFile::exists(file));
		REQUIRE(writeFile(file, "test"));
		REQUIRE(QFile::exists(file));
	}
}
