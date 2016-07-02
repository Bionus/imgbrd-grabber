#ifndef FAVORITE_TEST_H
#define FAVORITE_TEST_H

#include "models/favorite.h"


class FavoriteTest : public QObject
{
	Q_OBJECT

	public:
		FavoriteTest();

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
};

#endif // FAVORITE_TEST_H
