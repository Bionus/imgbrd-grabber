#ifndef TAG_SEARCH_QUERY_H
#define TAG_SEARCH_QUERY_H

#include <QStringList>
#include "models/search-query/search-query.h"


class TagSearchQuery //: public SearchQuery
{
	public:
		explicit TagSearchQuery(QStringList tags);
		QStringList tags() const;

	private:
		QStringList m_tags;
};

#endif // TAG_SEARCH_QUERY_H
