#ifndef PROFILE_TEST_H
#define PROFILE_TEST_H

#include <QDateTime>
#include <QList>
#include "test-suite.h"


class Profile;

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

	private:
		Profile *m_profile;
		QList<QDateTime> m_dates;
};

#endif // PROFILE_TEST_H
