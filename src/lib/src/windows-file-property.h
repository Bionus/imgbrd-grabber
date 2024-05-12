#ifndef WINDOWS_FILE_PROPERTY_H
#define WINDOWS_FILE_PROPERTY_H

#include <Windows.h>
#include <QMap>
#include <QString>


void initializeWindowsProperties();
void uninitializeWindowsProperties();

bool getAllWindowsProperties(const QString &filename, QMap<QString, QString> &out);
bool getWindowsProperty(const QString &filename, const QString &property, QString &out);
bool setWindowsProperty(const QString &filename, const QString &property, const QString &value);
bool clearAllWindowsProperties(const QString &filename);

#endif // WINDOWS_FILE_PROPERTY_H
