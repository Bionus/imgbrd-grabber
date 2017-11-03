#ifndef EXTENSION_ROTATOR_TEST_H
#define EXTENSION_ROTATOR_TEST_H

#include "test-suite.h"
#include "downloader/extension-rotator.h"


class ExtensionRotatorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testBasic();
		void testLoop();
		void testNotFound();
		void testKeepEmpty();
		void testEmptyFirst();
		void testEmptyList();
		void testEmptyBoth();
};

#endif // EXTENSION_ROTATOR_TEST_H
