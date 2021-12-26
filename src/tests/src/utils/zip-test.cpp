#include <QFile>
#include "catch.h"
#include "utils/zip.h"


TEST_CASE("Zip")
{
	SECTION("Create zip and unzip")
	{
		const QString filePath = "tests/resources/test.zip";
		const QHash<QString, QString> files = {
			{ "tests/resources/image_1x1.png", "1x1.png" },
			{ "tests/resources/image_200x200.png", "200x200.png" },
		};

		// Clean-up if necessary
		if (QFile::exists(filePath)) {
			REQUIRE(QFile::remove(filePath));
		}

		// Create ZIP
		REQUIRE(createZip(filePath, files));
		REQUIRE(QFile::exists(filePath));

		// Unzip
		REQUIRE(unzipFile(filePath, "tests/resources/unzip-dir"));
		REQUIRE(QFile::exists("tests/resources/unzip-dir/1x1.png"));
		REQUIRE(QFile::exists("tests/resources/unzip-dir/200x200.png"));

		// Clean-up
		REQUIRE(QFile::remove(filePath));
	}
}
