 #include "size-utils.h"
#include <QSize>


/**
 * Check that a QSize is bigger than another one.
 */
bool isBigger(QSize a, QSize b)
{
	// Invalid sizes are always considered smaller than valid ones
	if (a.width() > 0 && a.height() > 0 && b.width() <= 0 && b.height() <= 0) {
		return true;
	}

	// If images have both width and height, do a basic comparison
	if (a.width() > 0 && a.height() > 0 && b.width() > 0 && b.height() > 0) {
		return a.width() > b.width() && a.height() > b.height();
	}

	// If we have the width on both, use that first
	if (a.width() > 0 && b.width() > 0) {
		return a.width() > b.width();
	}

	// If we have the height on both, use that as fallback
	if (a.height() > 0 && b.height() > 0) {
		return a.height() > b.height();
	}

	// If we have nothing or one width and one height, we can't compare
	return false;
}

/**
 * Check if a QSize's biggest side is within a given min and max (both inclusive).
 */
bool isInRange(QSize size, int min, int max)
{
	const int biggest = qMax(size.width(), size.height());
	return biggest >= min && biggest <= max;
}
