#ifndef E621_TEST_H
#define E621_TEST_H

#include "integration-test-suite.h"


class E621Test : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testSwfUrls();
		void testXmlTags();
		void testJsonTags();
};

#endif // E621_TEST_H
