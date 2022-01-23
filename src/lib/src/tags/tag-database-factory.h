#ifndef TAG_DATABASE_FACTORY_H
#define TAG_DATABASE_FACTORY_H

class QString;
class ReadWritePath;
class TagDatabase;


class TagDatabaseFactory
{
	public:
		static TagDatabase *Create(const ReadWritePath &directory);
};

#endif // TAG_DATABASE_FACTORY_H
