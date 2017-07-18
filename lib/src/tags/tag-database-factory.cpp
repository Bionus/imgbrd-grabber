#include "tag-database-factory.h"
#include "tag-database-in-memory.h"
#include "tag-database-sqlite.h"
#include <QFile>


TagDatabase *TagDatabaseFactory::Create(QString directory)
{
	if (!directory.endsWith("/") && !directory.endsWith("\\"))
		directory += "/";

	QString typesFile = directory + "tag-types.txt";

	if (QFile::exists(directory + "tags.txt"))
		return new TagDatabaseInMemory(typesFile, directory + "tags.txt");

	return new TagDatabaseSqlite(typesFile, directory + "tags.db");
}
