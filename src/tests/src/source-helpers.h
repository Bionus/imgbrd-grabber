#ifndef SOURCE_HELPERS_H
#define SOURCE_HELPERS_H

#include <QString>


class Profile;

Profile *makeProfile();
void setupSource(const QString &source, QString dir = QString());
void setupSite(const QString &source, const QString &site, QString dir = QString());

#endif // SOURCE_HELPERS_H
