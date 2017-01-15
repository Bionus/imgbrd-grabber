#include "downloader.h"
#include <iostream>
#include <QFile>
#include <QDir>
#include <qmath.h>
#include "models/site.h"

Downloader::Downloader()
{}
Downloader::~Downloader()
{
	qDeleteAll(m_pages);
	qDeleteAll(m_pagesC);
	qDeleteAll(m_pagesT);

	qDeleteAll(m_opages);
	qDeleteAll(m_opagesC);
	qDeleteAll(m_opagesT);
}
void Downloader::clear()
{
	m_pages.clear();
	m_pagesC.clear();
	m_pagesT.clear();
	m_opages.clear();
	m_opagesC.clear();
	m_opagesT.clear();
}

Downloader::Downloader(Profile *profile, QStringList tags, QStringList postfiltering, QList<Site*> sources, int page, int max, int perpage, QString location, QString filename, QString user, QString password, bool blacklist, QStringList blacklistedtags, bool noduplicates, int tagsmin, QString tagsformat)
	: m_profile(profile), m_lastPage(nullptr), m_tags(tags), m_postfiltering(postfiltering), m_sites(sources), m_page(page), m_max(max), m_perpage(perpage), m_waiting(0), m_ignored(0), m_duplicates(0), m_tagsmin(tagsmin), m_location(location), m_filename(filename), m_user(user), m_password(password), m_blacklist(blacklist), m_noduplicates(noduplicates), m_tagsformat(tagsformat), m_blacklistedTags(blacklistedtags), m_quit(false)
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

	for (int i = 0; i < m_sites.size(); ++i)
	{
		Page *page = new Page(m_profile, m_sites.at(i), m_sites, m_tags, m_page, m_perpage, m_postfiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageCount);

		m_pagesC.append(page);
		m_opagesC.append(page);
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
	for (int i = 0; i < m_pagesC.size(); ++i)
		total += m_pagesC.at(i)->imagesCount();

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

	for (int i = 0; i < m_sites.size(); ++i)
	{
		Page *page = new Page(m_profile, m_sites.at(i), m_sites, m_tags, m_page, m_perpage, m_postfiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageTags);

		m_pagesT.append(page);
		m_opagesT.append(page);
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
	for (int i = 0; i < m_pagesT.size(); ++i)
		for (Tag tag : m_pagesT.at(i)->tags())
		{
			bool found = false;
			for (int j = 0; j < list.size(); ++j)
				if (list[j].text() == tag.text())
				{
					list[j].setCount(list[j].count() + tag.count());
					found = true;
				}
			if (!found)
				list.append(tag);
		}

	QMutableListIterator<Tag> i(list);
	while (i.hasNext())
		if (i.next().count() < m_tagsmin)
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

	for (int i = 0; i < m_sites.size(); ++i)
	{
		int pages = qCeil((float)m_max / m_perpage);
		if (pages <= 0 || m_perpage <= 0 || m_max <= 0)
			pages = 1;
		Site *site = m_sites.at(i);
		connect(site, &Site::finishedLoadingTags, this, &Downloader::finishedLoadingTags);
		for (int p = 0; p < pages; ++p)
		{
			m_pagesP.append(QPair<Site*, int>(site, m_page + p));
			m_opagesP.append(QPair<Site*, int>(site, m_page + p));
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::loadNext()
{
	if (m_cancelled)
		return;

	if (!m_opagesP.isEmpty())
	{
		QPair<Site*, int> tag = m_opagesP.takeFirst();
		log("Loading tags");
		tag.first->loadTags(tag.second, m_perpage);
		return;
	}

	if (!m_opagesC.isEmpty())
	{
		Page *page = m_opagesC.takeFirst();
		if (m_lastPage != nullptr)
		{ page->setLastPage(m_lastPage); }
		m_lastPage = page;
		log("Loading count '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_opagesT.isEmpty())
	{
		Page *page = m_opagesT.takeFirst();
		if (m_lastPage != nullptr)
		{ page->setLastPage(m_lastPage); }
		m_lastPage = page;
		log("Loading tags '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_opages.isEmpty())
	{
		Page *page = m_opages.takeFirst();
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
		if (i.next().count() < m_tagsmin)
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

	for (int i = 0; i < m_sites.size(); ++i)
	{
		int pages = qCeil((float)m_max / m_perpage);
		if (pages <= 0 || m_perpage <= 0 || m_max <= 0)
			pages = 1;
		for (int p = 0; p < pages; ++p)
		{
			Page *page = new Page(m_profile, m_sites.at(i), m_sites, m_tags, m_page + p, m_perpage, m_postfiltering, true, this);
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingImages);

			m_pages.append(page);
			m_opages.append(page);
			m_waiting++;
		}
	}

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
	for (int i = 0; i < m_pages.size(); ++i)
	{
		Page *p = m_pages.at(i);
		for (QSharedPointer<Image> img : p->images())
		{
			// Blacklisted tags
			if (!m_blacklist)
			{
				if (!img->blacklisted(m_blacklistedTags).empty())
				{
					++m_ignored;
					continue;
				}
			}

			// Skip uplicates
			if (m_noduplicates)
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
	for (QSharedPointer<Image> i : m_imagesDownloading)
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

	for (int i = 0; i < m_sites.size(); ++i)
	{
		int pages = qCeil((float)m_max / m_perpage);
		if (pages <= 0 || m_perpage <= 0 || m_max <= 0)
			pages = 1;
		for (int p = 0; p < pages; ++p)
		{
			Page *page = new Page(m_profile, m_sites.at(i), m_sites, m_tags, m_page + p, m_perpage, m_postfiltering, true, this);
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingUrls);

			m_pages.append(page);
			m_opages.append(page);
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
	QList<QSharedPointer<Image>> images;
	for (int i = 0; i < m_pages.size(); ++i)
	{
		Page *p = m_pages.at(i);
		for (QSharedPointer<Image> img : p->images())
		{
			// Blacklisted tags
			if (!m_blacklist)
			{
				if (!img->blacklisted(m_blacklistedTags).empty())
				{
					++m_ignored;
					continue;
				}
			}

			// Skip uplicates
			if (m_noduplicates)
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
	for (QSharedPointer<Image> img : images)
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
void Downloader::returnTagList(QList<Tag> ret)
{
	for (Tag tag : ret)
	{
		QString ret = m_tagsformat;
		ret.replace("\\t", "\t");
		ret.replace("\\n", "\n");
		ret.replace("\\r", "\r");
		ret.replace("%tag", tag.text());
		ret.replace("%count", QString::number(tag.count()));
		ret.replace("%type", tag.type());
		ret.replace("%stype", QString::number(tag.shortType()));
		std::cout << ret.toStdString() << std::endl;
	}
	emit quit();
}
void Downloader::returnStringList(QStringList ret)
{
	for (QString str : ret)
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
	int pages = qCeil((float)m_max / m_perpage);
	if (pages <= 0 || m_perpage <= 0 || m_max <= 0)
		pages = 1;
	return pages * m_sites.size();
}

QList<Page*> Downloader::getPages() const
{ return m_pages; }
QList<Site*> Downloader::getSites() const
{ return m_sites; }
