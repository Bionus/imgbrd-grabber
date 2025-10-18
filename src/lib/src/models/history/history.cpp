#include "history.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include "models/profile.h"
#include "utils/file-utils.h"


History::History(const QString &file, Profile *profile)
    : m_file(file), m_profile(profile)
{
    m_maxEntries = m_profile->getSettings()->value("History/MaxSearchCount", 100).toUInt();
    load();
}

bool History::load()
{
    QFile f(m_file);
    if (!f.open(QFile::ReadOnly)) {
        return false;
    }

    const QByteArray data = f.readAll();
    f.close();

    const QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull()) {
        return false;
    }

    m_entries.clear();

    const QJsonObject object = jsonDoc.object();
    const QJsonArray entries = object["entries"].toArray();
    for (auto entryJson : entries) {
        m_entries.append(HistoryEntry::fromJson(entryJson.toObject(), m_profile));
    }

    return true;
}

bool History::save()
{
    // Generate JSON array
    QJsonArray entries;
    for (const HistoryEntry &entry : m_entries) {
        QJsonObject entryJson;
        entry.toJson(entryJson);
        entries.append(entryJson);
    }

    // Generate result
    QJsonObject full;
    full["version"] = 1;
    full["entries"] = entries;

    // Write result
    return safeWriteFile(m_file, QJsonDocument(full).toJson());
}

void History::addQuery(const SearchQuery &query, const QList<Site*> &sites)
{
    // TODO(Bionus): add a hashmap to check if the query is already in the list of entries

    m_entries.append(HistoryEntry{
        query,
        sites,
        QDateTime::currentDateTimeUtc()
    });

    if (m_entries.size() > m_maxEntries) {
        m_entries.removeFirst();
    }
}

void History::clear()
{
    m_entries.clear();
}
