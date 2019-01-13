#ifndef BACKPORTS_H
#define BACKPORTS_H

#include <QtGlobal>


#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
	#include "qasconst.h"
	#include "qoverload.h"
#endif

#endif // BACKPORTS_H