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

	QMap<int, TagType> types = loadTypes(m_typeFile);

	QFile file(m_tagFile);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return false;

	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine();

		QStringList data = line.split(',');
		if (data.count() != 2)
			continue;

		int tId = data[1].toInt();
		if (!types.contains(tId))
			continue;

		m_database.insert(data[0], types[tId]);
	}
	file.close();

	return true;
}

QMap<int, TagType> TagDatabase::loadTypes(QString filename) const
{
	QMap<int, TagType> types;

	QFile f(filename);
	if (f.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream in(&f);
		while (!in.atEnd())
		{
			QString line = in.readLine();

			QStringList data = line.split(',');
			if (data.count() != 2)
				continue;

			types.insert(data[0].toInt(), TagType(data[1]));
		}
		f.close();
	}

	return types;
}

TagType TagDatabase::getTagType(QString tag) const
{
	if (m_database.contains(tag))
		return m_database[tag];

	return TagType("unknown");
}
