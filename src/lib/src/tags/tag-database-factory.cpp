#include "tags/tag-database-factory.h"
#include <QFile>
#include "tags/tag-database-in-memory.h"
#include "tags/tag-database-sqlite.h"
#include "utils/read-write-path.h"


TagDatabase *TagDatabaseFactory::Create(const ReadWritePath &directory)
{
	const ReadWritePath typesFile = directory.readWritePath("tag-types.txt");

	if (QFile::exists(directory.writePath("tags.txt"))) {
		return new TagDatabaseInMemory(typesFile, directory.writePath("tags.txt"));
	}

	return new TagDatabaseSqlite(typesFile, directory.writePath("tags.db"));
}
