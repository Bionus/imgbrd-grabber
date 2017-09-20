#ifndef DERPIBOORU_TEST_H
#define DERPIBOORU_TEST_H

#include "integration-test-suite.h"


class DerpibooruTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testJson();
		void testHtmlTags();
		void testJsonTags();
};

#endif // DERPIBOORU_TEST_H
