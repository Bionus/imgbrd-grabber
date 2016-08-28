#ifndef ZEROCHAN_TEST_H
#define ZEROCHAN_TEST_H

#include "integration-test-suite.h"


class ZerochanTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
        void testHtml();
		void testRss();
};

#endif // ZEROCHAN_TEST_H
