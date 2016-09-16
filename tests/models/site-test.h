#ifndef SITE_TEST_H
#define SITE_TEST_H

#include "test-suite.h"
#include "models/source.h"
#include "models/site.h"
#include <QSettings>


class SiteTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testSetUsername();
		void testSetPassword();

		void testFixUrlBasic();
		void testFixUrlRoot();
		void testFixUrlRelative();

		void testGetSites();

		/**
		 * Multiple calls should always return the very same pointer (not only the same
		 * data) as the result should be cached.
		 */
		//void testGetAllCached();

		void testLoadTags();
		//void testCheckForUpdates();
		void testCookies();
		void testLoginNone();
		void testLoginGet();
		void testLoginPost();

	private:
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
};

#endif // SITE_TEST_H
