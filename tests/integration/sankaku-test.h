#ifndef SANKAKU_TEST_H
#define SANKAKU_TEST_H

#include "integration-test-suite.h"


class SankakuTest : public IntegrationTestSuite
{
	Q_OBJECT

    private slots:
		void testHtml();
        void testAnimatedUrls();
};

#endif // SANKAKU_TEST_H
