#include "searchtab.h"
#include "sourceswindow.h"
#include "functions.h"



searchTab::searchTab(int id, QMap<QString,Site*> *sites, QWidget *parent) : QWidget(parent), m_id(id), m_sites(sites)
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



void searchTab::openSourcesWindow()
{
	sourcesWindow *adv = new sourcesWindow(m_selectedSources, m_sites, this);
	connect(adv, SIGNAL(valid(QList<bool>)), this, SLOT(saveSources(QList<bool>)));
	adv->show();
}
void searchTab::saveSources(QList<bool> sel)
{
	log(tr("Sauvegarde des sources..."));
	m_selectedSources = sel;
	QString sav;
	for (int i = 0; i < m_selectedSources.count(); i++)
	{ sav += (m_selectedSources.at(i) ? "1" : "0"); }
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	settings.setValue("sites", sav);
	DONE();
	updateCheckboxes();
}

void searchTab::setSources(QList<bool> sources)
{ m_selectedSources = sources; }

QList<bool> searchTab::sources()
{ return m_selectedSources; }
int searchTab::id()
{ return m_id; }
QStringList searchTab::selectedImages()
{ return m_selectedImages; }
