#ifndef DERPIBOORU_TEST_H
#define DERPIBOORU_TEST_H

#include "integration-test-suite.h"


class DerpibooruTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testJson();
};

#endif // DERPIBOORU_TEST_H
