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
#ifndef Q_OS_WIN
		void testRemoveFavoriteThumb();
#endif

		// MD5s
		void testLoadMd5s();
		void testAddMd5();
		void testUpdateMd5();
		void testRemoveMd5();
		void testMd5ActionDontKeepDeleted();
		void testMd5ActionKeepDeleted();

	private:
		Profile *m_profile;
};

#endif // PROFILE_TEST_H
