#include "searchtab.h"



searchTab::searchTab(int id, QWidget *parent) : QWidget(parent), m_id(id)
{ }
searchTab::~searchTab()
{ emit deleted(m_id); }


void searchTab::selectImage(Image *img)
{
	if (!m_selectedImages.contains(img->url()))
	{
		m_selectedImagesPtrs.append(img);
		m_selectedImages.append(img->url());
	}
}
void searchTab::unselectImage(Image *img)
{
	if (m_selectedImages.contains(img->url()))
	{
		int pos = m_selectedImages.indexOf(img->url());
		m_selectedImagesPtrs.removeAt(pos);
		m_selectedImages.removeAt(pos);
	}
}
void searchTab::toggleImage(Image *img)
{
	if (m_selectedImages.contains(img->url()))
	{
		int pos = m_selectedImages.indexOf(img->url());
		m_selectedImagesPtrs.removeAt(pos);
		m_selectedImages.removeAt(pos);
	}
	else
	{
		m_selectedImagesPtrs.append(img);
		m_selectedImages.append(img->url());
	}
}

void searchTab::setSources(QList<bool> sources)
{ m_selectedSources = sources; }

QList<bool> searchTab::sources()
{ return m_selectedSources; }
int searchTab::id()
{ return m_id; }
QStringList searchTab::selectedImages()
{ return m_selectedImages; }
