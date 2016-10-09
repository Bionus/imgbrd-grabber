#ifndef FAVORITE_TEST_H
#define FAVORITE_TEST_H

#include "test-suite.h"
#include "models/favorite.h"


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
};

#endif // FAVORITE_TEST_H
