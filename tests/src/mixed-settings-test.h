#ifndef MIXED_SETTINGS_TEST_H
#define MIXED_SETTINGS_TEST_H

#include "test-suite.h"


class QSettings;

class MixedSettingsTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();

		void testEmptySettings();
		void testValueFirstValid();
		void testValueDefault();
		void testSetValueResetToParent();
		void testSetValueResetToDefault();
		void testSetValueOverrideParent();
		void testSetValueOverrideDefault();
		void testChildKeys();

	private:
		QSettings *m_child;
		QSettings *m_parent;
};

#endif // MIXED_SETTINGS_TEST_H
