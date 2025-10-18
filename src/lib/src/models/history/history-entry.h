#ifndef HISTORY_ENTRY_H
#define HISTORY_ENTRY_H

#include <QDateTime>
#include <QList>
#include "models/search-query/search-query.h"


class Profile;
class Site;

struct HistoryEntry
{
    SearchQuery query;
    QList<Site*> sites;
    QDateTime date;

    static HistoryEntry fromJson(const QJsonObject &json, Profile *profile);
    void toJson(QJsonObject &json) const;
};

#endif //HISTORY_ENTRY_H