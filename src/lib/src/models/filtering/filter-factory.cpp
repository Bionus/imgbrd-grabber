#include "filter-factory.h"
#include "meta-filter.h"
#include "tag-filter.h"
#include "token-filter.h"


Filter *FilterFactory::build(QString filter)
{
	bool invert = false;

	// Invert the filter by prepending '-'
	if (filter.startsWith('-')) {
		filter = filter.right(filter.length() - 1);
		invert = true;
	}

	// Tokens
	if (filter.startsWith('%') && filter.endsWith('%')) {
		const QString token = filter.mid(1, filter.length() - 2);

		return new TokenFilter(token, invert);
	}

	// Meta-tags
	int colonPos = filter.lastIndexOf(':');
	if (colonPos != -1) {
		if (colonPos > 0 && colonPos < filter.length() - 1 && filter[colonPos + 1] != ':') {
			const QString type = filter.mid(0, colonPos).toLower();
			const QString val = filter.mid(colonPos + 1).toLower();

			return new MetaFilter(type, val, invert);
		}

		// Un-escape '::'
		filter = filter.replace("::", ":");
	}

	// Tags
	if (!filter.isEmpty()) {
		return new TagFilter(filter.trimmed(), invert);
	}

	return nullptr;
}
