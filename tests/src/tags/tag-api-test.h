#ifndef TAG_API_TEST_H
#define TAG_API_TEST_H

#include "test-suite.h"


class Profile;
class Site;

class TagApiTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testBasic();
		void testNetworkError();
		void testParseError();
		void testDoubleLoad();
		void testRedirect();
		void testAbort();

	private:
		Profile *m_profile;
		Site *m_site;
};

#endif // TAG_API_TEST_H
