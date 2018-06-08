#ifndef LOADER_DATA_H
#define LOADER_DATA_H

#include <QList>
#include <QPair>
#include <QSharedPointer>
#include <QStringList>
#include <QUrl>
#include "loader/downloadable.h"


struct LoaderData
{
	// Progress indicators
	int position;
	int max;

	// Results
	QList<QSharedPointer<Downloadable>> ignored;
	QList<QSharedPointer<Downloadable>> results;
};

#endif // LOADER_DATA_H
