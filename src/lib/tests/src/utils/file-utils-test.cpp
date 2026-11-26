#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QString>
#include <QTemporaryFile>
#include "catch.h"
#include "logger.h"
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


TEST_CASE("Directory creation")
{
	QTemporaryDir temporaryDir;
	REQUIRE(temporaryDir.isValid());
	QSignalSpy logs(&Logger::getInstance(), &Logger::newLog);

	const auto requireFailure = [&logs](const QString &path) {
		logs.clear();
		REQUIRE_FALSE(ensureDirectoryExists(path));
		REQUIRE(logs.count() == 1);
		const QString message = logs.first().first().toString();
		INFO(message.toStdString());
		REQUIRE(message.contains(path));
		REQUIRE(message.contains("[Error]"));
		// A path alone does not explain why creation failed.
		REQUIRE(message.contains('('));
		REQUIRE(message.indexOf(')') > message.indexOf('(') + 1);
	};

	SECTION("Creates nested directories with portable names")
	{
		const QString suffix = GENERATE(
			QString("one/two/three"),
			QString("folder with spaces/child"),
			QString::fromUtf8("caf\xc3\xa9/\xe6\x97\xa5\xe6\x9c\xac/\xf0\x9f\x98\x80"),
			QString(".hidden/child"),
			QString("trailing/slash/"),
			QString("repeated//separators///child"),
			QString("dot/./child"));
		const QString path = temporaryDir.filePath(suffix);
		CAPTURE(path);
		REQUIRE(ensureDirectoryExists(path));
		REQUIRE(QFileInfo(path).isDir());
		REQUIRE(ensureDirectoryExists(path));
		REQUIRE(logs.isEmpty());
	}

	SECTION("Existing directory preserves its contents")
	{
		const QString file = temporaryDir.filePath("keep.txt");
		REQUIRE(writeFile(file, "keep"));
		REQUIRE(ensureDirectoryExists(temporaryDir.path()));
		QFile contents(file);
		REQUIRE(contents.open(QIODevice::ReadOnly));
		REQUIRE(contents.readAll() == "keep");
		REQUIRE(logs.isEmpty());
	}

	SECTION("Current directory and filesystem root already exist")
	{
		REQUIRE(ensureDirectoryExists("."));
		REQUIRE(ensureDirectoryExists(QDir::rootPath()));
		REQUIRE(logs.isEmpty());
	}

	SECTION("Relative paths use the current directory")
	{
		const QString previous = QDir::currentPath();
		const auto restore = qScopeGuard([previous]() { QDir::setCurrent(previous); });
		REQUIRE(QDir::setCurrent(temporaryDir.path()));
		REQUIRE(ensureDirectoryExists("relative/child"));
		REQUIRE(QFileInfo(temporaryDir.filePath("relative/child")).isDir());
		REQUIRE(logs.isEmpty());
	}

	SECTION("Parent components reach the intended directory")
	{
		REQUIRE(QDir().mkpath(temporaryDir.filePath("existing")));
		REQUIRE(ensureDirectoryExists(temporaryDir.filePath("existing/../sibling/child")));
		REQUIRE(QFileInfo(temporaryDir.filePath("sibling/child")).isDir());
		REQUIRE(logs.isEmpty());
	}

	SECTION("Empty path is rejected")
	{
		REQUIRE_FALSE(ensureDirectoryExists(QString()));
		REQUIRE(logs.count() == 1);
		REQUIRE(logs.first().first().toString().contains("empty path"));
	}

	SECTION("Embedded null does not create a truncated path")
	{
		const QString prefix = temporaryDir.filePath("truncated");
		const QString path = prefix + QChar::Null + "/child";
		REQUIRE_FALSE(ensureDirectoryExists(path));
		REQUIRE_FALSE(QFileInfo::exists(prefix));
		REQUIRE_FALSE(logs.isEmpty());
	}

	SECTION("File at destination or ancestor is preserved and explained")
	{
		const QString suffix = GENERATE(QString(), QString("/child"), QString("/one/two"));
		const QString file = temporaryDir.filePath("blocker");
		REQUIRE(writeFile(file, "keep"));
		requireFailure(file + suffix);
		QFile contents(file);
		REQUIRE(contents.open(QIODevice::ReadOnly));
		REQUIRE(contents.readAll() == "keep");
	}

	SECTION("Overlong component is rejected and explained")
	{
		requireFailure(temporaryDir.filePath(QString(300, 'x') + "/child"));
	}

	SECTION("Existing Qt search path resolves without creating a literal prefix")
	{
		QDir::setSearchPaths("directorytest", { temporaryDir.path() });
		const auto restore = qScopeGuard([]() { QDir::setSearchPaths("directorytest", {}); });
		REQUIRE(QDir().mkpath(temporaryDir.filePath("existing")));
		REQUIRE(ensureDirectoryExists("directorytest:existing"));
		REQUIRE(logs.isEmpty());
	}

	#ifdef Q_OS_UNIX
		SECTION("Directory symlink supports existing and new children")
		{
			const QString target = temporaryDir.filePath("target");
			const QString link = temporaryDir.filePath("link");
			REQUIRE(QDir().mkpath(target));
			REQUIRE(QFile::link(target, link));
			REQUIRE(ensureDirectoryExists(link));
			REQUIRE(ensureDirectoryExists(link + "/child"));
			REQUIRE(QFileInfo(target + "/child").isDir());
			REQUIRE(QFileInfo(link).isSymLink());
			REQUIRE(logs.isEmpty());
		}

		SECTION("Dangling symlink is preserved when creation fails")
		{
			const QString target = temporaryDir.filePath("missing");
			const QString link = temporaryDir.filePath("link");
			REQUIRE(QFile::link(target, link));
			REQUIRE_FALSE(ensureDirectoryExists(link + "/child"));
			REQUIRE(QFileInfo(link).isSymLink());
			REQUIRE_FALSE(QFileInfo::exists(target));
			REQUIRE_FALSE(logs.isEmpty());
		}

		SECTION("Symlink loop is rejected and explained")
		{
			const QString link = temporaryDir.filePath("loop");
			REQUIRE(QFile::link(link, link));
			requireFailure(link + "/child");
			REQUIRE(QFileInfo(link).isSymLink());
		}

		SECTION("File symlink cannot become a directory")
		{
			const QString target = temporaryDir.filePath("file");
			const QString link = temporaryDir.filePath("link");
			REQUIRE(writeFile(target, "keep"));
			REQUIRE(QFile::link(target, link));
			requireFailure(link + "/child");
			REQUIRE(QFileInfo(link).isSymLink());
			REQUIRE(QFileInfo(target).isFile());
		}

		SECTION("Unwritable parent is rejected and explained")
		{
			const QString parent = temporaryDir.filePath("readonly");
			REQUIRE(QDir().mkpath(parent));
			const auto permissions = QFileInfo(parent).permissions();
			const auto restore = qScopeGuard([parent, permissions]() { QFile::setPermissions(parent, permissions); });
			REQUIRE(QFile::setPermissions(parent, QFile::ReadOwner | QFile::ExeOwner));
			if (QFileInfo(parent).isWritable()) {
				SKIP("Permissions are not enforced for this user (running as root?)");
			}
			requireFailure(parent + "/child");
			REQUIRE_FALSE(QFileInfo::exists(parent + "/child"));
		}

		SECTION("Unix permits Windows reserved names and punctuation")
		{
			const QString suffix = GENERATE(QString("CON/child"), QString("name:with?punctuation/child"), QString("trailing./child"));
			const QString path = temporaryDir.filePath(suffix);
			REQUIRE(ensureDirectoryExists(path));
			REQUIRE(QFileInfo(path).isDir());
			REQUIRE(logs.isEmpty());
		}
	#endif

	#ifdef Q_OS_WIN
		SECTION("Native separators create the intended directories")
		{
			const QString path = temporaryDir.filePath("native/child");
			REQUIRE(ensureDirectoryExists(QDir::toNativeSeparators(path)));
			REQUIRE(QFileInfo(path).isDir());
			REQUIRE(logs.isEmpty());
		}

		SECTION("Invalid Windows characters are rejected and explained")
		{
			const QString suffix = GENERATE(QString("bad<name/child"), QString("bad|name/child"), QString("bad*name/child"), QString("bad?name/child"));
			requireFailure(temporaryDir.filePath(suffix));
		}

		SECTION("Long absolute paths can be created")
		{
			QString path = temporaryDir.path();
			for (int i = 0; i < 20; ++i) {
				path += "/long-path-component";
			}
			REQUIRE(path.size() > 260);
			REQUIRE(ensureDirectoryExists(path));
			REQUIRE(QFileInfo(path).isDir());
			REQUIRE(logs.isEmpty());
		}
	#endif
}


#ifdef Q_OS_WIN
	TEST_CASE("Directory creation matches Qt Windows filename handling")
	{
		QTemporaryDir temporaryDir;
		REQUIRE(temporaryDir.isValid());
		// Reserved device names are covered by the next test case
		const QString suffix = GENERATE(
			QString("trailing./child"), QString("trailing /child"), QString(" leading/child"),
			QString("bad:name/child"), QString("bad\"name/child"));
		CAPTURE(suffix);
		const QString reference = temporaryDir.filePath("qt/" + suffix);
		const QString actual = temporaryDir.filePath("helper/" + suffix);
		const bool expected = QDir().mkpath(reference);
		REQUIRE(ensureDirectoryExists(actual) == expected);
		REQUIRE(QFileInfo(actual).isDir() == QFileInfo(reference).isDir());
	}
#endif


#ifdef Q_OS_WIN
	TEST_CASE("Directory creation matches Qt Windows reserved names")
	{
		QTemporaryDir temporaryDir;
		REQUIRE(temporaryDir.isValid());
		const QString name = GENERATE(
			QString("CON"), QString("PRN"), QString("AUX"), QString("NUL"),
			QString("COM1"), QString("COM2"), QString("COM3"), QString("COM4"), QString("COM5"),
			QString("COM6"), QString("COM7"), QString("COM8"), QString("COM9"),
			QString("LPT1"), QString("LPT2"), QString("LPT3"), QString("LPT4"), QString("LPT5"),
			QString("LPT6"), QString("LPT7"), QString("LPT8"), QString("LPT9"),
			QString("con"), QString("nUl.txt"), QString("COM1.log"), QString("LPT9.tar.gz"),
			QString::fromUtf8("COM\xc2\xb9"), QString::fromUtf8("LPT\xc2\xb2"));
		const QString child = GENERATE(QString(), QString("/child"));
		CAPTURE(name, child);
		const QString reference = temporaryDir.filePath("qt/" + name + child);
		const QString actual = temporaryDir.filePath("helper/" + name + child);
		const bool expected = QDir().mkpath(reference);
		QSignalSpy logs(&Logger::getInstance(), &Logger::newLog);

		REQUIRE(ensureDirectoryExists(actual) == expected);
		REQUIRE(QFileInfo(actual).isDir() == expected);
		if (expected) {
			REQUIRE(logs.isEmpty());
			REQUIRE(ensureDirectoryExists(actual));
		} else {
			REQUIRE(logs.count() == 1);
			const QString message = logs.first().first().toString();
			INFO(message.toStdString());
			REQUIRE(message.contains(actual));
			REQUIRE(message.contains("[Error]"));
			REQUIRE(message.contains('('));
		}
	}
#endif
