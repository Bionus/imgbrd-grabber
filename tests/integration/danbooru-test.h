#ifndef DANBOORU_TEST_H
#define DANBOORU_TEST_H

#include "integration-test-suite.h"


class DanbooruTest : public IntegrationTestSuite
{
	Q_OBJECT

	private slots:
		void testHtml();
		void testXml();

	private:
		Downloader	*m_downloader;
		QSettings	*m_settings;
		Site		*m_site;
};

#endif // DANBOORU_TEST_H
