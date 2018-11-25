#ifndef PACK_LOADER_TEST_H
#define PACK_LOADER_TEST_H

#include "test-suite.h"


class Profile;
class Site;

class PackLoaderTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testGetQuery();
		void testBasic();
		void testWrongResultsCount();
		void testGalleries();

	protected:
		QList<int> getResults(Profile *profile, Site *site, QString search, int perPage, int total, int packSize, bool galleriesCountAsOne);

	private:
		Profile *m_profile;
};

#endif // PACK_LOADER_TEST_H
