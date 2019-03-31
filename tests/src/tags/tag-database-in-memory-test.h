#ifndef TAG_DATABASE_IN_MEMORY_TEST_H
#define TAG_DATABASE_IN_MEMORY_TEST_H

#include "tag-database-test-suite.h"


class TagDatabaseInMemoryTest : public TagDatabaseTestSuite
{
	Q_OBJECT

	public:
		TagDatabaseInMemoryTest();

	private slots:
		void loadNonExistingFile();
		void loadEmpty();
		void testLoadInvalidTypes();
		void loadInvalidLines();
		void loadValidData();
		void saveEmpty();
		void saveData();
};

#endif // TAG_DATABASE_IN_MEMORY_TEST_H
