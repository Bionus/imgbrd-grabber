#ifndef DANBOORU_TEST_H
#define DANBOORU_TEST_H

#include "integration-test-suite.h"


class DanbooruTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testXml();
		void testPageTags();
		void testHtmlTags();
		void testXmlTags();
		void testJsonTags();
};

#endif // DANBOORU_TEST_H
