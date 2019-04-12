#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "commands/sql-worker.h"
#include "catch.h"


TEST_CASE("SqlWorkerTest")
{
	QFile::remove("test_sql_worker.db");

	SECTION("Connect works")
	{
		SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

		REQUIRE(worker.connect());
		REQUIRE(worker.connect());
	}

	SECTION("Connect fails")
	{
		SqlWorker worker("NOT_EXISTING_SQL_DRIVER", "1", "2", "3", "4", nullptr);

		REQUIRE(!worker.connect());
		REQUIRE(!worker.connect());
	}

	SECTION("Connect disabled")
	{
		SqlWorker worker("IGNORED_BECAUSE_DISABLED", "", "", "", "", nullptr);

		REQUIRE(worker.connect());
		REQUIRE(worker.connect());
	}


	SECTION("EscapeInteger")
	{
		SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

		REQUIRE(worker.escape(-3) == QString("-3"));
		REQUIRE(worker.escape(0) == QString("0"));
		REQUIRE(worker.escape(21) == QString("21"));
		REQUIRE(worker.escape(12345) == QString("12345"));
	}

	SECTION("Escape string")
	{
		SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

		REQUIRE(worker.escape("test") == QString("'test'"));
		REQUIRE(worker.escape("test'ed") == QString("'test''ed'"));
	}


	SECTION("Exec create and insert")
	{
		SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

		REQUIRE(worker.execute("CREATE TABLE IF NOT EXISTS test_table (some_value INT);"));
		REQUIRE(worker.execute("INSERT INTO test_table (some_value) VALUES (1), (3), (21);"));

		QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("test_sql_worker.db");
		REQUIRE(db.open());

		QSqlQuery query = db.exec("SELECT some_value FROM test_table");
		int idVal = query.record().indexOf("some_value");
		QList<int> values;
		while (query.next()) {
			values.append(query.value(idVal).toInt());
		}

		REQUIRE(values == QList<int>() << 1 << 3 << 21);
	}
}
