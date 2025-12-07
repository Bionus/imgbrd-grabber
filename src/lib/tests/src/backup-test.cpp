#include <QDir>
#include <QFile>
#include <QSettings>
#include <QTemporaryDir>
#include "catch.h"
#include "backup.h"
#include "models/profile.h"
#include "source-helpers.h"
#include "utils/zip.h"


TEST_CASE("Backup")
{
	const QScopedPointer<Profile> profile(makeProfile());
	QTemporaryDir tmpDir;

	const QString zipFile = "backup-test.zip";
	const QString zipDir = tmpDir.path();

	SECTION("settings.ini")
	{
		// Set a single setting on the profile and back it up
		profile->getSettings()->setValue("foo", "bar");
		REQUIRE(saveBackup(profile.data(), zipFile));
		REQUIRE(QFile::exists(zipFile));

		// Unzipping the backup should contain a settings.ini file
		REQUIRE(unzipFile(zipFile, zipDir));
		const QStringList files = QDir(zipDir).entryList(QDir::Files | QDir::NoDotAndDotDot);
		REQUIRE(files.contains("settings.ini"));

		// Creating a profile file from the backup directory should have the previous setting
		Profile after(zipDir);
		REQUIRE(after.getSettings()->childKeys() == QStringList() << "foo");
		REQUIRE(after.getSettings()->value("foo").toString() == "bar");

		// Create a fresh profile and import the backup in it, the setting key should be available
		const QScopedPointer<Profile> fresh(makeProfile());
		fresh->getSettings()->setValue("key", "value"); // This should be overwritten by the backup
		REQUIRE(fresh->getSettings()->childKeys() == QStringList() << "key");
		REQUIRE(fresh->getSettings()->value("key").toString() == "value");
		REQUIRE(loadBackup(fresh.data(), zipFile));
		REQUIRE(fresh->getSettings()->childKeys() == QStringList() << "foo");
		REQUIRE(fresh->getSettings()->value("foo").toString() == "bar");
	}
}
