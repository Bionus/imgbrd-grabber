#include <QJsonObject>
#include <QTemporaryFile>
#include "models/image-size.h"
#include "catch.h"


TEST_CASE("ImageSize")
{
	SECTION("TemporaryPath")
	{
		QFile file1("tests/resources/tmp/tmp1.txt");
		REQUIRE(file1.open(QFile::Truncate | QFile::WriteOnly | QFile::Text));
		file1.write("test");
		file1.close();

		QFile file2("tests/resources/tmp/tmp2.txt");
		REQUIRE(file2.open(QFile::Truncate | QFile::WriteOnly | QFile::Text));
		file2.write("test");
		file2.close();

		auto *is = new ImageSize();

		REQUIRE(is->setTemporaryPath(file1.fileName()));
		REQUIRE(!is->setTemporaryPath(file1.fileName()));
		REQUIRE(is->fileSize == 4);

		REQUIRE(is->setTemporaryPath(file2.fileName()));
		REQUIRE(!file1.exists());

		delete is;
		REQUIRE(!file2.exists());
	}

	SECTION("SavePath")
	{
		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("test");
		file.close();

		ImageSize is;
		REQUIRE(is.setSavePath(file.fileName()));
		REQUIRE(!is.setSavePath(file.fileName()));
		REQUIRE(is.fileSize == 4);
	}

	SECTION("SaveDefault")
	{
		const QString dest = "tests/resources/tmp/image-size.jpg";

		ImageSize is;
		REQUIRE(is.save(dest) == QString());
		REQUIRE(!QFile::exists(dest));
	}

	SECTION("SaveMove")
	{
		return; // FIXME

		const QString dest = "tests/resources/tmp/image-size.jpg";

		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("test");
		file.close();

		ImageSize is;
		REQUIRE(is.setTemporaryPath(file.fileName()));
		REQUIRE(is.save(dest) == file.fileName());

		REQUIRE(!file.exists());
		REQUIRE(QFile::exists(dest));
		REQUIRE(QFile::remove(dest));
	}

	SECTION("SaveCopy")
	{
		const QString dest = "tests/resources/tmp/image-size.jpg";

		QTemporaryFile file;
		REQUIRE(file.open());
		file.write("test");
		file.close();

		ImageSize is;
		REQUIRE(is.setSavePath(file.fileName()));
		REQUIRE(is.save(dest) == file.fileName());

		REQUIRE(file.exists());
		REQUIRE(QFile::exists(dest));
		REQUIRE(QFile::remove(dest));
	}

	SECTION("Pixmap")
	{
		QPixmap pix("tests/resources/image_1x1.png");

		ImageSize is;
		is.setPixmap(pix);

		REQUIRE(is.pixmap().toImage() == pix.toImage());
	}

	SECTION("PixmapRect")
	{
		QPixmap pix("tests/resources/image_200x200.png");

		ImageSize is;
		is.rect = QRect(0, 0, 20, 40);
		is.setPixmap(pix);

		REQUIRE(is.pixmap().size() == QSize(20, 40));
	}

	SECTION("Serialization")
	{
		ImageSize original;
		original.fileSize = 123456;
		original.size = QSize(800, 600);
		original.rect = QRect(10, 20, 30, 40);

		QJsonObject json;
		original.write(json);

		ImageSize dest;
		dest.read(json);

		REQUIRE(dest.fileSize == original.fileSize);
		REQUIRE(dest.size == original.size);
		REQUIRE(dest.rect == original.rect);
	}
}
