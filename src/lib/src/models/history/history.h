#ifndef HISTORY_H
#define HISTORY_H

#include <QList>
#include <QString>
#include "history-entry.h"
#include "models/search-query/search-query.h"


class Site;

class History
{
	public:
		explicit History(const QString &file, Profile *profile);
		bool load();
		bool save();

		void addQuery(const SearchQuery &query, const QList<Site*> &sites);
		void clear();

	private:
		QString m_file;
		Profile *m_profile;
		QList<HistoryEntry> m_entries;
		unsigned int m_maxEntries;
};

#endif // HISTORY_H
