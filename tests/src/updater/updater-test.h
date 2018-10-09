#ifndef UPDATER_TEST_H
#define UPDATER_TEST_H

#include "test-suite.h"
#include "updater/program-updater.h"


class UpdaterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testCompareEqual();
		void testCompareEqualAlphas();

		void testCompareMinor();
		void testCompareNormal();
		void testCompareMajor();
		void testCompareTen();
		void testCompareMissing();

		void testCompareAlphas();
		void testCompareAlphaToNew();
		void testCompareAlphaToOld();
		void testCompareAlphaToBeta();

	private:
		ProgramUpdater m_updater;
};

#endif // UPDATER_TEST_H
