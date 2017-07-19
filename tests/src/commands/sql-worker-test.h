#ifndef SQL_WORKER_TEST_H
#define SQL_WORKER_TEST_H

#include "test-suite.h"


class SqlWorkerTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void cleanup();

		void testConnectOk();
		void testConnectError();
		void testConnectDisabled();
		void testEscapeInteger();
		void testEscapeString();
		void testExecCreateAndInsert();
};

#endif // SQL_WORKER_TEST_H
