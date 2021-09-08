#ifndef ZIP_H
#define ZIP_H

#include <QString>


bool createZip(const QString &filePath, const QHash<QString, QString> &files);
bool unzipFile(const QString &filePath, const QString &destinationDir);

#endif // ZIP_H
