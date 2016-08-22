#ifndef INTEGRATION_TEST_SUITE_H
#define INTEGRATION_TEST_SUITE_H

#include <QSettings>
#include "test-suite.h"
#include "downloader.h"
#include "models/site.h"


class IntegrationTestSuite : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

    protected:
        QList<Image*> getImages(QString site, QString source, QString format, QString tags);

    protected:
		Downloader	*m_downloader;
		QSettings	*m_settings;
		Site		*m_site;
};

#endif // INTEGRATION_TEST_SUITE_H
