#include "downloader.h"
#include <iostream>
#include <QApplication>

Downloader::Downloader()
{}

Downloader::Downloader(QStringList tags, QStringList postfiltering, QStringList sources, int page, int max, int perpage, QString location, QString filename, QString user, QString password, bool blacklist, bool noduplicates)
	: m_tags(tags), m_postfiltering(postfiltering), m_sources(sources), m_page(page), m_max(max), m_perpage(perpage), m_location(location), m_filename(filename), m_user(user), m_password(password), m_blacklist(blacklist), m_noduplicates(noduplicates)
{
	m_quit = false;
	QMap<QString, Site*> *sites = Site::getAllSites();

	m_sites = new QList<Site*>();
	foreach (QString source, sources)
	{
		if (!sites->contains(source))
			std::cerr << "Source '"+source.toStdString()+"' not found" << std::endl;
		else
			m_sites->append(sites->value(source));
	}

	m_pages = new QList<Page*>();
	m_waiting = 0;
}

void Downloader::setQuit(bool quit)
{
	m_quit = quit;
}

void Downloader::getPageCount()
{
	if (m_sites->empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;

	for (int i = 0; i < m_sites->size(); ++i)
	{
		Page *page = new Page(m_sites->at(i), Site::getAllSites(), m_tags, m_page, m_perpage, m_postfiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageCount);

		m_pages->append(page);
		m_waiting++;
	}

	for (int i = 0; i < m_pages->size(); ++i)
	{
		log("Loading tags' '"+m_pages->at(i)->url().toString()+"'");
		m_pages->at(i)->loadTags();
	}
}
void Downloader::finishedLoadingPageCount(Page *page)
{
	log("Received page '"+page->url().toString()+"'");

	m_waiting--;
	if (m_waiting > 0)
		return;

	int total = 0;
	for (int i = 0; i < m_pages->size(); ++i)
		total += m_pages->at(i)->imagesCount();

	if (m_quit)
		returnInt(total);
	else
		emit finishedPageCount(total);
}

void Downloader::getTags()
{
	if (m_sites->empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;

	for (int i = 0; i < m_sites->size(); ++i)
	{
		Page *page = new Page(m_sites->at(i), Site::getAllSites(), m_tags, m_page, m_perpage, m_postfiltering, true, this);
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingTags);

		m_pages->append(page);
		m_waiting++;
	}

	for (int i = 0; i < m_pages->size(); ++i)
	{
		log("Loading tags' '"+m_pages->at(i)->url().toString()+"'");
		m_pages->at(i)->loadTags();
	}
}
void Downloader::finishedLoadingTags(Page *page)
{
	log("Received page '"+page->url().toString()+"'");

	m_waiting--;
	if (m_waiting > 0)
		return;

	QList<Tag> list;
	for (int i = 0; i < m_pages->size(); ++i)
		foreach (Tag tag, m_pages->at(i)->tags())
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

	if (m_quit)
		returnTagList(list);
	else
		emit finishedTags(list);
}

void Downloader::getImages()
{
	if (m_sites->empty())
	{
		std::cerr << "No valid source found" << std::endl;
		return;
	}

	m_waiting = 0;

	for (int i = 0; i < m_sites->size(); ++i)
	{
		int pages = (int)ceil((float)m_max / m_perpage);
		if (pages <= 0 || m_perpage <= 0 || m_max <= 0)
			pages = 1;
		for (int p = 0; p < pages; ++p)
		{
			Page *page = new Page(m_sites->at(i), Site::getAllSites(), m_tags, m_page + p, m_perpage, m_postfiltering, true, this);
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingImages);

			m_pages->append(page);
			m_waiting++;
		}
	}

	for (int i = 0; i < m_pages->size(); ++i)
	{
		log("Loading page '"+m_pages->at(i)->url().toString()+"'");
		m_pages->at(i)->load();
	}
}
void Downloader::finishedLoadingImages(Page *page)
{
	log("Received page '"+page->url().toString()+"'");

	m_waiting--;
	if (m_waiting > 0)
		return;

	QList<Image*> images;
	for (int i = 0; i < m_pages->size(); ++i)
		foreach (Image *img, m_pages->at(i)->images())
		{
			if (m_noduplicates)
			{
				bool found = false;
				foreach (Image *image, images)
					if (image->md5() == img->md5())
						found = true;
				if (found)
					continue;
			}
			images.append(img);
		}

	QStringList urls;
	int i = 0;
	foreach (Image *img, images)
		if (m_max <= 0 || i++ < m_max)
			urls.append(img->url());

	if (m_quit)
		returnStringList(urls);
	else
		emit finishedImages(urls);
}

void Downloader::returnInt(int ret)
{
	std::cout << ret << std::endl;
	qApp->quit();
}
void Downloader::returnString(QString ret)
{
	std::cout << ret.toStdString() << std::endl;
	qApp->quit();
}
void Downloader::returnTagList(QList<Tag> ret)
{
	foreach (Tag tag, ret)
		std::cout << tag.text().toStdString() << "\t" << tag.count() << "\t" << tag.type().toStdString() << std::endl;
	qApp->quit();
}
void Downloader::returnStringList(QStringList ret)
{
	foreach (QString str, ret)
		std::cout << str.toStdString() << std::endl;
	qApp->quit();
}
