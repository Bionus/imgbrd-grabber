#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QString>
#include "tag-type.h"


class TagDatabase
{
	public:
		TagDatabase();
		bool load(QString typeFile, QString tagFile);
		TagType getTagType(QString tag) const;

	protected:
		QMap<int, TagType> loadTypes(QString filename) const;

	private:
		QMap<QString, TagType> m_database;
};

#endif // TAG_DATABASE_H
