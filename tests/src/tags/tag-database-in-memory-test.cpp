#include <QtTest>
#include "tag-database-in-memory-test.h"
#include "tags/tag-database-in-memory.h"


TagDatabaseInMemoryTest::TagDatabaseInMemoryTest()
	: TagDatabaseTestSuite(new TagDatabaseInMemory("tests/resources/tag-types.txt", "tests/resources/tags.txt"))
{}


static TagDatabaseInMemoryTest instance;
