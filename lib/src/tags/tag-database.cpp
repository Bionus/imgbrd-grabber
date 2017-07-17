#include "tag-database.h"
#include <QFile>


TagDatabase::TagDatabase(QString typeFile, QString tagFile)
	: m_typeFile(typeFile), m_tagFile(tagFile)
{}

bool TagDatabase::load()
{
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

	QFile f(typeFile);
	if (f.open(QFile::ReadOnly | QFile::Text))
	{
		QTextStream in(&f);
		while (!in.atEnd())
		{
			QString line = in.readLine();

			QStringList data = line.split(',');
			if (data.count() != 2)
				continue;

			types[data[0].toInt()] = data[1];
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
