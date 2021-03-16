#ifndef WINDOWS_FILE_PROPERTY_H
#define WINDOWS_FILE_PROPERTY_H

#include <Windows.h>
#include <QMap>
#include <QString>


bool initialize();
bool getAllProperties(const QString &filename, QMap<QString, QString> &out);
bool getProperty(const QString &filename, const QString &property, QString &out);
bool setProperty(const QString &filename, const QString &property, const QString &value);

#endif // WINDOWS_FILE_PROPERTY_H
