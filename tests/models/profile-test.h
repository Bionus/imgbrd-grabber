#ifndef PROFILE_TEST_H
#define PROFILE_TEST_H

#include "test-suite.h"
#include "models/profile.h"


class ProfileTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testConstructorEmpty();
		void testConstructorPath();

		// Favorites
		void testLoadFavorites();
		void testAddFavorite();
		void testRemoveFavorite();
		void testRemoveFavoriteThumb();

		// MD5s
		void testLoadMd5s();
		void testAddMd5();
		void testUpdateMd5();
		void testRemoveMd5();

	private:
		Profile *m_profile;
};

#endif // PROFILE_TEST_H
