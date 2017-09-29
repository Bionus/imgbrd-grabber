#include "tag-database-in-memory.h"
#include <QFile>
#include <QTextStream>


TagDatabaseInMemory::TagDatabaseInMemory(QString typeFile, QString tagFile)
	: TagDatabase(std::move(typeFile)), m_tagFile(std::move(tagFile))
{}

bool TagDatabaseInMemory::load()
{
	// Don't reload databases
	if (!m_database.isEmpty())
		return true;

	// Load tag types
	if (!TagDatabase::load())
		return false;

	QFile file(m_tagFile);
	if (!file.exists())
		return true;
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return false;

	QTextStream in(&file);
	while (!in.atEnd())
	{
		QString line = in.readLine();

		QStringList data = line.split(',');
		if (data.count() != 2)
			continue;

		int tId = data[1].toInt();
		if (!m_tagTypes.contains(tId))
			continue;

		QString tag = data[0];
		if (tag.isEmpty())
			continue;
		tag.squeeze();

		m_database.insert(tag, m_tagTypes[tId]);
	}
	file.close();
	m_database.squeeze();

	return true;
}

bool TagDatabaseInMemory::save()
{
	QFile file(m_tagFile);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
		return false;

	// Inverted tag type map to get the tag type ID from its name
	QMap<QString, int> tagTypes;
	for (int typeId : m_tagTypes.keys())
		tagTypes.insert(m_tagTypes[typeId].name(), typeId);

	QHashIterator<QString, TagType> i(m_database);
	while (i.hasNext())
	{
		i.next();

		TagType tagType = i.value();
		int tagTypeId = tagTypes.contains(tagType.name()) ? tagTypes[tagType.name()] : -1;

		file.write(QString(i.key() + "," + QString::number(tagTypeId) + "\n").toUtf8());
	}

	file.close();
	return true;
}

void TagDatabaseInMemory::setTags(const QList<Tag> &tags)
{
	m_database.clear();
	for (const Tag &tag : tags)
		m_database.insert(tag.text(), tag.type());
}

QMap<QString, TagType> TagDatabaseInMemory::getTagTypes(QStringList tags) const
{
	QMap<QString, TagType> ret;
	for (const QString &tag : tags)
		if (m_database.contains(tag))
			ret.insert(tag, m_database[tag]);

	return ret;
}

int TagDatabaseInMemory::count() const
{
	return m_database.count();
}
