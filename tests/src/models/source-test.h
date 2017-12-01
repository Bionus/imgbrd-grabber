#ifndef SOURCE_TEST_H
#define SOURCE_TEST_H

#include "test-suite.h"
#include "models/source.h"
#include "models/site.h"
#include <QSettings>


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
		Source *m_source;
};

#endif // SOURCE_TEST_H
