#ifndef GELBOORU_TEST_H
#define GELBOORU_TEST_H

#include "integration-test-suite.h"


class GelbooruTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testXml();
		void testPageTags();
		void testHtmlTags();
};

#endif // GELBOORU_TEST_H
