#include "tag-database.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>


TagDatabase::TagDatabase(QString typeFile)
	: m_typeFile(typeFile)
{}

bool TagDatabase::load()
{
	loadTypes(m_typeFile);
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

QMap<int, TagType> TagDatabase::tagTypes() const
{
	return m_tagTypes;
}
