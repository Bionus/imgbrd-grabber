#ifndef LOADER_DATA_H
#define LOADER_DATA_H

#include <QList>
#include <QSharedPointer>
#include "loader/downloadable.h"


struct LoaderData
{
	// Progress indicators
	int position = -1;
	int max = -1;

	// Results
	QList<QSharedPointer<Downloadable>> ignored;
	QList<QSharedPointer<Downloadable>> results;
};

#endif // LOADER_DATA_H
