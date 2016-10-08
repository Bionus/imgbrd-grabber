#include <QFile>
#include <QMouseEvent>
#include <QMessageBox>
#include "search-tab.h"
#include "sources/sourceswindow.h"
#include "functions.h"
#include "mainwindow.h"
#include "viewer/zoomwindow.h"


searchTab::searchTab(int id, QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent)
	: QWidget(parent), m_profile(profile), m_id(id), m_lastPageMaxId(0), m_lastPageMinId(0), m_sites(sites), m_parent(parent), m_settings(parent->settings()), m_from_history(false), m_history_cursor(0)
{
	// Auto-complete list
	QFile words("words.txt");
	if (words.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!words.atEnd())
			m_completion.append(QString(words.readLine()).trimmed().split(" ", QString::SkipEmptyParts));
		words.close();
	}
	QFile wordsc(savePath("wordsc.txt"));
	if (wordsc.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!wordsc.atEnd())
			m_completion.append(QString(wordsc.readLine()).trimmed().split(" ", QString::SkipEmptyParts));
		wordsc.close();
	}

	// Favorite tags
	for (Favorite fav : m_favorites)
		m_completion.append(fav.getName());

	// Modifiers
	for (int i = 0; i < sites->size(); i++)
		if (sites->value(sites->keys().at(i))->contains("Modifiers"))
			m_completion.append(sites->value(sites->keys().at(i))->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts));

	m_completion.removeDuplicates();
	m_completion.sort();
}

searchTab::~searchTab()
{
	emit deleted(m_id);
}


void searchTab::setSelectedSources(QSettings *settings)
{
	QString sel = '1'+QString().fill('0',m_sites->count()-1);
	QString sav = settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selectedSources.append(sav.at(i) == '1' ? true : false);
	}
}

void searchTab::setTagsFromPages(const QMap<QString, Page*> &pages)
{
	// Tags for this page
	QList<Tag> taglist;
	QStringList tagsGot;
	QStringList autocompleteAdd;
	for (int i = 0; i < pages.count(); i++)
	{
		QList<Tag> tags = pages.value(pages.keys().at(i))->tags();
		for (Tag tag : tags)
		{
			if (!tag.text().isEmpty())
			{
				// Add to auto-complete list if it has enough count
				if (tag.count() >= m_settings->value("tagsautoadd", 10).toInt() && !m_completion.contains(tag.text()))
				{
					autocompleteAdd.append(tag.text());
					m_completion.append(tag.text());
				}

				// If we already have this tag in the list, we increase its count
				if (tagsGot.contains(tag.text()))
				{
					int index = tagsGot.indexOf(tag.text());
					taglist[index].setCount(taglist[index].count() + tag.count());
				}
				else
				{
					taglist.append(tag);
					tagsGot.append(tag.text());
				}
			}
		}
	}

	// Add new words to auto-complete list
	QFile wordsc(savePath("wordsc.txt"));
	if (wordsc.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
	{
		wordsc.write(QString('\n').toLatin1());
		wordsc.write(autocompleteAdd.join(' ').toLatin1());
		wordsc.close();
	}

	// We sort tags by frequency
	qSort(taglist.begin(), taglist.end(), sortByFrequency);

	m_tags = taglist;
	m_parent->setTags(m_tags, this);
}

QStringList searchTab::reasonsToFail(Page* page, QStringList completion, QString *meant)
{
	QStringList reasons = QStringList();

	// If the request yieleded no source, the server may be offline
	if (page->source().isEmpty())
	{ reasons.append(tr("serveur hors-ligne")); }

	// Some sources do not allow more than two tags per search
	if (page->search().count() > 2)
	{ reasons.append(tr("trop de tags")); }

	// Many sources don't allow browsing after page 1000
	if (page->page() > 1000)
	{ reasons.append(tr("page trop éloignée")); }

	// Auto-correct
	if (meant != nullptr && !page->search().isEmpty())
	{
		QMap<QString, QString> results, clean;

		int c = 0;
		for (QString tag : page->search())
		{
			int lev = (tag.length()/3)+2;
			for (int w = 0; w < completion.size(); w++)
			{
				int d = levenshtein(tag, completion.at(w));
				if (d < lev)
				{
					if (results[tag].isEmpty())
					{ c++; }
					results[tag] = "<b>"+completion.at(w)+"</b>";
					clean[tag] = completion.at(w);
					lev = d;
				}
			}
			if (lev == 0)
			{
				results[tag] = tag;
				c--;
			}
		}

		if (c > 0)
		{
			QStringList res = results.values(), cl = clean.values();
			*meant = "<a href=\""+cl.join(" ").toHtmlEscaped()+"\" style=\"color:black;text-decoration:none;\">"+res.join(" ")+"</a>";
		}
	}

	return reasons;
}

void searchTab::addResultsPage(Page *page, const QList<Image*> &imgs, QString noResultsMessage)
{
	int pos = m_pages.values().indexOf(page);
	if (pos < 0)
		return;

	QLabel *txt = new QLabel(this);
	if (imgs.count() == 0)
	{
		QString meant;
		QStringList reasons = reasonsToFail(page, m_completion, &meant);
		if (!meant.isEmpty() && ui_widgetMeant != nullptr)
		{
			ui_widgetMeant->show();
			ui_labelMeant->setText(meant);
		}

		QString msg = noResultsMessage == nullptr ? tr("Aucun résultat") : noResultsMessage;
		txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+msg+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : ""));
	}
	else
	{
		int pageCount = page->pagesCount(false);
		int imageCount = page->imagesCount(false);
		txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(page->page()).arg(pageCount > 0 ? QString::number(pageCount) : "?").arg(imgs.count()).arg(imageCount > 0 ? QString::number(imageCount) : "?"));
	}
	txt->setOpenExternalLinks(true);
	/*if (page->search().join(" ") != m_search->toPlainText() && m_settings->value("showtagwarning", true).toBool())
	{
		QStringList uncommon = m_search->toPlainText().toLower().trimmed().split(" ", QString::SkipEmptyParts);
		uncommon.append(m_settings->value("add").toString().toLower().trimmed().split(" ", QString::SkipEmptyParts));
		for (int i = 0; i < page->search().size(); i++)
		{
			if (uncommon.contains(page->search().at(i)))
			{ uncommon.removeAll(page->search().at(i)); }
		}
		if (!uncommon.isEmpty())
		{ txt->setText(txt->text()+"<br/>"+QString(tr("Des modificateurs ont été otés de la recherche car ils ne sont pas compatibles avec cet imageboard : %1.")).arg(uncommon.join(" "))); }
	}*/
	if (!page->errors().isEmpty() && m_settings->value("showwarnings", true).toBool())
	{ txt->setText(txt->text()+"<br/>"+page->errors().join("<br/>")); }

	int page_x = pos % ui_spinColumns->value();
	int page_y = (pos / ui_spinColumns->value()) * 2;
	ui_layoutResults->addWidget(txt, page_y, page_x);
	ui_layoutResults->setRowMinimumHeight(page_y, height() / 20);
	if (m_layouts.size() > pos)
	{ ui_layoutResults->addLayout(m_layouts[pos], page_y + 1, page_x); }
}

void searchTab::addHistory(QString tags, int page, int ipp, int cols)
{
	QMap<QString,QString> srch = QMap<QString,QString>();
	srch["tags"] = tags;
	srch["page"] = QString::number(page);
	srch["ipp"] = QString::number(ipp);
	srch["columns"] = QString::number(cols);
	m_history.append(srch);

	if (m_history.size() > 1)
	{
		m_history_cursor++;
		ui_buttonHistoryBack->setEnabled(true);
		ui_buttonHistoryNext->setEnabled(false);
	}
}
void searchTab::historyBack()
{
	if (m_history_cursor <= 0)
		return;

	m_from_history = true;
	m_history_cursor--;

	ui_spinPage->setValue(m_history[m_history_cursor]["page"].toInt());
	ui_spinImagesPerPage->setValue(m_history[m_history_cursor]["ipp"].toInt());
	ui_spinColumns->setValue(m_history[m_history_cursor]["columns"].toInt());
	setTags(m_history[m_history_cursor]["tags"]);

	ui_buttonHistoryNext->setEnabled(true);
	if (m_history_cursor == 0)
	{ ui_buttonHistoryBack->setEnabled(false); }
}
void searchTab::historyNext()
{
	if (m_history_cursor >= m_history.size() - 1)
		return;

	m_from_history = true;
	m_history_cursor++;

	ui_spinPage->setValue(m_history[m_history_cursor]["page"].toInt());
	ui_spinImagesPerPage->setValue(m_history[m_history_cursor]["ipp"].toInt());
	ui_spinColumns->setValue(m_history[m_history_cursor]["columns"].toInt());
	setTags(m_history[m_history_cursor]["tags"]);

	ui_buttonHistoryBack->setEnabled(true);
	if (m_history_cursor == m_history.size() - 1)
	{ ui_buttonHistoryNext->setEnabled(false); }
}

void searchTab::getSel()
{
	if (m_selectedImagesPtrs.empty())
		return;

	for (Image *img : m_selectedImagesPtrs)
	{
		QStringList tags;
		for (Tag tag : img->tags())
		{ tags.append(tag.typedText()); }

		QMap<QString,QString> values;
		values.insert("id", QString::number(img->id()));
		values.insert("md5", img->md5());
		values.insert("rating", img->rating());
		values.insert("tags", tags.join(" "));
		values.insert("file_url", img->fileUrl().toString());
		values.insert("date", img->createdAt().toString(Qt::ISODate));
		values.insert("site", img->site());
		values.insert("filename", m_settings->value("Save/filename").toString());
		values.insert("folder", m_settings->value("Save/path").toString());

		values.insert("page_url", m_sites->value(img->site())->value("Urls/Html/Post"));
		QString t = m_sites->value(img->site())->contains("DefaultTag") ? m_sites->value(img->site())->value("DefaultTag") : "";
		values["page_url"].replace("{tags}", t);
		values["page_url"].replace("{id}", values["id"]);

		emit batchAddUnique(values);
	}

	m_selectedImagesPtrs.clear();
	m_selectedImages.clear();
	for (QBouton *l : m_boutons)
	{ l->setChecked(false); }
}

void searchTab::updateCheckboxes()
{
	log(tr("Mise à jour des cases à cocher."));

	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();

	QStringList urls = m_sites->keys();
	int n = m_settings->value("Sources/Letters", 3).toInt();
	int m = n;

	for (int i = 0; i < urls.size(); i++)
	{
		QString url = urls[i];
		if (url.startsWith("www."))
		{ url = url.right(url.length() - 4); }
		else if (url.startsWith("chan."))
		{ url = url.right(url.length() - 5); }

		if (n < 0)
		{
			m = url.indexOf('.');
			if (n < -1 && url.indexOf('.', m+1) != -1)
			{ m = url.indexOf('.', m+1); }
		}

		bool isChecked = m_selectedSources.size() > i ? m_selectedSources.at(i) : false;
		QCheckBox *c = new QCheckBox(url.left(m), this);
			c->setChecked(isChecked);
			ui_layoutSourcesList->addWidget(c);

		m_checkboxes.append(c);
	}

	DONE();
}

void searchTab::webZoom(int id)
{
	Image *image = m_images.at(id);

	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
		QStringList detected = image->blacklisted(blacklistedtags);
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(m_parent, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}

	zoomWindow *zoom = new zoomWindow(image, image->page()->site(), m_sites, m_profile, m_parent);
	zoom->show();
	connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
	connect(zoom, SIGNAL(poolClicked(int, QString)), m_parent, SLOT(addPoolTab(int, QString)));
}


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
	m_settings->setValue("sites", sav);
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

	m_parent->updateFavorites();
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

	m_parent->updateFavorites();
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
