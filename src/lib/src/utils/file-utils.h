#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QByteArray>
#include <QString>


bool copyRecursively(QString srcFilePath, QString tgtFilePath, bool overwrite = false);
bool safeWriteFile(const QString &filePath, const QByteArray &data, bool backup = false);

#endif // FILE_UTILS_H
