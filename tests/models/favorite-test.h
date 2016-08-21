#ifndef FAVORITE_TEST_H
#define FAVORITE_TEST_H

#include "test-suite.h"
#include "models/favorite.h"


class FavoriteTest : public TestSuite
{
	Q_OBJECT

	private slots:
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
#if !__linux__
		void testSetImageFirst();
		void testGetImageNotExists();
		void testGetImageBig();
		void testGetImageSmall();
#endif
};

#endif // FAVORITE_TEST_H
