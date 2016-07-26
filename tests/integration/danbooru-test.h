#ifndef DANBOORU_TEST_H
#define DANBOORU_TEST_H

#include "test-suite.h"
#include "downloader.h"
#include <QSettings>


class DanbooruTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void init();
		void cleanup();

		void testHtml();
		void testXml();

	private:
		Downloader	*m_downloader;
		QSettings	*m_settings;
};

#endif // DANBOORU_TEST_H
