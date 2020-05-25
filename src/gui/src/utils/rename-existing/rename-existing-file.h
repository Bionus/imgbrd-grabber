#ifndef RENAME_EXISTING_FILE_H
#define RENAME_EXISTING_FILE_H

#include <QString>
#include <QStringList>


struct RenameExistingFile
{
	QString md5;
	QString path;
	QString newPath;
	QStringList children;
};

#endif // RENAME_EXISTING_FILE_H
