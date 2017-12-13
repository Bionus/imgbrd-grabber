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

		void testDefaultApis();
		void testNoApis();

		void testSetUsername();
		void testSetPassword();

		void testFixUrlBasic();
		void testFixUrlRoot();
		void testFixUrlRelative();

		void testGetSites();

		void testLoadTags();
		void testCookies();
		void testLoginNone();
		void testLoginGet();
		void testLoginPost();

	private:
		Profile *m_profile;
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
};

#endif // SITE_TEST_H
