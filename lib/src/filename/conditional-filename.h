#ifndef CONDITIONAL_FILENAME_H
#define CONDITIONAL_FILENAME_H

#include <QString>
#include "models/filename.h"


class ConditionalFilename
{
	public:
		ConditionalFilename(QString condition, const QString &filename, QString path);

		QString condition;
		Filename filename;
		QString path;
};

#endif // CONDITIONAL_FILENAME_H
