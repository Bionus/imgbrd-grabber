#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include <QString>
#include "history-entry.h"
#include "history-key.h"
#include "models/search-query/search-query.h"


class Site;

 class History : public QObject
{
	Q_OBJECT

	public:
		explicit History(const QString &file, Profile *profile, QObject *parent = nullptr);
		bool load();
		bool save();

		void addQuery(const SearchQuery &query, const QList<Site*> &sites);
		void removeQuery(const SearchQuery &query, const QList<Site*> &sites);
		void clear();

		const QList<QSharedPointer<HistoryEntry>> &entries() const;

	signals:
		void changed();

	private:
		QString m_file;
		Profile *m_profile;
		QList<QSharedPointer<HistoryEntry>> m_entries;
		QHash<HistoryKey, QSharedPointer<HistoryEntry>> m_entriesMap;
		unsigned int m_maxEntries;
};

#endif // HISTORY_H
