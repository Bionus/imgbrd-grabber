#include "tag-database.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>


TagDatabase::TagDatabase(QString typeFile, QString tagFile)
	: m_typeFile(typeFile), m_tagFile(tagFile)
{}

bool TagDatabase::load()
{
	// Don't reload databases
	if (!m_database.isEmpty())
		return true;

	loadTypes(m_typeFile);

	QFile file(m_tagFile);
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

		m_database.insert(data[0], m_tagTypes[tId]);
	}
	file.close();

	return true;
}

void TagDatabase::loadTypes(QString filename)
{
	if (!m_tagTypes.isEmpty())
		return;

	QFile f(filename);
	if (!f.open(QFile::ReadOnly | QFile::Text))
		return;

	QTextStream in(&f);
	while (!in.atEnd())
	{
		QString line = in.readLine();

		QStringList data = line.split(',');
		if (data.count() != 2)
			continue;

		m_tagTypes.insert(data[0].toInt(), TagType(data[1]));
	}
	f.close();
}

bool TagDatabase::save()
{
	QFile file(m_tagFile);
	if (!file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text))
		return false;

	// Inverted tag type map to get the tag type ID from its name
	QMap<QString, int> tagTypes;
	for (int typeId : m_tagTypes.keys())
		tagTypes.insert(m_tagTypes[typeId].name(), typeId);

	QMapIterator<QString, TagType> i(m_database);
	while (i.hasNext())
	{
		i.next();

		TagType tagType = i.value();
		int tagTypeId = tagTypes.contains(tagType.name()) ? tagTypes[tagType.name()] : -1;

		file.write(QString(i.key() + "," + QString::number(tagTypeId) + "\n").toUtf8());
	}

	file.close();
}

void TagDatabase::setTags(const QList<Tag> &tags)
{
	m_database.clear();
	for (Tag tag : tags)
		m_database.insert(tag.text(), tag.type());
}

TagType TagDatabase::getTagType(QString tag) const
{
	if (m_database.contains(tag))
		return m_database[tag];

	return TagType("unknown");
}

QMap<int, TagType> TagDatabase::tagTypes() const
{
	return m_tagTypes;
}
