#include "tags/tag-database-factory.h"
#include <QFile>
#include "tags/tag-database-in-memory.h"
#include "tags/tag-database-sqlite.h"


TagDatabase *TagDatabaseFactory::Create(QString directory)
{
	if (!directory.endsWith("/") && !directory.endsWith("\\")) {
		directory += "/";
	}

	const QString typesFile = directory + "tag-types.txt";

	if (QFile::exists(directory + "tags.txt")) {
		return new TagDatabaseInMemory(typesFile, directory + "tags.txt");
	}

	return new TagDatabaseSqlite(typesFile, directory + "tags.db");
}
