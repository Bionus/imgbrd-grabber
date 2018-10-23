#ifndef OAUTH2_LOGIN_TEST_H
#define OAUTH2_LOGIN_TEST_H

#include <QNetworkAccessManager>
#include "custom-network-access-manager.h"
#include "test-suite.h"


class Profile;
class Site;
class Source;

class OAuth2LoginTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testNonTestable();
		void testLoginSuccess();
		void testLoginFailure();

	private:
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
		CustomNetworkAccessManager m_manager;
};

#endif // OAUTH2_LOGIN_TEST_H
