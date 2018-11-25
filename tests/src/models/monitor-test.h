#ifndef MONITOR_TEST_H
#define MONITOR_TEST_H

#include "test-suite.h"


class Profile;
class Site;
class Source;

class MonitorTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testSite();
		void testInterval();
		void testLastCheck();
		void testCumulated();
		void testSerialization();
		void testCompare();

	private:
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
};

#endif // MONITOR_TEST_H
