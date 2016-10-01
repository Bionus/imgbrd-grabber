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

		void testLoadFavorites();
		void testAddFavorite();
		void testRemoveFavorite();

	private:
		Profile *m_profile;
};

#endif // PROFILE_TEST_H
