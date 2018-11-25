#include "updater-test.h"
#include <QtTest>


void UpdaterTest::testCompareEqual()
{
	QCOMPARE(m_updater.compareVersions("1.0.0", "1.0.0"), 0);
	QCOMPARE(m_updater.compareVersions("1.4.0", "1.4.0"), 0);
	QCOMPARE(m_updater.compareVersions("1.4.7", "1.4.7"), 0);
}

void UpdaterTest::testCompareEqualAlphas()
{
	QCOMPARE(m_updater.compareVersions("1.0.0a2", "1.0.0a2"), 0);
	QCOMPARE(m_updater.compareVersions("1.4.0a2", "1.4.0a2"), 0);
	QCOMPARE(m_updater.compareVersions("1.4.7a2", "1.4.7a2"), 0);
}


void UpdaterTest::testCompareMinor()
{
	QCOMPARE(m_updater.compareVersions("1.0.1", "1.0.0"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0", "1.0.1"), -1);
}

void UpdaterTest::testCompareNormal()
{
	QCOMPARE(m_updater.compareVersions("1.1.0", "1.0.0"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0", "1.1.0"), -1);
}

void UpdaterTest::testCompareMajor()
{
	QCOMPARE(m_updater.compareVersions("2.0.0", "1.0.0"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0", "2.0.0"), -1);
}

void UpdaterTest::testCompareTen()
{
	QCOMPARE(m_updater.compareVersions("2.0.0", "1.10.0"), 1);
	QCOMPARE(m_updater.compareVersions("1.10.0", "2.0.0"), -1);
}

void UpdaterTest::testCompareMissing()
{
	QCOMPARE(m_updater.compareVersions("1.0.1", "1.0"), 1);
	QCOMPARE(m_updater.compareVersions("1.0", "1.0.1"), -1);
}


void UpdaterTest::testCompareAlphas()
{
	QCOMPARE(m_updater.compareVersions("1.0.0a3", "1.0.0a2"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0a2", "1.0.0a3"), -1);
}

void UpdaterTest::testCompareAlphaToNew()
{
	QCOMPARE(m_updater.compareVersions("1.0.0", "1.0.0a3"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0a3", "1.0.0"), -1);
}

void UpdaterTest::testCompareAlphaToOld()
{
	QCOMPARE(m_updater.compareVersions("1.0.0a3", "0.1.0"), 1);
	QCOMPARE(m_updater.compareVersions("0.1.0", "1.0.0a3"), -1);
}

void UpdaterTest::testCompareAlphaToBeta()
{
	QCOMPARE(m_updater.compareVersions("1.0.0b1", "1.0.0a3"), 1);
	QCOMPARE(m_updater.compareVersions("1.0.0a3", "1.0.0b1"), -1);
}


QTEST_MAIN(UpdaterTest)
