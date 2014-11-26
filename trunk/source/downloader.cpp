#include "downloader.h"
#include <iostream>
#include <QApplication>
#include <QFile>
#include <QDir>

Downloader::Downloader()
{}

Downloader::Downloader(QStringList tags, QStringList postfiltering, QStringList sources, int page, int max, int perpage, QString location, QString filename, QString user, QString password, bool blacklist, bool noduplicates, int tagsmin, QString tagsformat)
	: m_tags(tags), m_postfiltering(postfiltering), m_sources(sources), m_page(page), m_max(max), m_perpage(perpage), m_location(location), m_filename(filename), m_user(user), m_password(password), m_blacklist(blacklist), m_noduplicates(noduplicates), m_tagsmin(tagsmin), m_tagsformat(tagsformat)
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
    m_pagesC = new QList<Page*>();
	m_pagesT = new QList<Page*>();
	m_pagesP = new QList<QPair<Site*, int>>();
	m_opages = new QList<Page*>();
	m_opagesC = new QList<Page*>();
	m_opagesT = new QList<Page*>();
	m_opagesP = new QList<QPair<Site*, int>>();
	m_waiting = 0;
	m_results = new QList<Tag>();
	m_images = new QList<Image*>();
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

		m_pagesC->append(page);
		m_opagesC->append(page);
		m_waiting++;
	}

	loadNext();
}
void Downloader::finishedLoadingPageCount(Page *page)
{
	log("Received page '"+page->url().toString()+"'");

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	int total = 0;
	for (int i = 0; i < m_pagesC->size(); ++i)
		total += m_pagesC->at(i)->imagesCount();

	if (m_quit)
		returnInt(total);
	else
		emit finishedPageCount(total);
}

void Downloader::getPageTags()
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
		connect(page, &Page::finishedLoadingTags, this, &Downloader::finishedLoadingPageTags);

		m_pagesT->append(page);
		m_opagesT->append(page);
		m_waiting++;
	}

	loadNext();
}
void Downloader::finishedLoadingPageTags(Page *page)
{
	log("Received tags '"+page->url().toString()+"'");

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QList<Tag> list;
	for (int i = 0; i < m_pagesT->size(); ++i)
		foreach (Tag tag, m_pagesT->at(i)->tags())
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
		Site *site = m_sites->at(i);
		connect(site, &Site::finishedLoadingTags, this, &Downloader::finishedLoadingTags);
		for (int p = 0; p < pages; ++p)
		{
			m_pagesP->append(QPair<Site*, int>(site, m_page + p));
			m_opagesP->append(QPair<Site*, int>(site, m_page + p));
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::loadNext()
{
	if (!m_opagesP->isEmpty())
	{
		QPair<Site*, int> tag = m_opagesP->takeFirst();
		log("Loading tags");
		tag.first->loadTags(tag.second, m_perpage);
		return;
	}

	if (!m_opagesC->isEmpty())
	{
		Page *page = m_opagesC->takeFirst();
		log("Loading count '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_opagesT->isEmpty())
	{
		Page *page = m_opagesT->takeFirst();
		log("Loading tags '"+page->url().toString()+"'");
		page->loadTags();
		return;
	}

	if (!m_opages->isEmpty())
	{
		Page *page = m_opages->takeFirst();
		log("Loading images '"+page->url().toString()+"'");
		page->load();
		return;
	}

	if (!m_images->isEmpty())
	{
		Image *image = m_images->takeFirst();
		log("Loading image '"+image->url()+"'");
		connect(image, &Image::finishedImage, this, &Downloader::finishedLoadingImage);
		image->loadImage();
		return;
	}
}
void Downloader::finishedLoadingTags(QList<Tag> tags)
{
	log("Received pure tags");

	m_results->append(tags);
	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

	QMutableListIterator<Tag> i(*m_results);
	while (i.hasNext())
		if (i.next().count() < m_tagsmin)
			i.remove();

	if (m_quit)
		returnTagList(*m_results);
	else
		emit finishedTags(*m_results);
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
			m_opages->append(page);
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::finishedLoadingImages(Page *page)
{
	log("Received page '"+page->url().toString()+"'");
    emit finishedImagesPage(page);

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

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

	QList<Image*> imgs;
	int i = 0;
	foreach (Image *img, images)
		if (m_max <= 0 || i++ < m_max)
			imgs.append(img);

	if (m_quit)
		downloadImages(imgs);
	else
		emit finishedImages(imgs);
}

void Downloader::downloadImages(QList<Image*> images)
{
	m_images->clear();
	m_images->append(images);
	m_waiting = images.size();

	loadNext();
}
void Downloader::finishedLoadingImage(Image *image)
{
	log("Received image '"+image->url()+"'");

	if (m_quit)
	{
		QString path = image->path(m_filename, m_location);
		path = (m_location.endsWith('/') ? m_location.left(m_location.length() - 1) : m_location) + "/" + (path.startsWith('/') ? path.right(path.length() - 1) : path);
		QFile f(QDir::toNativeSeparators(path));
		if (f.open(QFile::WriteOnly))
		{
			f.write(image->data());
			f.close();
			log("Saved to '"+path+"'");
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
			connect(page, &Page::finishedLoading, this, &Downloader::finishedLoadingUrls);

			m_pages->append(page);
			m_opages->append(page);
			m_waiting++;
		}
	}

	loadNext();
}
void Downloader::finishedLoadingUrls(Page *page)
{
	log("Received page '"+page->url().toString()+"'");
    emit finishedUrlsPage(page);

	if (--m_waiting > 0)
	{
		loadNext();
		return;
	}

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
		emit finishedUrls(urls);
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
	qApp->quit();
}
void Downloader::returnStringList(QStringList ret)
{
	foreach (QString str, ret)
		std::cout << str.toStdString() << std::endl;
	qApp->quit();
}

void Downloader::setData(QVariant data)
{
    m_data = data;
}
QVariant Downloader::getData()
{
    return m_data;
}
