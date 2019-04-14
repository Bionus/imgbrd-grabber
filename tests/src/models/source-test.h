#ifndef SOURCE_TEST_H
#define SOURCE_TEST_H

#include "test-suite.h"


class Profile;
class QSettings;
class Source;

class SourceTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testMissingJavascript();
		void testInvalidJavascript();
		void testMissingSites();
		void testIgnoreEmptySites();

	private:
		Profile *m_profile;
};

#endif // SOURCE_TEST_H
