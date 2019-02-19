#include "filename/conditional-filename.h"


ConditionalFilename::ConditionalFilename(QString condition, const QString &filename, QString path)
	: condition(std::move(condition)), filename(filename), path(std::move(path))
{}
