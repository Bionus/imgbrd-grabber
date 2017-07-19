#include <QtTest>
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


static SqlWorkerTest instance;
