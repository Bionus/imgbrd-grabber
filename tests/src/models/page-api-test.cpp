#include "page-api-test.h"
#include <QtTest>
#include "models/source.h"


void PageApiTest::init()
{
	setupSource("Danbooru (2.0)");
	setupSite("Danbooru (2.0)", "danbooru.donmai.us");

	setupSource("Gelbooru (0.2)");
	setupSite("Gelbooru (0.2)", "gelbooru.com");

	QString path = "tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini";
	QSettings settings(path, QSettings::IniFormat);
	settings.setValue("auth/pseudo", "user");
	settings.setValue("auth/password", "pass");
	settings.sync();

	m_profile = new Profile("tests/resources/");
	m_sites.append(new Site("danbooru.donmai.us", new Source(m_profile, "tests/resources/sites/Danbooru (2.0)")));
	m_site = new Site("gelbooru.com", new Source(m_profile, "tests/resources/sites/Gelbooru (0.2)"));
}

void PageApiTest::cleanup()
{
	m_sites.first()->deleteLater();
	m_site->deleteLater();

	QFile::remove("tests/resources/sites/Danbooru (2.0)/danbooru.donmai.us/defaults.ini");
}


void PageApiTest::testParseUrlBasic()
{
	QStringList tags = QStringList() << "test" << "tag";
	Page page(m_profile, m_site, m_sites, tags);
	PageApi pageApi(&page, m_profile, m_site, m_site->getApis().first(), tags);

	QCOMPARE(pageApi.url().toString(), QString("https://gelbooru.com/index.php?page=dapi&s=post&q=index&limit=25&pid=0&tags=test tag"));
}

void PageApiTest::testParseUrlLogin()
{
	Site *site = m_sites.first();

	QStringList tags = QStringList() << "test" << "tag";
	Page page(m_profile, site, m_sites, tags);
	PageApi pageApi(&page, m_profile, site, site->getApis().first(), tags);

	QCOMPARE(pageApi.url().toString(), QString("https://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=1&tags=test tag"));
}

void PageApiTest::testParseUrlAltPage()
{
	Site *site = m_sites.first();

	QStringList tags = QStringList() << "test" << "tag";
	Page prevPage(m_profile, site, m_sites, tags, 1000);
	Page page(m_profile, site, m_sites, tags, 1001);
	PageApi pageApi(&page, m_profile, site, site->getApis().first(), tags, 1001);
	pageApi.setLastPage(&prevPage);

	QCOMPARE(pageApi.url().toString(), QString("https://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=b0&tags=test tag"));
}


static PageApiTest instance;
