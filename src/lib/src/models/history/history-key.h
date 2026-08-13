#ifndef HISTORY_KEY_H
#define HISTORY_KEY_H

#include <QList>
#include "models/search-query/search-query.h"


class Site;

struct HistoryKey
{
	SearchQuery query;
	QList<Site*> sites;
};


inline bool operator==(const HistoryKey &lhs, const HistoryKey &rhs)
{
	return lhs.query == rhs.query && lhs.sites == rhs.sites;
}

inline size_t qHash(const HistoryKey &key, size_t seed)
{
	// FIXME(Bionus): use key.query and implement qHash on SearchQuery
	return qHashMulti(seed, key.query.tags, key.sites);
}

#endif // HISTORY_KEY_H
