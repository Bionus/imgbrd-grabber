#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <QByteArray>
#include <QString>


bool copyRecursively(QString srcFilePath, QString tgtFilePath, bool overwrite = false);
bool safeCopyFile(const QString &from, const QString &dest, bool backup = false);
bool safeWriteFile(const QString &filePath, const QByteArray &data, bool backup = false);

bool ensureFileParent(const QString &filePath);
bool writeFile(const QString &filePath, const QByteArray &data);

#endif // FILE_UTILS_H
