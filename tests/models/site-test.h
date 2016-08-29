#ifndef SITE_TEST_H
#define SITE_TEST_H

#include "test-suite.h"
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
		void testGetAllCached();

	private:
		QSettings *m_settings;
		Site *m_site;
};

#endif // SITE_TEST_H
