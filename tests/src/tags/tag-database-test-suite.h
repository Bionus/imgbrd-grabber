#ifndef TAG_DATABASE_TEST_SUITE_H
#define TAG_DATABASE_TEST_SUITE_H

#include "test-suite.h"


class TagDatabase;

class TagDatabaseTestSuite : public TestSuite
{
	Q_OBJECT

	protected:
		explicit TagDatabaseTestSuite(TagDatabase *database);

	private slots:
		void initTestCase();

		void testAlreadyLoaded();
		void testTypesProperlyLoaded();
		void testEmptyContainsNone();
		void testFilledContainsAll();
		void testFilledContainsSome();

	private:
		TagDatabase *m_database;
};

#endif // TAG_DATABASE_TEST_SUITE_H
