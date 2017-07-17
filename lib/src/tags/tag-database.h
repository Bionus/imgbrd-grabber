#ifndef TAG_DATABASE_H
#define TAG_DATABASE_H

#include <QString>
#include <QMap>
#include "tag-type.h"
#include "tag.h"


class TagDatabase
{
	public:
		TagDatabase(QString typeFile, QString tagFile);
		bool load();
		bool save();
		void setTags(const QList<Tag> &tags);
		TagType getTagType(QString tag) const;
		QMap<int, TagType> tagTypes() const;

	protected:
		void loadTypes(QString filename);

	private:
		QString m_typeFile;
		QString m_tagFile;
		QMap<QString, TagType> m_database;
		QMap<int, TagType> m_tagTypes;
};

#endif // TAG_DATABASE_H
