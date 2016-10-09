#include <QFile>
#include <QMouseEvent>
#include <QMessageBox>
#include "search-tab.h"
#include "sources/sourceswindow.h"
#include "functions.h"
#include "mainwindow.h"
#include "viewer/zoomwindow.h"
#include "models/filename.h"


searchTab::searchTab(int id, QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent)
	: QWidget(parent), m_profile(profile), m_id(id), m_lastPageMaxId(0), m_lastPageMinId(0), m_sites(sites), m_parent(parent), m_settings(profile->getSettings()), m_pagemax(-1), m_stop(true), m_from_history(false), m_history_cursor(0)
{
	setAttribute(Qt::WA_DeleteOnClose);

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

QColor searchTab::imageColor(Image *img) const
{
	// Blacklisted
	QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
	QStringList detected = img->blacklisted(blacklistedtags);
	if (!detected.isEmpty())
		return QColor("#000000");

	// Favorited (except for exact favorite search)
	for (Tag tag : img->tags())
		if (!img->page()->search().contains(tag.text()))
			for (Favorite fav : m_favorites)
				if (fav.getName() == tag.text())
					return QColor("#ffc0cb");

	// Image with a parent
	if (img->parentId() != 0)
		return QColor("#cccc00");

	// Image with children
	if (img->hasChildren())
		return QColor("#00ff00");

	// Pending image
	if (img->status() == "pending")
		return QColor("#0000ff");

	return QColor();
}

void searchTab::clear()
{
	// Reset loading variables
	m_stop = true;
	m_pagemax = -1;

	// Clear page details
	m_tags.clear();
	m_parent->setTags(m_tags, this);
	m_parent->setWiki("");

	// Clear layout
	for (int i = 0; i < m_layouts.size(); i++)
	{ clearLayout(m_layouts[i]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();
	m_boutons.clear();
	clearLayout(ui_layoutResults);

	// Abort current loadings
	for (int i = 0; i < m_pages.size(); i++)
	{
		m_pages.value(m_pages.keys().at(i))->abort();
		m_pages.value(m_pages.keys().at(i))->abortTags();
	}
	//qDeleteAll(m_pages);
	m_pages.clear();
	for (int i = 0; i < m_images.size(); i++)
	{ m_images.at(i)->abortPreview(); }
	//qDeleteAll(m_images);
	m_images.clear();
}

TextEdit *searchTab::createAutocomplete()
{
	TextEdit *ret = new TextEdit(m_profile, this);
	connect(ret, &TextEdit::returnPressed, this, &searchTab::load);

	// Add auto-complete if necessary
	if (m_settings->value("autocompletion", true).toBool())
	{
		QCompleter *completer = new QCompleter(m_completion, ret);
		completer->setCaseSensitivity(Qt::CaseInsensitive);

		ret->setCompleter(completer);
	}

	return ret;
}

void searchTab::loadImageThumbnails(Page *page, const QList<Image *> &imgs)
{
	QStringList tags = page->search();
	for (int i = 0; i < imgs.count(); i++)
	{
		QStringList detected;
		Image *img = imgs.at(i);
		QList<QChar> modifiers = QList<QChar>() << '~';
		for (int r = 0; r < tags.size(); r++)
		{
			if (modifiers.contains(tags[r][0]))
			{ tags[r] = tags[r].right(tags[r].size()-1); }
		}
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{ detected = img->blacklisted(m_settings->value("blacklistedtags").toString().toLower().split(" ")); }
		if (!detected.isEmpty() && m_settings->value("hideblacklisted", false).toBool())
		{ log(tr("Image #%1 ignorée. Raison : %2.").arg(i).arg("\""+detected.join(", ")+"\""));; }
		else
		{
			connect(img, SIGNAL(finishedLoadingPreview(Image*)), this, SLOT(finishedLoadingPreview(Image*)));
			img->loadPreview();
		}
	}
}

void searchTab::finishedLoadingPreview(Image *img)
{
	if (m_stop)
		return;

	if (img->previewImage().isNull())
	{
		log(tr("<b>Attention :</b> %1").arg(tr("une des miniatures est vide (<a href=\"%1\">%1</a>).").arg(img->previewUrl().toString())));
		return;
	}

	// Download whitelist images on thumbnail view
	QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
	QStringList detected = img->blacklisted(blacklistedtags);
	QStringList whitelistedtags(m_settings->value("whitelistedtags").toString().split(" "));
	QStringList whitelisted = img->blacklisted(whitelistedtags);
	if (!whitelisted.isEmpty() && m_settings->value("whitelist_download", "image").toString() == "page")
	{
		bool download = false;
		if (!detected.isEmpty())
		{
			int reponse = QMessageBox::question(this, tr("Grabber"), tr("Certains tags de l'image sont dans la liste blanche : \"%1\". Cependant, certains dans la liste noire : \"%2\". Voulez-vous la télécharger tout de même ?").arg(whitelisted.join(", "), detected.join(", ")), QMessageBox::Yes | QMessageBox::Open | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{ download = true; }
			else if (reponse == QMessageBox::Open)
			{
				zoomWindow *zoom = new zoomWindow(img, img->page()->site(), m_sites, m_profile, m_parent);
				zoom->show();
				connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
				connect(zoom, SIGNAL(poolClicked(int, QString)), m_parent, SLOT(addPoolTab(int, QString)));
			}
		}
		else
		{ download = true; }

		if (download)
		{
			connect(img, SIGNAL(finishedImage(Image*)), m_parent, SLOT(saveImage(Image*)));
			connect(img, SIGNAL(finishedImage(Image*)), m_parent, SLOT(decreaseDownloads()));

			Filename filename(m_settings->value("Save/filename").toString());
			if (filename.needExactTags(img->page()->site()))
			{
				connect(img, SIGNAL(finishedLoadingTags(Image*)), img, SLOT(loadImage()));
				img->loadDetails();
			}
			else
			{ img->loadImage(); }

			m_parent->increaseDownloads();
		}
	}

	bool merge = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked() && !m_images.empty();
	addResultsImage(img, merge);
}

bool searchTab::waitForMergedResults(bool merged, Page *page, QList<Image*> &imgs)
{
	m_page++;

	if (!merged)
	{
		imgs = page->images();
		return true;
	}

	if (m_page != m_pages.size())
		return false;

	QStringList md5s;
	for (int i = 0; i < m_images.count(); i++)
	{
		QString md5 = m_images.at(i)->md5();
		if (md5.isEmpty())
			continue;

		if (md5s.contains(md5))
			m_images.removeAt(i--);
		else
			md5s.append(md5);
	}

	imgs = m_images;
	return true;
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
		int pageCount = page->pagesCount();
		int imageCount = page->imagesCount();
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
void searchTab::addResultsImage(Image *img, bool merge)
{
	int position = m_images.indexOf(img);
	int page = 0;
	if (!merge)
	{
		page = m_pages.values().indexOf(img->page());
		if (page < 0)
		{ return; }
	}

	float size = img->fileSize();
	QString unit = getUnit(&size);
	QColor color = imageColor(img);

	QBouton *l = new QBouton(position, m_settings->value("resizeInsteadOfCropping", true).toBool(), m_settings->value("borders", 3).toInt(), color, this);
		l->setCheckable(true);
		l->setChecked(m_selectedImages.contains(img->url()));
		l->setToolTip(QString("%1%2%3%4%5%6%7%8")
			.arg(img->tags().isEmpty() ? " " : tr("<b>Tags :</b> %1<br/><br/>").arg(img->stylishedTags(m_profile).join(" ")))
			.arg(img->id() == 0 ? " " : tr("<b>ID :</b> %1<br/>").arg(img->id()))
			.arg(img->rating().isEmpty() ? " " : tr("<b>Classe :</b> %1<br/>").arg(img->rating()))
			.arg(img->hasScore() ? tr("<b>Score :</b> %1<br/>").arg(img->score()) : " ")
			.arg(img->author().isEmpty() ? " " : tr("<b>Posteur :</b> %1<br/><br/>").arg(img->author()))
			.arg(img->width() == 0 || img->height() == 0 ? " " : tr("<b>Dimensions :</b> %1 x %2<br/>").arg(QString::number(img->width()), QString::number(img->height())))
			.arg(img->fileSize() == 0 ? " " : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(size), unit))
			.arg(!img->createdAt().isValid() ? " " : tr("<b>Date :</b> %1").arg(img->createdAt().toString(tr("'le 'dd/MM/yyyy' à 'hh:mm"))))
		);
		l->scale(img->previewImage(), m_settings->value("thumbnailUpscale", 1.0f).toFloat());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(toggled(int, bool, bool)), this, SLOT(toggleImage(int, bool, bool)));
		connect(l, SIGNAL(rightClick(int)), m_parent, SLOT(batchChange(int)));

	int perpage = img->page()->site()->value("Urls/Selected/Tags").contains("{limit}") ? ui_spinImagesPerPage->value() : img->page()->images().size();
	perpage = perpage > 0 ? perpage : 20;
	int pp = perpage;
	if (merge)
	{ pp = m_images.count(); }
	int pl = ceil(sqrt((double)pp));
	if (m_layouts.size() > page)
	{ m_layouts[page]->addWidget(l, floor(float(position % pp) / pl), position % pl); }

	m_boutons.insert(img, l);
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
			int reply = QMessageBox::question(parentWidget(), tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
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
	bool selected = m_selectedImages.contains(img->url());
	m_boutons[img]->setChecked(!selected);

	if (selected)
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

void searchTab::toggleImage(int id, bool toggle, bool range)
{
	if (toggle)
		selectImage(m_images[id]);
	else
		unselectImage(m_images[id]);

	if (range)
	{
		if (id > m_lastToggle)
			for (int i = m_lastToggle + 1; i < id; ++i)
				toggleImage(m_images[i]);
		else
			for (int i = m_lastToggle - 1; i > id; --i)
				toggleImage(m_images[i]);
	}

	m_lastToggle = id;
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
