#ifndef PAGE_TEST_H
#define PAGE_TEST_H

#include "models/page.h"
#include "models/site.h"
#include "test-suite.h"


class PageTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testIncompatibleModifiers();
		void testLoadAbort();
		void testLoadTagsAbort();

	private:
		Profile *m_profile;
		QList<Site*> m_sites;
		Site *m_site;
};

#endif // PAGE_TEST_H
