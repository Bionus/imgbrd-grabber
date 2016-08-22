#ifndef SANKAKU_TEST_H
#define SANKAKU_TEST_H

#include "test-suite.h"
#include "downloader.h"
#include <QSettings>


class SankakuTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

		void myInit(QString source);

		void testHtml();

	private:
		Downloader	*m_downloader;
		QSettings	*m_settings;
		Site		*m_site;
};

#endif // SANKAKU_TEST_H
