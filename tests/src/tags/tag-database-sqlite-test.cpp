#include "tag-database-sqlite-test.h"
#include <QtTest>
#include "tags/tag-database-sqlite.h"


TagDatabaseSqliteTest::TagDatabaseSqliteTest()
	: TagDatabaseTestSuite(new TagDatabaseSqlite("tests/resources/tag-types.txt", "tests/resources/tags.db"))
{}


QTEST_MAIN(TagDatabaseSqliteTest)
