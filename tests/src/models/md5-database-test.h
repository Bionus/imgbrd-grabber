#ifndef MD5_DATABASE_TEST_H
#define MD5_DATABASE_TEST_H

#include <QSettings>
#include "test-suite.h"


class Md5DatabaseTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testLoad();
		void testAdd();
		void testUpdate();
		void testRemove();
		void testActionDontKeepDeleted();
		void testActionKeepDeleted();

	private:
		QSettings *m_settings;
};

#endif // MD5_DATABASE_TEST_H
