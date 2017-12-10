#include "downloader/downloader.h"
#include <QDir>
#include <QFile>
#include <qmath.h>
#include <iostream>
#include "logger.h"
#include "models/image.h"
#include "models/page.h"
#include "models/post-filter.h"
#include "models/site.h"


Downloader::~Downloader()
{
	qDeleteAll(m_pages);
	qDeleteAll(m_pagesC);
	qDeleteAll(m_pagesT);

	qDeleteAll(m_oPages);
	qDeleteAll(m_oPagesC);
	qDeleteAll(m_oPagesT);
}
void Downloader::clear()
{
	m_pages.clear();
	m_pagesC.clear();
	m_pagesT.clear();
	m_oPages.clear();
	m_oPagesC.clear();
	m_oPagesT.clear();
}

Downloader::Downloader(Profile *profile, QStringList tags, QStringList postFiltering, QList<Site*> sources, int page, int max, int perPage, QString location, QString filename, QString user, QString password, bool blacklist, QStringList blacklistedTags, bool noDuplicates, int tagsMin, QString tagsFormat, Downloader *previous)
	: m_profile(profile), m_lastPage(nullptr), m_tags(tags), m_postFiltering(postFiltering), m_sites(sources), m_page(page), m_max(max), m_perPage(perPage), m_waiting(0), m_ignored(0), m_duplicates(0), m_tagsMin(tagsMin), m_location(location), m_filename(filename), m_user(user), m_password(password), m_blacklist(blacklist), m_noDuplicates(noDuplicates), m_tagsFormat(tagsFormat), m_blacklistedTags(blacklistedTags), m_quit(false), m_previous(previous), m_cancelled(false)
{ }

void Downloader::setQuit(bool quit)
{
	m_quit = quit;
}

void Downloader::getPageCount()
{
	if (m_sites.empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;
	m_ignored = 0;
	m_duplicates = 0;
	m_cancelled = false;

	for (Site *site : m_sites)
	{
		Page *page = new Page(m_profile, site, m_sites, m_tags, m_page, m_perPage, m_postFiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageCount);

		m_pagesC.append(page);
		m_oPagesC.append(page);
		m_waiting++;
	}

	loadNext();
}
void Downloader::finishedLoadingPageCount(Page *page)
{
	if (m_cancelled)
		return;

	log(QString("Received page count '%1' (%2)").arg(page->url().toString(), QString::number(page->images().count())));

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	int total = 0;
	for (Page *p : m_pagesC)
		total += p->imagesCount();

	if (m_quit)
		returnInt(total);
	else
		emit finishedPageCount(total);
}

void Downloader::getPageTags()
{
	if (m_sites.empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;
	m_cancelled = false;

	for (Site *site : m_sites)
	{
		Page *page = new Page(m_profile, site, m_sites, m_tags, m_page, m_perPage, m_postFiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageTags);

		m_pagesT.append(page);
		m_oPagesT.append(page);
		m_waiting++;
	}

	loadNext();
}
void Downloader::finishedLoadingPageTags(Page *page)
{
	if (m_cancelled)
		return;

	log(QString("Received tags '%1' (%2)").arg(page->url().toString(), QString::number(page->tags().count())));

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QList<Tag> list;
	for (auto p : m_pagesT)
	{
		for (const Tag &tag : p->tags())
		{
			bool found = false;
			for (auto &t : list)
			{
				if (t.text() == tag.text())
				{
					t.setCount(t.count() + tag.count());
					found = true;
				}
			}
			if (!found)
				list.append(tag);
		}
	}

	QMutableListIterator<Tag> i(list);
	while (i.hasNext())
		if (i.next().count() < m_tagsMin)
			i.remove();

	if (m_quit)
		returnTagList(list);
	else
		emit finishedTags(list);
}

void Downloader::getTags()
{
	if (m_sites.empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;
	m_cancelled = false;

	for (Site *site : m_sites)
	{
		int pages = qCeil(static_cast<float>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0)
			pages = 1;
		connect(site, &Site::finishedLoadingTags, this, &Downloader::finishedLoadingTags);
		for (int p = 0; p < pages; ++p)
		{
			m_pagesP.append(QPair<Site*, int>(site, m_page + p));
			m_oPagesP.append(QPair<Site*, int>(site, m_page + p));
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::loadNext()
{
	if (m_cancelled)
		return;

	if (!m_oPagesP.isEmpty())
	{
		QPair<Site*, int> tag = m_oPagesP.takeFirst();
		log("Loading tags");
		tag.first->loadTags(tag.second, m_perPage);
		return;
	}

	if (!m_oPagesC.isEmpty())
	{
		Page *page = m_oPagesC.takeFirst();
		if (m_lastPage != nullptr)
		{ page->setLastPage(m_lastPage); }
		m_lastPage = page;
		log("Loading count '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_oPagesT.isEmpty())
	{
		Page *page = m_oPagesT.takeFirst();
		if (m_lastPage != nullptr)
		{ page->setLastPage(m_lastPage); }
		m_lastPage = page;
		log("Loading tags '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_oPages.isEmpty())
	{
		Page *page = m_oPages.takeFirst();
		if (m_lastPage != nullptr)
		{ page->setLastPage(m_lastPage); }
		m_lastPage = page;
		log("Loading images '"+page->url().toString()+"'");
		page->load();
		return;
	}

	if (!m_images.isEmpty())
	{
		QSharedPointer<Image> image = m_images.takeFirst();
		m_imagesDownloading.append(image);
		log("Loading image '"+image->url()+"'");
		connect(image.data(), &Image::finishedImage, this, &Downloader::finishedLoadingImage);
		image->loadImage();
		return;
	}
}
void Downloader::finishedLoadingTags(QList<Tag> tags)
{
	if (m_cancelled)
		return;

	log(QString("Received pure tags (%1)").arg(tags.count()));

	m_results.append(tags);
	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QMutableListIterator<Tag> i(m_results);
	while (i.hasNext())
		if (i.next().count() < m_tagsMin)
			i.remove();

	if (m_quit)
		returnTagList(m_results);
	else
		emit finishedTags(m_results);
}

void Downloader::getImages()
{
	if (m_sites.empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;
	m_cancelled = false;

	for (Site *site : m_sites)
	{
		int pages = qCeil(static_cast<float>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0)
			pages = 1;
		for (int p = 0; p < pages; ++p)
		{
			Page *page = new Page(m_profile, site, m_sites, m_tags, m_page + p, m_perPage, m_postFiltering, true, this);
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingImages);

			m_pages.append(page);
			m_oPages.append(page);
			m_waiting++;
		}
	}

	if (m_previous != nullptr)
		m_lastPage = m_previous->lastPage();

	loadNext();
}
void Downloader::finishedLoadingImages(Page *page)
{
	if (m_cancelled)
		return;

	log(QString("Received image page '%1' (%2)").arg(page->url().toString(), QString::number(page->images().count())));
	emit finishedImagesPage(page);

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QSet<QString> md5s;
	QList<QSharedPointer<Image>> images;
	for (Page *p : m_pages)
	{
		for (const QSharedPointer<Image> &img : p->images())
		{
			// Blacklisted tags
			if (!m_blacklist)
			{
				if (!PostFilter::blacklisted(img->tokens(m_profile), m_blacklistedTags).empty())
				{
					++m_ignored;
					continue;
				}
			}

			// Skip duplicates
			if (m_noDuplicates)
			{
				if (md5s.contains(img->md5()))
					continue;
				md5s.insert(img->md5());
			}

			images.append(img);
			if (images.count() == m_max)
				break;
		}

		if (images.count() == m_max)
			break;
	}

	if (m_quit)
		downloadImages(images);
	else
		emit finishedImages(images);
}

void Downloader::downloadImages(QList<QSharedPointer<Image>> images)
{
	m_images.clear();
	m_images.append(images);
	m_waiting = images.size();

	loadNext();
}
void Downloader::finishedLoadingImage()
{
	if (m_cancelled)
		return;

	QSharedPointer<Image> image;
	for (const QSharedPointer<Image> &i : m_imagesDownloading)
		if (i.data() == sender())
			image = i;
	if (image.isNull())
		return;
	m_imagesDownloading.removeAll(image);

	log(QString("Received image '%1'").arg(image->url()));

	if (m_quit)
	{
		QStringList paths = image->path(m_filename, m_location);
		for (QString path : paths)
		{
			path = (m_location.endsWith('/') ? m_location.left(m_location.length() - 1) : m_location) + "/" + (path.startsWith('/') ? path.right(path.length() - 1) : path);
			QFile f(QDir::toNativeSeparators(path));
			if (f.open(QFile::WriteOnly))
			{
				f.write(image->data());
				f.close();
				log(QString("Saved to '%1'").arg(path));
			}
		}
	}
	else
		emit finishedImage(image);

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	if (m_quit)
		returnString("Downloaded images successfully.");
}

void Downloader::getUrls()
{
	if (m_sites.empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;
	m_ignored = 0;
	m_duplicates = 0;
	m_cancelled = false;

	for (Site *site : m_sites)
	{
		int pages = qCeil(static_cast<float>(m_max) / m_perPage);
		if (pages <= 0 || m_perPage <= 0 || m_max <= 0)
			pages = 1;
		for (int p = 0; p < pages; ++p)
		{
			Page *page = new Page(m_profile, site, m_sites, m_tags, m_page + p, m_perPage, m_postFiltering, true, this);
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingUrls);

			m_pages.append(page);
			m_oPages.append(page);
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::finishedLoadingUrls(Page *page)
{
	if (m_cancelled)
		return;

	log(QString("Received url page '%1' (%2)").arg(page->url().toString(), QString::number(page->images().count())));
	emit finishedUrlsPage(page);

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QSet<QString> md5s;
	QVector<QSharedPointer<Image>> images;
	for (Page *p : m_pages)
	{
		for (const QSharedPointer<Image> &img : p->images())
		{
			// Blacklisted tags
			if (!m_blacklist)
			{
				if (!PostFilter::blacklisted(img->tokens(m_profile), m_blacklistedTags).empty())
				{
					++m_ignored;
					continue;
				}
			}

			// Skip duplicates
			if (m_noDuplicates)
			{
				if (md5s.contains(img->md5()))
					continue;
				md5s.insert(img->md5());
			}

			images.append(img);
			if (images.count() == m_max)
				break;
		}

		if (images.count() == m_max)
			break;
	}

	QStringList urls;
	int i = 0;
	for (const QSharedPointer<Image> &img : images)
		if (m_max <= 0 || i++ < m_max)
			urls.append(img->url());

	if (m_quit)
		returnStringList(urls);
	else
		emit finishedUrls(urls);
}

void Downloader::returnInt(int ret)
{
	std::cout << ret << std::endl;
	emit quit();
}
void Downloader::returnString(QString ret)
{
	std::cout << ret.toStdString() << std::endl;
	emit quit();
}
void Downloader::returnTagList(QList<Tag> tags)
{
	for (const Tag &tag : tags)
	{
		QString ret = m_tagsFormat;
		ret.replace("\\t", "\t");
		ret.replace("\\n", "\n");
		ret.replace("\\r", "\r");
		ret.replace("%tag", tag.text());
		ret.replace("%count", QString::number(tag.count()));
		ret.replace("%type", tag.type().name());
		ret.replace("%stype", QString::number(tag.type().number()));
		std::cout << ret.toStdString() << std::endl;
	}
	emit quit();
}
void Downloader::returnStringList(QStringList ret)
{
	for (const QString &str : ret)
		std::cout << str.toStdString() << std::endl;
	emit quit();
}

void Downloader::setData(QVariant data)
{ m_data = data; }
QVariant Downloader::getData() const
{ return m_data; }

void Downloader::cancel()
{ m_cancelled = true; }

int Downloader::ignoredCount() const
{ return m_ignored; }
int Downloader::duplicatesCount() const
{ return m_duplicates; }
int Downloader::pagesCount() const
{
	int pages = qCeil(static_cast<float>(m_max) / m_perPage);
	if (pages <= 0 || m_perPage <= 0 || m_max <= 0)
		pages = 1;
	return pages * m_sites.size();
}
int Downloader::imagesMax() const
{ return m_max; }
Page *Downloader::lastPage() const
{ return m_lastPage; }

QList<Page*> Downloader::getPages() const
{ return m_pages; }
QList<Site*> Downloader::getSites() const
{ return m_sites; }
