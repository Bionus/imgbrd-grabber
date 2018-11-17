#include "tags/tag-database-factory.h"
#include <QFile>
#include "tags/tag-database-in-memory.h"
#include "tags/tag-database-sqlite.h"


TagDatabase *TagDatabaseFactory::Create(QString directory)
{
	if (!directory.endsWith("/") && !directory.endsWith("\\"))
		directory += "/";

	const QString typesFile = directory + "tag-types.txt";

	if (QFile::exists(directory + "tags.db"))
		return new TagDatabaseSqlite(typesFile, directory + "tags.db");

	return new TagDatabaseInMemory(typesFile, directory + "tags.txt");
}
