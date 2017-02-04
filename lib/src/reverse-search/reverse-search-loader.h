#ifndef REVERSE_SEARCH_LOADER_H
#define REVERSE_SEARCH_LOADER_H

#include <QSettings>
#include <QList>
#include "reverse-search-engine.h"


class ReverseSearchLoader
{
	public:
		ReverseSearchLoader(QSettings *settings);
		QList<ReverseSearchEngine> getAllReverseSearchEngines() const;

	private:
		QSettings *m_settings;
};

#endif // REVERSE_SEARCH_LOADER_H
