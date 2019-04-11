#ifndef URL_LOGIN_TEST_H
#define URL_LOGIN_TEST_H

#include "network/network-manager.h"
#include "test-suite.h"


class Profile;
class Site;
class Source;

class UrlLoginTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testNonTestable();
		void testLoginSuccess();
		void testLoginFailure();
		void testComplementUrl();

	private:
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
		NetworkManager m_manager;
};

#endif // URL_LOGIN_TEST_H
