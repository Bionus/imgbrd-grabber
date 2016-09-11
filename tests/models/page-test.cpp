#include <QtTest>
#include "page-test.h"
#include "models/tag.h"


void PageTest::init()
{
	m_settings = new QSettings("tests/resources/settings.ini", QSettings::IniFormat);

	m_sites.append(new Site(m_settings, "release/sites/Danbooru (2.0)", "danbooru.donmai.us"));
	m_site = new Site(m_settings, "release/sites/Gelbooru (0.2)", "gelbooru.com");
}

void PageTest::cleanup()
{
	m_settings->deleteLater();

	m_sites.first()->deleteLater();
	m_site->deleteLater();
}


void PageTest::testIncompatibleModifiers()
{
	Page page(m_site, m_sites, QStringList() << "test" << "status:deleted");

	QCOMPARE(page.search().count(), 1);
	QCOMPARE(page.search().first(), QString("test"));
}

void PageTest::testParseUrlBasic()
{
	Page page(m_site, m_sites, QStringList() << "test" << "tag");

	QCOMPARE(page.parseUrl("/index.php?page=post&s=list&pid={pid}&tags={tags}").toString(),
			 QString("http://gelbooru.com/index.php?page=post&s=list&pid=0&tags=test tag"));
}
void PageTest::testParseUrlLogin()
{
	Site *site = m_sites.first();
	site->setUsername("user");
	site->setPassword("pass");

	Page page(site, m_sites, QStringList() << "test" << "tag");

	QCOMPARE(page.parseUrl("/posts.xml?{login}limit={limit}&page={page}{altpage}&tags={tags}").toString(),
			 QString("http://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=1&tags=test tag"));
}
void PageTest::testParseUrlAltPage()
{
	Site *site = m_sites.first();
	Page prevPage(site, m_sites, QStringList() << "test" << "tag", 1000);
	Page page(site, m_sites, QStringList() << "test" << "tag", 1001);
	page.setLastPage(&prevPage);

	QCOMPARE(page.parseUrl("/posts.xml?{login}limit={limit}&page={page}{altpage}&tags={tags}").toString(),
			 QString("http://danbooru.donmai.us/posts.xml?login=user&password_hash=pass&limit=25&page=b0&tags=test tag"));
}


static PageTest instance;
