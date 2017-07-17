#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QString>
#include "tag-type.h"


class TagDatabase
{
	public:
		TagDatabase(QString typeFile, QString tagFile);
		bool load();
		TagType getTagType(QString tag) const;

	protected:
		QMap<int, TagType> loadTypes(QString filename) const;

	private:
		QString m_typeFile;
		QString m_tagFile;
		QMap<QString, TagType> m_database;
};

#endif // TAG_DATABASE_H
