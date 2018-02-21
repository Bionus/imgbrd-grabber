#ifndef SOURCE_TEST_H
#define SOURCE_TEST_H

#include <QSettings>
#include "models/site.h"
#include "models/source.h"
#include "test-suite.h"


class SourceTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testMissingXml();
		void testInvalidXml();
		void testMissingSites();
		void testIgnoreEmptySites();

	private:
		QSettings *m_settings;
		Profile *m_profile;
		Source *m_source;
};

#endif // SOURCE_TEST_H
