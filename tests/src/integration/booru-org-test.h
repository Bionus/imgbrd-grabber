#ifndef BOORU_ORG_TEST_H
#define BOORU_ORG_TEST_H

#include "integration-test-suite.h"


class BooruOrgTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testPageTags();
};

#endif // BOORU_ORG_TEST_H
