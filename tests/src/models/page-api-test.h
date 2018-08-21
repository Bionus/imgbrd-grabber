#ifndef PAGE_API_TEST_H
#define PAGE_API_TEST_H

#include "test-suite.h"


class Profile;
class Site;

class PageApiTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testParseUrlBasic();
		void testParseUrlLogin();
		void testParseUrlAltPage();

	private:
		Profile *m_profile;
		QList<Site*> m_sites;
		Site *m_site;
};

#endif // PAGE_API_TEST_H
