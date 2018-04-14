#ifndef INTEGRATION_TEST_SUITE_H
#define INTEGRATION_TEST_SUITE_H

#include <QSettings>
#include "downloader/downloader.h"
#include "models/image.h"
#include "models/site.h"
#include "models/source.h"
#include "test-suite.h"


class IntegrationTestSuite : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

	protected:
		QList<Image*> getImages(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file, bool javaScript);
		QList<Tag> getPageTags(const QString &site, const QString &source, const QString &format, const QString &tags, const QString &file, bool javaScript);
		QList<Tag> getTags(const QString &site, const QString &source, const QString &format, const QString &file, bool javaScript);

	protected:
		Downloader *m_downloader;
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
		QStringList m_filesToRemove;
};

#endif // INTEGRATION_TEST_SUITE_H
