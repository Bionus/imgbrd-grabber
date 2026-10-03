#include "downloader/extension-rotator.h"
#include "catch.h"


TEST_CASE("ExtensionRotatorTest")
{
	SECTION("Basic")
	{
		ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Loop")
	{
		ExtensionRotator rotator("png", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Not found")
	{
		ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Keep empty")
	{
		ExtensionRotator rotator("jpg", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty first")
	{
		ExtensionRotator rotator("", QStringList() << "jpg" << "png" << "gif");

		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));
		REQUIRE(rotator.next() == QString("gif"));
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty list")
	{
		ExtensionRotator rotator("jpg", QStringList());

		REQUIRE(rotator.next() == QString());
	}

	SECTION("Empty both")
	{
		ExtensionRotator rotator("", QStringList());

		REQUIRE(rotator.next() == QString());
	}

	SECTION("Copy constructor")
	{
		ExtensionRotator rotator("mp4", QStringList() << "jpg" << "png" << "gif");
		REQUIRE(rotator.next() == QString("jpg"));
		REQUIRE(rotator.next() == QString("png"));

		ExtensionRotator cpy(rotator);
		REQUIRE(cpy.next() == QString("gif"));
		REQUIRE(cpy.next() == QString());
	}

	SECTION("Video never rotates into a still image")
	{
		// The animated default list, as built by Image::init().
		const QStringList animated { "mp4", "webm", "gif", "jpg", "png", "jpeg", "swf" };

		REQUIRE(ExtensionRotator::withoutStillImages(animated)
			== QStringList() << "mp4" << "webm");

		// Without the filter a webm video 404 offers mp4 -> gif -> jpg -> png, and
		// the jpg retry succeeds with the thumbnail, silently saving a still image
		// in place of the video (#3726).
		ExtensionRotator rotator("webm", ExtensionRotator::withoutStillImages(animated));
		QStringList offered;
		for (int i = 0; i < 5; ++i) {
			const QString next = rotator.next();
			if (next.isEmpty()) {
				break;
			}
			offered << next;
		}
		REQUIRE(offered == QStringList() << "mp4");
		REQUIRE(rotator.next() == QString());
	}

	SECTION("Keeps other video formats")
	{
		// Only still images are dropped, so formats the user configured survive.
		REQUIRE(ExtensionRotator::withoutStillImages(QStringList() << "mp4" << "mkv" << "mov" << "webm" << "jpg")
			== QStringList() << "mp4" << "mkv" << "mov" << "webm");
	}

	SECTION("Still images are all removed")
	{
		REQUIRE(ExtensionRotator::withoutStillImages(QStringList() << "jpg" << "jpeg" << "jpe" << "png" << "apng" << "gif" << "swf").isEmpty());
	}

	SECTION("Empty list stays empty")
	{
		REQUIRE(ExtensionRotator::withoutStillImages(QStringList()).isEmpty());
	}
}
