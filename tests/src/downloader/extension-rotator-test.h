#ifndef EXTENSION_ROTATOR_TEST_H
#define EXTENSION_ROTATOR_TEST_H

#include "downloader/extension-rotator.h"
#include "test-suite.h"


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
		void testCopyConstructor();
};

#endif // EXTENSION_ROTATOR_TEST_H
