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
		void cleanup();

		void myInit(QString source);

		void testHtml();
		void testXml();

	private:
		Downloader	*m_downloader;
		QSettings	*m_settings;
		Site		*m_site;
};

#endif // DANBOORU_TEST_H
