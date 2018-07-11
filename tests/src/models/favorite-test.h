#ifndef FAVORITE_TEST_H
#define FAVORITE_TEST_H

#include "models/favorite.h"
#include "test-suite.h"


class FavoriteTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testBasicConstructor();
		void testGetName();
		void testGetNameClean();
		void testGetNote();
		void testSetNote();
		void testGetLastViewed();
		void testSetLastViewed();
		void testGetImagePath();
		void testSetImagePath();
		void testGetMonitors();
		void testEquals();
		void testEqualsAll();
		void testEqualsCase();
		void testNotEquals();
#ifndef HEADLESS
		void testSetImageFirst();
		void testGetImageNotExists();
		void testGetImageBig();
		void testGetImageSmall();
		void testGetImageResize();
#endif
		void testToString();
		void testFromString();
		void testSortByNote();
		void testSortByName();
		void testSortByLastViewed();
};

#endif // FAVORITE_TEST_H
