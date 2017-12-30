#include <QtTest>
#include "page-api-test.h"
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

	m_sites.append(new Site("danbooru.donmai.us", new Source(&profile, "tests/resources/sites/Danbooru (2.0)")));
	m_site = new Site("gelbooru.com", new Source(&profile, "tests/resources/sites/Gelbooru (0.2)"));

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
	Page page(&profile, m_site, m_sites, tags);
	PageApi pageApi(&page, &profile, m_site, m_site->getApis().first(), tags);

	QCOMPARE(pageApi.parseUrl("/index.php?page=post&s=list&pid={pid}&tags={tags}").toString(),
			 QString("https://gelbooru.com/index.php?page=post&s=list&pid=0&tags=test tag"));
}

void PageApiTest::testParseUrlLogin()
{
	Site *site = m_sites.first();

	QStringList tags = QStringList() << "test" << "tag";
	Page page(&profile, site, m_sites, tags);
	PageApi pageApi(&page, &profile, site, site->getApis().first(), tags);

	QCOMPARE(pageApi.parseUrl("/posts.xml?{login}limit={limit}&page={page}{altpage}&tags={tags}").toString(),
			 QString("https://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=1&tags=test tag"));
}

void PageApiTest::testParseUrlAltPage()
{
	QStringList tags = QStringList() << "test" << "tag";
	Site *site = m_sites.first();
	Page prevPage(&profile, site, m_sites, tags, 1000);
	Page page(&profile, site, m_sites, tags, 1001);
	PageApi pageApi(&page, &profile, site, site->getApis().first(), tags, 1001);
	pageApi.setLastPage(&prevPage);

	QCOMPARE(pageApi.parseUrl("/posts.xml?{login}limit={limit}&page={altpage}&tags={tags}").toString(),
			 QString("https://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=b0&tags=test tag"));
}


static PageApiTest instance;
