#ifndef TAG_DATABASE_FACTORY_H
#define TAG_DATABASE_FACTORY_H

#include <QString>
#include "tags/tag-database.h"


class TagDatabaseFactory
{
	public:
		static TagDatabase *Create(QString directory);
};

#endif // TAG_DATABASE_FACTORY_H
