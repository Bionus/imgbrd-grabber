#ifndef INTEGRATION_TEST_SUITE_H
#define INTEGRATION_TEST_SUITE_H

#include <QList>
#include <QString>
#include <QStringList>
#include "test-suite.h"


class Downloader;
class Image;
class Profile;
class Site;
class Source;
class Tag;

class IntegrationTestSuite : public TestSuite
{
	Q_OBJECT

	private slots:
		void initTestCase();
		void cleanup();

	protected:
		QList<Image*> getImages(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file);
		QList<Tag> getPageTags(const QString &source, const QString &site, const QString &format, const QString &tags, const QString &file);
		QList<Tag> getTags(const QString &source, const QString &site, const QString &format, const QString &file);

	protected:
		Downloader *m_downloader;
		Profile *m_profile;
		Source *m_source;
		Site *m_site;
		QStringList m_filesToRemove;
};

#endif // INTEGRATION_TEST_SUITE_H
