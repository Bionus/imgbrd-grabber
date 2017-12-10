#ifndef INTEGRATION_TEST_SUITE_H
#define INTEGRATION_TEST_SUITE_H

#include <QSettings>
#include "test-suite.h"
#include "downloader/downloader.h"
#include "models/source.h"
#include "models/site.h"
#include "models/image.h"


class IntegrationTestSuite : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

	protected:
		QList<Image*> getImages(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file = "");
		QList<Tag> getPageTags(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file = "");
		QList<Tag> getTags(const QString &site, const QString &source, const QString &format, const QString &file = "");

	protected:
		Downloader *m_downloader;
		Site *m_site;
		QStringList m_filesToRemove;
};

#endif // INTEGRATION_TEST_SUITE_H
