#include "search-tab.h"
#include "sources/sourceswindow.h"
#include "functions.h"
#include "mainwindow.h"
#include <QFile>
#include <QMouseEvent>

extern mainWindow *_mainwindow;



searchTab::searchTab(int id, QMap<QString,Site*> *sites, QWidget *parent)
	: QWidget(parent), m_id(id), m_sites(sites), m_lastPageMaxId(0), m_lastPageMinId(0)
{ }
searchTab::~searchTab()
{ emit deleted(m_id); }

void searchTab::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::XButton1)
	{ previousPage(); }
	else if (e->button() == Qt::XButton2)
	{ nextPage(); }
}


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


void searchTab::favorite()
{
	Favorite newFav(m_link, 50, QDateTime::currentDateTime());
	m_favorites.append(newFav);

	QFile f(savePath("favorites.txt"));
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(newFav.getName() + "|" + QString::number(newFav.getNote()) + "|" + newFav.getLastViewed().toString(Qt::ISODate) + "\r\n").toUtf8());
	f.close();

	/*QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	if (!QDir(savePath("thumbs")).exists())
	{ QDir(savePath()).mkdir("thumbs"); }
	img.save(savePath("thumbs/"+m_link+".png"), "PNG");*/

	_mainwindow->updateFavorites();
}

void searchTab::unfavorite()
{
	Favorite favorite("", 0, QDateTime::currentDateTime());
	for (Favorite fav : m_favorites)
	{
		if (fav.getName() == m_link)
		{
			favorite = fav;
			m_favorites.removeAll(fav);
			break;
		}
	}
	if (favorite.getName().isEmpty())
		return;

	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();

	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(QRegExp::escape(favorite.getName()) + "\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);

	f.open(QIODevice::WriteOnly);
		f.write(favs.toUtf8());
	f.close();

	if (QFile::exists(savePath("thumbs/" + favorite.getName(true) + ".png")))
	{ QFile::remove(savePath("thumbs/" + favorite.getName(true) + ".png")); }

	_mainwindow->updateFavorites();
}


void searchTab::setSources(QList<bool> sources)
{ m_selectedSources = sources; }

QList<bool> searchTab::sources()
{ return m_selectedSources; }
int searchTab::id()
{ return m_id; }
QStringList searchTab::selectedImages()
{ return m_selectedImages; }

QList<Tag> searchTab::results()
{ return m_tags; }
