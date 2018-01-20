#ifndef PAGE_API_TEST_H
#define PAGE_API_TEST_H

#include "models/page.h"
#include "models/site.h"
#include "test-suite.h"


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
		QList<Site*> m_sites;
		Site *m_site;
};

#endif // PAGE_API_TEST_H
