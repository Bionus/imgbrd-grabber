#include <QtTest>
#include "tag-database-sqlite-test.h"
#include "tags/tag-database-sqlite.h"


TagDatabaseSqliteTest::TagDatabaseSqliteTest()
	: TagDatabaseTestSuite(new TagDatabaseSqlite("tests/resources/tag-types.txt", "tests/resources/tags.db"))
{}


static TagDatabaseSqliteTest instance;
