#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include "sql-worker-test.h"
#include "commands/sql-worker.h"


void SqlWorkerTest::cleanup()
{
	QFile::remove("test_sql_worker.db");
}


void SqlWorkerTest::testConnectOk()
{
	SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

	QVERIFY(worker.connect());
	QVERIFY(worker.connect());
}

void SqlWorkerTest::testConnectError()
{
	SqlWorker worker("NOT_EXISTING_SQL_DRIVER", "1", "2", "3", "4", nullptr);

	QVERIFY(!worker.connect());
	QVERIFY(!worker.connect());
}

void SqlWorkerTest::testConnectDisabled()
{
	SqlWorker worker("IGNORED_BECAUSE_DISABLED", "", "", "", "", nullptr);

	QVERIFY(worker.connect());
	QVERIFY(worker.connect());
}


void SqlWorkerTest::testEscapeInteger()
{
	SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

	QCOMPARE(worker.escape(-3), QString("-3"));
	QCOMPARE(worker.escape(0), QString("0"));
	QCOMPARE(worker.escape(21), QString("21"));
	QCOMPARE(worker.escape(12345), QString("12345"));
}

void SqlWorkerTest::testEscapeString()
{
	SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

	QCOMPARE(worker.escape("test"), QString("'test'"));
	QCOMPARE(worker.escape("test'ed"), QString("'test''ed'"));
}


void SqlWorkerTest::testExecCreateAndInsert()
{
	SqlWorker worker("QSQLITE", "", "", "", "test_sql_worker.db", nullptr);

	QVERIFY(worker.execute("CREATE TABLE IF NOT EXISTS test_table (some_value INT);"));
	QVERIFY(worker.execute("INSERT INTO test_table (some_value) VALUES (1), (3), (21);"));

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("test_sql_worker.db");
	QVERIFY(db.open());

	QSqlQuery query = db.exec("SELECT some_value FROM test_table");
	int idVal = query.record().indexOf("some_value");
	QList<int> values;
	while (query.next())
		values.append(query.value(idVal).toInt());

	QCOMPARE(values, QList<int>() << 1 << 3 << 21);
}


static SqlWorkerTest instance;
