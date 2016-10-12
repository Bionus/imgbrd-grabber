#include <QtTest>
#include "page-api-test.h"
#include "models/tag.h"
#include "models/source.h"


void PageApiTest::init()
{
	m_sites.append(new Site("danbooru.donmai.us", new Source("release/sites/Danbooru (2.0)")));
	m_site = new Site("gelbooru.com", new Source("release/sites/Gelbooru (0.2)"));
}

void PageApiTest::cleanup()
{
	m_sites.first()->deleteLater();
	m_site->deleteLater();
}


void PageApiTest::testParseUrlBasic()
{
	QStringList tags = QStringList() << "test" << "tag";
	Page page(&profile, m_site, m_sites, tags);
	PageApi pageApi(&page, &profile, m_site, m_site->getApis().first(), tags);

	QCOMPARE(pageApi.parseUrl("/index.php?page=post&s=list&pid={pid}&tags={tags}").toString(),
			 QString("http://gelbooru.com/index.php?page=post&s=list&pid=0&tags=test tag"));
}

void PageApiTest::testParseUrlLogin()
{
	Site *site = m_sites.first();
	site->setUsername("user");
	site->setPassword("pass");

	QStringList tags = QStringList() << "test" << "tag";
	Page page(&profile, site, m_sites, tags);
	PageApi pageApi(&page, &profile, site, site->getApis().first(), tags);

	QCOMPARE(pageApi.parseUrl("/posts.xml?{login}limit={limit}&page={page}{altpage}&tags={tags}").toString(),
			 QString("http://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=1&tags=test tag"));
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
			 QString("http://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=b0&tags=test tag"));
}


static PageApiTest instance;
