#ifndef DOWNLOAD_QUERY_GROUP_TEST_H
#define DOWNLOAD_QUERY_GROUP_TEST_H

#include "test-suite.h"


class DownloadQueryGroupTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testCompare();
		void testSerialization();
};

#endif // DOWNLOAD_QUERY_GROUP_TEST_H
