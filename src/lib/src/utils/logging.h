#ifndef UTILS_LOGGING_H
#define UTILS_LOGGING_H

#include <QString>


class Profile;

void logSystemInformation(Profile *profile);
QString logToHtml(const QString &msg);

#endif // UTILS_LOGGING_H
