#ifndef LOADER_DATA_H
#define LOADER_DATA_H

#include <QList>
#include <QPair>
#include <QUrl>
#include <QStringList>
#include <QSharedPointer>
#include "downloadable.h"


struct LoaderData
{
	// Progress indicators
	int position;
	int max;

	// Results
	int ignored;
	QList<QSharedPointer<Downloadable>> results;
};

#endif // LOADER_DATA_H
