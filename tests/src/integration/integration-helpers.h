#ifndef INTEGRATION_HELPERS_H
#define INTEGRATION_HELPERS_H

#include <QList>
#include <QString>


class Image;
class Tag;

QList<Image*> getImages(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file);
QList<Tag> getPageTags(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file);
QList<Tag> getTags(const QString &source, const QString &site, const QString &format, const QString &file);

#endif // INTEGRATION_HELPERS_H
