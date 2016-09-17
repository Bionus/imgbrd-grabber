#include <QSettings>
#include <QFile>
#include <QNetworkCookie>
#include <iostream>
#include <QSet>
#include "page.h"
#include "json.h"
#include "math.h"
#include "site.h"



Page::Page(Site *site, QList<Site*> sites, QStringList tags, int page, int limit, QStringList postFiltering, bool smart, QObject *parent, int pool, int lastPage, int lastPageMinId, int lastPageMaxId)
	: QObject(parent), m_site(site), m_regexApi(0), m_postFiltering(postFiltering), m_errors(QStringList()), m_imagesPerPage(limit), m_currentSource(0), m_lastPage(lastPage), m_lastPageMinId(lastPageMinId), m_lastPageMaxId(lastPageMaxId), m_smart(smart)
{
	m_website = m_site->url();
	m_imagesCount = -1;
	m_pagesCount = -1;

	// Replace shortcuts to increase compatibility
	QString text = " "+tags.join(" ")+" ";
	text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
		.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
		.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
		.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
		.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
		.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
	tags = text.split(" ", QString::SkipEmptyParts);
	tags.removeDuplicates();

	// Get the list of all enabled modifiers
	QStringList modifiers = QStringList();
	for (Site *site : sites)
	{
		if (site->contains("Modifiers"))
		{ modifiers.append(site->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts)); }
	}
	if (m_site->contains("Modifiers"))
	{
		QStringList mods = m_site->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts);
		for (int j = 0; j < mods.size(); j++)
		{ modifiers.removeAll(mods[j]); }
	}

	// Remove modifiers from tags
	for (int k = 0; k < modifiers.size(); k++)
	{ tags.removeAll(modifiers[k]); }
	m_search = tags;

	// Generate pages
	for (Api *api : m_site->getApis())
	{
		m_pageApis.append(new PageApi(this, m_site, api, m_search, page, limit, postFiltering, smart, parent, pool, lastPage, lastPageMinId, lastPageMaxId));
		if (api->getName() == "Html" && m_regexApi < 0)
		{ m_regexApi = m_pageApis.count() - 1; }
	}
	m_currentApi = -1;

	// Set values
	m_page = page;
	m_pool = pool;
	fallback(false);
}
Page::~Page()
{
	// qDeleteAll(m_images);
}

void Page::fallback(bool bload)
{
	m_currentApi++;

	if (m_currentApi >= m_site->getApis().count())
	{
		log(tr("Aucune source valide du site n'a retourné de résultat."));
		m_errors.append(tr("Aucune source valide du site n'a retourné de résultat."));
		emit failedLoading(this);
		return;
	}
	if (m_currentApi > 0)
	{ log(tr("Chargement en %1 échoué. Nouvel essai en %2.").arg(m_site->getApis().at(m_currentApi - 1)->getName()).arg(m_site->getApis().at(m_currentApi)->getName())); }

	if (bload)
		load();
}

void Page::setLastPage(Page *page)
{
	m_lastPage = page->page();
	m_lastPageMaxId = page->maxId();
	m_lastPageMinId = page->minId();

	m_currentApi--;
	if (!page->nextPage().isEmpty())
	{ /*m_url = page->nextPage();*/ }
	else
	{ fallback(false); }
}

void Page::load(bool rateLimit)
{
	connect(m_pageApis[m_currentApi], &PageApi::finishedLoading, this, &Page::loadFinished);
	m_pageApis[m_currentApi]->load(rateLimit);
}
void Page::loadFinished(PageApi *api, PageApi::LoadResult status)
{
	if (api != m_pageApis[m_currentApi])
		return;

	if (status == PageApi::LoadResult::Ok)
		emit finishedLoading(this);
	else
		fallback();
}
void Page::abort()
{
	m_pageApis[m_currentApi]->abort();
}

void Page::loadTags()
{
	if (m_regexApi < 0)
		return;

	connect(m_pageApis[m_regexApi], &PageApi::finishedLoadingTags, this, &Page::loadTagsFinished);
	m_pageApis[m_regexApi]->loadTags();
}
void Page::loadTagsFinished(PageApi *api)
{
	if (m_regexApi < 0 || api != m_pageApis[m_regexApi])
		return;

	emit finishedLoadingTags(this);
}
void Page::abortTags()
{
	if (m_regexApi < 0)
		return;

	m_pageApis[m_regexApi]->abortTags();
}


void Page::clear()
{
	for (PageApi *pageApi : m_pageApis)
		pageApi->clear();
}

Site			*Page::site()		{ return m_site;								}
QString			Page::website()		{ return m_website;								}
QString			Page::wiki()		{ return m_wiki;								}
QStringList		Page::search()		{ return m_search;								}
QStringList		Page::errors()		{ return m_errors;								}
int				Page::imagesPerPage()	{ return m_imagesPerPage;					}
int				Page::page()		{ return m_page;								}
QList<Image*>	Page::images()		{ return m_pageApis[m_currentApi]->images();	}
QUrl			Page::url()			{ return m_pageApis[m_currentApi]->url();		}
QString			Page::source()		{ return m_pageApis[m_currentApi]->source();	}
QList<Tag>		Page::tags()		{ return m_pageApis[m_currentApi]->tags();		}
QUrl			Page::nextPage()	{ return m_pageApis[m_currentApi]->nextPage();	}
QUrl			Page::prevPage()	{ return m_pageApis[m_currentApi]->prevPage();	}
int				Page::highLimit()	{ return m_pageApis[m_currentApi]->highLimit(); }

int Page::imagesCount(bool guess)
{
	if (m_regexApi >= 0)
	{
		int count = m_pageApis[m_regexApi]->imagesCount(false);
		if (count >= 0)
			return count;
	}
	return m_pageApis[m_currentApi]->imagesCount(guess);
}
int Page::pagesCount(bool guess)
{
	if (m_regexApi >= 0)
	{
		int count = m_pageApis[m_regexApi]->pagesCount(false);
		if (count >= 0)
			return count;
	}
	return m_pageApis[m_currentApi]->pagesCount(guess);
}

int Page::maxId()
{
	return 0;
	/*int maxId = 0;
	for (Image *img : m_images)
		if (img->id() > maxId || maxId == 0)
			maxId = img->id();
	return maxId;*/
}
int Page::minId()
{
	return 0;
	/*int minId = 0;
	for (Image *img : m_images)
		if (img->id() < minId || minId == 0)
			minId = img->id();
	return minId;*/
}
