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
	int ddot = filter.indexOf(':');
	if (ddot != -1) {
		if (ddot > 0 && ddot < filter.length() - 1 && filter[ddot + 1] != ':') {
			const QString type = filter.mid(0, ddot).toLower();
			const QString val = filter.mid(ddot + 1).toLower();

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
