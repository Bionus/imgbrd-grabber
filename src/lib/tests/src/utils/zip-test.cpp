#include <QFile>
#include "catch.h"
#include "raii-helpers.h"
#include "utils/zip.h"


TEST_CASE("Zip")
{
	FileDeleter removeFile("tests/resources/test.zip", true);
	DirectoryDeleter removeDir("tests/resources/unzip-dir");

	SECTION("Create zip and unzip")
	{
		const QString filePath = "tests/resources/test.zip";
		const QHash<QString, QString> files = {
			{ "tests/resources/image_1x1.png", "1x1.png" },
			{ "tests/resources/image_200x200.png", "200x200.png" },
		};

		// Create ZIP
		REQUIRE(createZip(filePath, files));
		REQUIRE(QFile::exists(filePath));

		// Unzip
		REQUIRE(unzipFile(filePath, "tests/resources/unzip-dir"));
		REQUIRE(QFile::exists("tests/resources/unzip-dir/1x1.png"));
		REQUIRE(QFile::exists("tests/resources/unzip-dir/200x200.png"));
	}
}
