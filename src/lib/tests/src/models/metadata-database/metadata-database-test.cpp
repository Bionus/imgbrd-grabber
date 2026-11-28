#include <QDateTime>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "models/metadata-database/metadata-database.h"
#include "catch.h"
#include "raii-helpers.h"


static MetadataDatabaseEntry makeEntry(const QString &md5, const QString &website, const QString &id, const QStringList &artists, const QStringList &generals)
{
	MetadataDatabaseEntry entry;
	entry.md5 = md5;
	entry.website = website;
	entry.id = id;
	entry.rating = "general";
	if (!artists.isEmpty()) {
		entry.tags.insert("artist", artists);
	}
	if (!generals.isEmpty()) {
		entry.tags.insert("general", generals);
	}
	entry.allTags = artists + generals;
	return entry;
}


TEST_CASE("MetadataDatabase")
{
	FileDeleter fileDeleter("tests/resources/metadata-test.sqlite", true);

	const QString md5 = "6533cf0248229ac131607dcef46a74dd";
	const MetadataDatabaseEntry danbooru = makeEntry(md5, "danbooru.donmai.us", "4204575", { "artist 1" }, { "tag 1", "tag_2", "1girl" });
	const MetadataDatabaseEntry gelbooru = makeEntry(md5, "gelbooru.com", "5680029", { "artist 1" }, { "tag 3" });
	const MetadataDatabaseEntry other = makeEntry("ad0234829205b9033196ba818f7a872b", "gelbooru.com", "123", {}, { "tag 4" });

	{
		MetadataDatabase init("tests/resources/metadata-test.sqlite");
		REQUIRE(init.save(danbooru));
		REQUIRE(init.save(gelbooru));
		REQUIRE(init.save(other));
	}

	SECTION("It can count the number of entries")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		REQUIRE(database.count() == 3);
	}

	SECTION("It returns all the sources of a file in insertion order")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		const QList<MetadataDatabaseEntry> entries = database.get(md5);

		REQUIRE(entries.count() == 2);
		REQUIRE(entries[0] == danbooru);
		REQUIRE(entries[1] == gelbooru);
		REQUIRE(entries[0].tags.value("general") == QStringList({ "tag 1", "tag_2", "1girl" }));
		REQUIRE(entries[0].updatedAt.isValid());
	}

	SECTION("It can check if a post was already stored")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		REQUIRE(database.exists("danbooru.donmai.us", "4204575"));
		REQUIRE(database.exists("gelbooru.com", "123"));
		REQUIRE(!database.exists("danbooru.donmai.us", "5680029"));
		REQUIRE(!database.exists("tbib.org", "4204575"));
		REQUIRE(!database.exists("", ""));
	}

	SECTION("It returns nothing for unknown files")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		REQUIRE(database.get("00000000000000000000000000000000").isEmpty());
		REQUIRE(database.get("").isEmpty());
	}

	SECTION("Saving an existing source updates it in place")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");

		MetadataDatabaseEntry updated = danbooru;
		updated.rating = "sensitive";
		updated.tags.insert("general", { "new tag" });
		updated.allTags = QStringList { "artist 1", "new tag" };
		REQUIRE(database.save(updated));

		const QList<MetadataDatabaseEntry> entries = database.get(md5);
		REQUIRE(database.count() == 3);
		REQUIRE(entries.count() == 2);
		REQUIRE(entries[0] == updated);
		REQUIRE(entries[1] == gelbooru);
	}

	SECTION("It stores the post date")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		const QDateTime date(QDate(2024, 1, 2), QTime(3, 4, 5, 678), Qt::UTC);

		MetadataDatabaseEntry dated = danbooru;
		dated.date = date;
		REQUIRE(database.save(dated));
		REQUIRE(database.get(md5)[0].date == date);
		REQUIRE(!database.get(md5)[1].date.isValid());

		// Saving the same post without a date keeps the known one
		REQUIRE(database.save(danbooru));
		REQUIRE(database.get(md5)[0].date == date);
	}

	SECTION("It adds the missing columns to databases created by older versions")
	{
		const QString oldPath = "tests/resources/metadata-old-test.sqlite";
		FileDeleter oldDeleter(oldPath, true);
		{
			QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "metadata-old-test");
			db.setDatabaseName(oldPath);
			REQUIRE(db.open());
			QSqlQuery query(db);
			REQUIRE(query.exec("CREATE TABLE metadata (id INTEGER PRIMARY KEY AUTOINCREMENT, md5 CHAR(32) NOT NULL, website TEXT NOT NULL, website_name TEXT, item TEXT NOT NULL, rating TEXT, \"artist\" TEXT, \"copyright\" TEXT, \"character\" TEXT, \"model\" TEXT, \"species\" TEXT, \"meta\" TEXT, \"general\" TEXT, tags TEXT, updated_at TEXT, UNIQUE(md5, website, item))"));
			REQUIRE(query.exec("INSERT INTO metadata (md5, website, item, tags) VALUES ('" + md5 + "', 'tbib.org', '8945870', '[\"tag 5\"]')"));
			query.finish();
			db.close();
		}
		QSqlDatabase::removeDatabase("metadata-old-test");

		MetadataDatabase database(oldPath);
		QList<MetadataDatabaseEntry> entries = database.get(md5);
		REQUIRE(entries.count() == 1);
		REQUIRE(entries[0].allTags == QStringList("tag 5"));
		REQUIRE(!entries[0].date.isValid());

		const QDateTime date(QDate(2020, 5, 6), QTime(7, 8, 9), Qt::UTC);
		entries[0].date = date;
		entries[0].tags.insert("lore", { "tag 6" });
		REQUIRE(database.save(entries[0]));
		REQUIRE(database.get(md5)[0].date == date);
		REQUIRE(database.get(md5)[0].tags.value("lore") == QStringList("tag 6"));
	}

	SECTION("It refuses entries without an identifier")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		REQUIRE(!database.save(makeEntry("", "danbooru.donmai.us", "1", {}, {})));
		REQUIRE(!database.save(makeEntry(md5, "", "1", {}, {})));
		REQUIRE(!database.save(makeEntry(md5, "danbooru.donmai.us", "", {}, {})));
		REQUIRE(database.count() == 3);
	}

	SECTION("Can remove a file using remove()")
	{
		MetadataDatabase database("tests/resources/metadata-test.sqlite");
		REQUIRE(database.remove(md5));
		REQUIRE(database.get(md5).isEmpty());
		REQUIRE(database.count() == 1);
	}
}
