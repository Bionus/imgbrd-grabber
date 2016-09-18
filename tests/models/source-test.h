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

		/**
		 * Multiple calls should always return the very same pointer (not only the same
		 * data) as the result should be cached.
		 */
		void testGetAllCached();

		void testCheckForUpdates();

	private:
		QSettings *m_settings;
		Source *m_source;
};

#endif // SOURCE_TEST_H
