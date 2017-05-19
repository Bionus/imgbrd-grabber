#include "search-tab.h"
#include <QMouseEvent>
#include <QMessageBox>
#include <QSet>
#include <QMenu>
#include "ui/textedit.h"
#include "ui/QBouton.h"
#include "ui/verticalscrollarea.h"
#include "ui/fixed-size-grid-layout.h"
#include "downloader/download-query-image.h"
#include "models/favorite.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/filename.h"
#include "sources/sourceswindow.h"
#include "viewer/zoomwindow.h"
#include "mainwindow.h"
#include "helpers.h"
#include "functions.h"

#define FIXED_IMAGE_WIDTH 150


searchTab::searchTab(QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent)
	: QWidget(parent), m_profile(profile), m_lastPageMaxId(0), m_lastPageMinId(0), m_sites(sites), m_favorites(profile->getFavorites()), m_parent(parent), m_settings(profile->getSettings()), m_pagemax(-1), m_stop(true), m_from_history(false), m_history_cursor(0), m_lastTags(QString())
{
	setAttribute(Qt::WA_DeleteOnClose);

	// Auto-complete list
	m_completion.append(profile->getAutoComplete());
	m_completion.append(profile->getCustomAutoComplete());

	// Favorite tags
	for (Favorite fav : m_favorites)
		m_completion.append(fav.getName());

	// Modifiers
	for (int i = 0; i < sites->size(); i++)
		if (sites->value(sites->keys().at(i))->contains("Modifiers"))
			m_completion.append(sites->value(sites->keys().at(i))->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts));

	m_completion.removeDuplicates();
	m_completion.sort();

	setSelectedSources(m_settings);
}

searchTab::~searchTab()
{
	qDeleteAll(m_pages);
	m_pages.clear();
	m_images.clear();
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();
	for (Site *site : m_layouts.keys())
	{ clearLayout(m_layouts[site]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();
	m_boutons.clear();
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

void searchTab::optionsChanged()
{
	log(QString("Updating settings for tab \"%1\".").arg(windowTitle()));
	// ui->retranslateUi(this);

	ui_spinImagesPerPage->setValue(m_settings->value("limit", 20).toInt());
	ui_spinColumns->setValue(m_settings->value("columns", 1).toInt());

	/*QPalette p = ui->widgetResults->palette();
	p.setColor(ui->widgetResults->backgroundRole(), QColor(m_settings->value("serverBorderColor", "#000000").toString()));
	ui->widgetResults->setPalette(p);*/
	ui_layoutResults->setHorizontalSpacing(m_settings->value("Margins/main", 10).toInt());
}

void searchTab::setTagsFromPages(const QMap<QString, Page*> &pages)
{
	// Tags for this page
	QList<Tag> taglist;
	QStringList tagsGot;
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
					m_profile->addAutoComplete(tag.text());
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
	{ reasons.append(tr("server offline")); }

	// Some sources do not allow more than two tags per search
	if (page->search().count() > 2)
	{ reasons.append(tr("too many tags")); }

	// Many sources don't allow browsing after page 1000
	if (page->page() > 1000)
	{ reasons.append(tr("page too far")); }

	// Auto-correct
	if (meant != nullptr && !page->search().isEmpty())
	{
		QMap<QString, QString> results, clean;
		QList<QChar> modifiers = QList<QChar>() << '~' << '-';

		int c = 0;
		for (QString tag : page->search())
		{
			if (modifiers.contains(tag[0]))
				tag = tag.mid(1);

			int lev = qCeil((tag.length() - 1) / 4.0f);
			for (QString comp : completion)
			{
				// Ignore tags that are too long
				if (abs(comp.length() - tag.length()) > lev)
					continue;

				int d = levenshtein(tag, comp);
				if (d < lev)
				{
					if (results[tag].isEmpty())
					{ c++; }
					results[tag] = "<b>"+comp+"</b>";
					clean[tag] = comp;
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

QColor searchTab::imageColor(QSharedPointer<Image> img) const
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
	for (int i = 0; i < ui_layoutResults->rowCount(); ++i)
	{ ui_layoutResults->setRowMinimumHeight(i, 0); }
	for (Site *site : m_layouts.keys())
	{ clearLayout(m_layouts[site]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();
	m_boutons.clear();
	m_pageLabels.clear();
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

void searchTab::finishedLoading(Page* page)
{
	if (m_stop)
		return;

	log(QString("Receiving page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));

	m_lastPage = page->page();
	m_lastPageMinId = page->minId();
	m_lastPageMaxId = page->maxId();

	// Filter images depending on tabs
	QList<QSharedPointer<Image>> validImages;
	for (QSharedPointer<Image> img : page->images())
		if (validateImage(img))
			validImages.append(img);

	// Remove already existing images for merged results
	bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	QList<QSharedPointer<Image>> imgs = merged ? mergeResults(page->page(), validImages) : validImages;

	m_images.append(imgs);

	int maxpage = page->pagesCount();
	if (maxpage < m_pagemax || m_pagemax == -1)
		m_pagemax = maxpage;
	ui_buttonNextPage->setEnabled(maxpage > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1 || (page->imagesCount() == 0 && page->images().count() > 0));
	ui_buttonLastPage->setEnabled(maxpage > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1);

	if (!merged)
		addResultsPage(page, imgs);

	if (!m_settings->value("useregexfortags", true).toBool())
		setTagsFromPages(m_pages);

	postLoading(page, imgs);
}

void searchTab::failedLoading(Page *page)
{
	if (ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked())
		postLoading(page, page->images());
}

void searchTab::postLoading(Page *page, QList<QSharedPointer<Image>> imgs)
{
	m_page++;

	bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	if (merged)
	{
		// Increase the progress bar status
		if (ui_progressMergeResults != nullptr)
			ui_progressMergeResults->setValue(ui_progressMergeResults->value() + 1);

		// Hide progress bar when we load the last page
		bool finished = m_page == m_pages.count() || (ui_progressMergeResults != nullptr && ui_progressMergeResults->value() == ui_progressMergeResults->maximum());
		if (ui_stackedMergeResults != nullptr && finished)
			ui_stackedMergeResults->setCurrentIndex(1);

		// Create the label when loading the first page
		if (m_page == 1)
		{
			QLabel *txt = new QLabel(this);
			txt->setOpenExternalLinks(true);
			setMergedLabelText(txt, m_images);
			m_pageLabels.insert(nullptr, txt);

			ui_layoutResults->addWidget(txt, 0, 0);
			ui_layoutResults->setRowMinimumHeight(0, txt->sizeHint().height() + 10);
		}
	}

	loadImageThumbnails(page, imgs);

	ui_buttonGetAll->setDisabled(m_images.empty());
	ui_buttonGetPage->setDisabled(m_images.empty());
	ui_buttonGetSel->setDisabled(m_images.empty());
}

void searchTab::finishedLoadingTags(Page *page)
{
	setTagsFromPages(m_pages);

	// Wiki
	if (!page->wiki().isEmpty())
	{
		m_wiki = "<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+page->wiki();
		m_parent->setWiki(m_wiki);
	}

	int maxpage = page->pagesCount();
	if (maxpage < m_pagemax || m_pagemax == -1)
		m_pagemax = maxpage;
	ui_buttonNextPage->setEnabled(maxpage > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1 || (page->imagesCount() == 0 && page->images().count() > 0));
	ui_buttonLastPage->setEnabled(maxpage > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1);

	// Update image and page count
	QList<QSharedPointer<Image>> imgs;
	for (QSharedPointer<Image> img : page->images())
		if (validateImage(img))
			imgs.append(img);

	if (ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked() && m_pageLabels.contains(nullptr))
		setMergedLabelText(m_pageLabels[nullptr], m_images);
	else if (m_pageLabels.contains(page))
		setPageLabelText(m_pageLabels[page], page, imgs);
}

void searchTab::loadImageThumbnails(Page *page, const QList<QSharedPointer<Image>> &imgs)
{
	QStringList tags = page->search();
	for (int i = 0; i < imgs.count(); i++)
	{
		QStringList detected;
		QSharedPointer<Image> img = imgs.at(i);
		QList<QChar> modifiers = QList<QChar>() << '~' << '-';
		for (int r = 0; r < tags.size(); r++)
			if (modifiers.contains(tags[r][0]))
				tags[r] = tags[r].mid(1);

		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{ detected = img->blacklisted(m_settings->value("blacklistedtags").toString().toLower().split(" ")); }
		if (!detected.isEmpty() && m_settings->value("hideblacklisted", false).toBool())
		{ log(QString("Image #%1 ignored. Reason: %2.").arg(i).arg("\""+detected.join(", ")+"\""));; }
		else
		{
			connect(img.data(), &Image::finishedLoadingPreview, this, &searchTab::finishedLoadingPreview);
			img->loadPreview();
		}
	}
}

void searchTab::finishedLoadingPreview()
{
	if (m_stop)
		return;

	QSharedPointer<Image> img;
	for (QSharedPointer<Image> i : m_images)
		if (i.data() == sender())
			img = i;
	if (img.isNull())
		return;

	if (img->previewImage().isNull())
	{
		log(QString("<b>Warning:</b> %1").arg(tr("one of the thumbnails is empty (<a href=\"%1\">%1</a>).").arg(img->previewUrl().toString())));
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
			int reponse = QMessageBox::question(this, "Grabber", tr("Some tags from the image are in the whitelist: %1. However, some tags are in the blacklist: %2. Do you want to download it anyway?").arg(whitelisted.join(", "), detected.join(", ")), QMessageBox::Yes | QMessageBox::Open | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{ download = true; }
			else if (reponse == QMessageBox::Open)
			{
				zoomWindow *zoom = new zoomWindow(m_images, img, img->page()->site(), m_sites, m_profile, m_parent);
				zoom->show();
				connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
				connect(zoom, SIGNAL(poolClicked(int, QString)), m_parent, SLOT(addPoolTab(int, QString)));
			}
		}
		else
		{ download = true; }

		if (download)
		{ img->loadAndSave(m_settings->value("Save/filename").toString(), m_settings->value("Save/path").toString()); }
	}

	bool merge = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked() && !m_images.empty();
	addResultsImage(img, merge);
}

QList<QSharedPointer<Image>> searchTab::mergeResults(int page, QList<QSharedPointer<Image>> results)
{
	QSet<QString> pageMd5s;
	for (QSharedPointer<Image> img : m_images)
	{
		QString md5 = img->md5();
		if (md5.isEmpty())
			continue;

		pageMd5s.insert(md5);
		addMergedMd5(page, md5);
	}

	QList<QSharedPointer<Image>> ret;
	for (QSharedPointer<Image> img : results)
	{
		QString md5 = img->md5();
		if (md5.isEmpty() || (!pageMd5s.contains(md5) && !containsMergedMd5(page, md5)))
		{
			ret.append(img);

			if (!md5.isEmpty())
			{
				pageMd5s.insert(md5);
				addMergedMd5(page, md5);
			}
		}
	}

	return ret;
}

void searchTab::addMergedMd5(int page, QString md5)
{
	for (QPair<int, QSet<QString>> &pair : m_mergedMd5s)
	{
		if (pair.first == page)
		{
			pair.second.insert(md5);
			return;
		}
	}

	QSet<QString> set;
	set.insert(md5);
	m_mergedMd5s.append(QPair<int, QSet<QString>>(page, set));
}

bool searchTab::containsMergedMd5(int page, QString md5)
{
	for (const QPair<int, QSet<QString>> &pair : m_mergedMd5s)
	{
		// We only check the sets before the page was loaded
		if (pair.first == page)
			break;

		if (pair.second.contains(md5))
			return true;
	}

	return false;
}

void searchTab::addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage)
{
	int pos = m_pages.values().indexOf(page);
	if (pos < 0)
		return;

	QLabel *txt = new QLabel(this);
	txt->setOpenExternalLinks(true);
	setPageLabelText(txt, page, imgs, noResultsMessage);
	m_pageLabels.insert(page, txt);

	int page_x = pos % ui_spinColumns->value();
	int page_y = (pos / ui_spinColumns->value()) * 2;
	ui_layoutResults->addWidget(txt, page_y, page_x);
	ui_layoutResults->setRowMinimumHeight(page_y, txt->sizeHint().height() + 10);

	if (m_layouts.size() > pos)
	{ addLayout(m_layouts[page->site()], page_y + 1, page_x); }
}
void searchTab::setMergedLabelText(QLabel *txt, const QList<QSharedPointer<Image>> &imgs)
{
	int maxPage = 0;
	for (Page *p : m_pages)
	{
		int pagesCount = p->pagesCount();
		if (pagesCount > maxPage)
			maxPage = pagesCount;
	}

	int sumImages = 0;
	for (Page *p : m_pages)
	{
		int imagesCount = p->imagesCount();
		if (imagesCount > 0)
			sumImages += p->imagesCount();
	}

	QString links;
	if (m_pages.count() > 5)
		links = "Multiple sources";
	else
		for (Page *p : m_pages)
			links += QString(!links.isEmpty() ? ", " : "") + "<a href=\""+p->url().toString().toHtmlEscaped()+"\">"+p->site()->name()+"</a>";

	txt->setText(QString(links + " - Page %1 of %2 (%3 of max %4)").arg(ui_spinPage->value()).arg(maxPage).arg(imgs.count()).arg(sumImages));
}
void searchTab::setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage)
{
	// No results message
	if (imgs.count() == 0)
	{
		QString meant;
		QStringList reasons = reasonsToFail(page, m_completion, &meant);
		if (!meant.isEmpty() && ui_widgetMeant != nullptr)
		{
			ui_widgetMeant->show();
			ui_labelMeant->setText(meant);
		}
		QString msg = noResultsMessage == nullptr ? tr("No result") : noResultsMessage;
		txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+msg+(reasons.count() > 0 ? "<br/>"+tr("Possible reasons: %1").arg(reasons.join(", ")) : ""));
	}
	else
	{
		int pageCount = page->pagesCount();
		int imageCount = page->imagesCount();
		txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+tr("Page %1 of %2 (%3 of %4)").arg(page->page()).arg(pageCount > 0 ? QString::number(pageCount) : "?").arg(imgs.count()).arg(imageCount > 0 ? QString::number(imageCount) : "?"));
	}
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
	// Show warnings
	if (!page->errors().isEmpty() && m_settings->value("showwarnings", true).toBool())
	{
		txt->setText(txt->text()+"<br/>"+page->errors().join("<br/>"));
	}
}

QBouton *searchTab::createImageThumbnail(int position, QSharedPointer<Image> img)
{
	float size = img->fileSize();
	QString unit = getUnit(&size);
	QColor color = imageColor(img);

	bool resizeInsteadOfCropping = m_settings->value("resizeInsteadOfCropping", true).toBool();
	bool resultsScrollArea = m_settings->value("resultsScrollArea", true).toBool();
	bool fixedWidthLayout = m_settings->value("resultsFixedWidthLayout", false).toBool();
	int borderSize = m_settings->value("borders", 3).toInt();

	QBouton *l = new QBouton(position, resizeInsteadOfCropping, resultsScrollArea, borderSize, color, this);
	l->setCheckable(true);
	l->setChecked(m_selectedImages.contains(img->url()));
	l->setToolTip(QString("%1%2%3%4%5%6%7%8")
		.arg(img->tags().isEmpty() ? " " : tr("<b>Tags:</b> %1<br/><br/>").arg(img->stylishedTags(m_profile).join(" ")))
		.arg(img->id() == 0 ? " " : tr("<b>ID:</b> %1<br/>").arg(img->id()))
		.arg(img->rating().isEmpty() ? " " : tr("<b>Rating:</b> %1<br/>").arg(img->rating()))
		.arg(img->hasScore() ? tr("<b>Score:</b> %1<br/>").arg(img->score()) : " ")
		.arg(img->author().isEmpty() ? " " : tr("<b>User:</b> %1<br/><br/>").arg(img->author()))
		.arg(img->width() == 0 || img->height() == 0 ? " " : tr("<b>Size:</b> %1 x %2<br/>").arg(QString::number(img->width()), QString::number(img->height())))
		.arg(img->fileSize() == 0 ? " " : tr("<b>Filesize:</b> %1 %2<br/>").arg(QString::number(size), unit))
		.arg(!img->createdAt().isValid() ? " " : tr("<b>Date:</b> %1").arg(img->createdAt().toString(tr("'the 'MM/dd/yyyy' at 'hh:mm"))))
	);
	l->scale(img->previewImage(), m_settings->value("thumbnailUpscale", 1.0f).toFloat());
	l->setFlat(true);

	l->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(l, &QWidget::customContextMenuRequested, this, [this, img]{ thumbnailContextMenu(img); });

	if (fixedWidthLayout)
		l->setFixedSize(FIXED_IMAGE_WIDTH + borderSize * 2, FIXED_IMAGE_WIDTH + borderSize * 2);

	connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
	connect(l, SIGNAL(toggled(int, bool, bool)), this, SLOT(toggleImage(int, bool, bool)));
	connect(l, SIGNAL(rightClick(int)), m_parent, SLOT(batchChange(int)));

	return l;
}
void searchTab::thumbnailContextMenu(QSharedPointer<Image> img)
{
	QString fn = m_settings->value("Save/filename").toString();
	QString path = m_settings->value("Save/path").toString();

	QMenu *menu = new QMenu(this);

	menu->addAction(QIcon(":/images/icons/save.png"), tr("Save"), this, [img, fn, path]{ img->loadAndSave(fn, path); });
	if (!m_selectedImagesPtrs.empty())
	{
		menu->addAction(QIcon(":/images/icons/save.png"), tr("Save selected"), this, [this, fn, path]{
			for (QSharedPointer<Image> img : m_selectedImagesPtrs)
			{ img->loadAndSave(fn, path); }
		});
	}

	menu->addSeparator();
	menu->addAction(QIcon(":/images/icons/browser.png"), tr("Open in browser"), this, [img]{ QDesktopServices::openUrl(img->pageUrl()); });

	menu->exec(QCursor::pos());
}

int searchTab::getActualImagesPerPage(Page *page, bool merge)
{
	// If we are using merged results, the images/page corresponds to the total number of images
	if (merge)
		return m_images.count();

	int imagesPerPage;

	// If we can customize the limit, that means we can have confidence in the spin value
	if (page->site()->value("Urls/Selected/Tags").contains("{limit}"))
		imagesPerPage = ui_spinImagesPerPage->value();
	else
		imagesPerPage = page->images().size();

	return (imagesPerPage <= 0 ? 20 : imagesPerPage);
}

void searchTab::addResultsImage(QSharedPointer<Image> img, bool merge)
{
	int absolutePosition = m_images.indexOf(img);
	int relativePosition = merge ? absolutePosition : img->page()->images().indexOf(img);
	int imagesPerPage = getActualImagesPerPage(img->page(), merge);

	QBouton *button = createImageThumbnail(absolutePosition, img);
	m_boutons.insert(img, button);

	FixedSizeGridLayout *layout = m_layouts[m_layouts.contains(nullptr) ? nullptr : img->parentSite()];
	layout->addFixedSizeWidget(button, relativePosition, imagesPerPage);
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

	for (QSharedPointer<Image> img : m_selectedImagesPtrs)
	{
		emit batchAddUnique(DownloadQueryImage(m_settings, img, img->parentSite()));
	}

	m_selectedImagesPtrs.clear();
	m_selectedImages.clear();
	for (QBouton *l : m_boutons)
	{ l->setChecked(false); }
}

void searchTab::updateCheckboxes()
{
	log(QString("Updating checkboxes."));

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
	QSharedPointer<Image> image = m_images.at(id);

	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
		QStringList detected = image->blacklisted(blacklistedtags);
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(parentWidget(), tr("Blacklist"), tr("%n tag figuring in the blacklist detected in this image: %1. Do you want to display it anyway?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}

	zoomWindow *zoom = new zoomWindow(m_images, image, image->page()->site(), m_sites, m_profile, m_parent);
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


void searchTab::selectImage(QSharedPointer<Image> img)
{
	if (!m_selectedImagesPtrs.contains(img))
	{
		m_selectedImagesPtrs.append(img);
		m_selectedImages.append(img->url());
	}
}

void searchTab::unselectImage(QSharedPointer<Image> img)
{
	if (m_selectedImagesPtrs.contains(img))
	{
		int pos = m_selectedImagesPtrs.indexOf(img);
		m_selectedImagesPtrs.removeAt(pos);
		m_selectedImages.removeAt(pos);
	}
}

void searchTab::toggleImage(QSharedPointer<Image> img)
{
	bool selected = m_selectedImagesPtrs.contains(img);
	m_boutons[img]->setChecked(!selected);

	if (selected)
	{
		int pos = m_selectedImagesPtrs.indexOf(img);
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
	sourcesWindow *adv = new sourcesWindow(m_profile, m_selectedSources, m_sites, this);
	connect(adv, SIGNAL(valid(QList<bool>)), this, SLOT(saveSources(QList<bool>)));
	adv->show();
}

void searchTab::saveSources(QList<bool> sel, bool canLoad)
{
	log("Saving sources...");

	QString sav;
	for (bool enabled : sel)
	{ sav += (enabled ? "1" : "0"); }
	m_settings->setValue("sites", sav);
	m_selectedSources = sel;

	// Log into new sources
	QStringList keys = m_sites->keys();
	for (int i = 0; i < m_sites->count(); i++)
	{
		if (sav.at(i) == '1')
		{ m_sites->value(keys[i])->login(m_profile); }
	}

	updateCheckboxes();

	DONE();

	m_mergedMd5s.clear();
	if (m_history.isEmpty() && canLoad)
	{ load(); }
}


void searchTab::loadTags(QStringList tags)
{
	log("Loading results...");

	// Enable or disable scroll mode
	bool resultsScrollArea = m_settings->value("resultsScrollArea", true).toBool();
	ui_scrollAreaResults->setScrollEnabled(resultsScrollArea);

	// Append "additional tags" setting
	tags.append(m_settings->value("add").toString().trimmed().split(" ", QString::SkipEmptyParts));

	// Save previous pages
	QStringList keys = m_sites->keys();
	QMap<QString, Page*> lastPages;
	for (int i = 0; i < m_selectedSources.size(); i++)
	{
		QString site = keys[i];
		if (m_checkboxes.at(i)->isChecked() && m_pages.contains(site))
			lastPages.insert(site, m_pages[site]);
	}

	clear();

	// Disable download buttons
	ui_buttonGetAll->setEnabled(false);
	ui_buttonGetPage->setEnabled(false);
	ui_buttonGetSel->setEnabled(false);

	// Get the search values
	QString search = tags.join(" ");
	int perpage = ui_spinImagesPerPage->value();

	if (!m_from_history)
	{ addHistory(search, ui_spinPage->value(), ui_spinImagesPerPage->value(), ui_spinColumns->value()); }
	m_from_history = false;

	if (search != m_lastTags && !m_lastTags.isNull())
	{ m_mergedMd5s.clear(); }
	if (search != m_lastTags && !m_lastTags.isNull() && m_history_cursor == m_history.size() - 1)
	{ ui_spinPage->setValue(1); }
	m_lastTags = search;

	if (ui_widgetMeant != nullptr)
		ui_widgetMeant->hide();
	ui_buttonFirstPage->setEnabled(ui_spinPage->value() > 1);
	ui_buttonPreviousPage->setEnabled(ui_spinPage->value() > 1);

	bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	if (merged)
		m_layouts.insert(nullptr, createImagesLayout(m_settings));

	for (Site *site : loadSites())
	{
		// Load results
		Page *page = new Page(m_profile, site, m_sites->values(), tags, ui_spinPage->value(), perpage, m_postFiltering->toPlainText().split(" ", QString::SkipEmptyParts), false, this, 0, m_lastPage, m_lastPageMinId, m_lastPageMaxId);
		connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
		connect(page, SIGNAL(failedLoading(Page*)), this, SLOT(failedLoading(Page*)));
		if (lastPages.contains(page->website()))
		{ page->setLastPage(lastPages[page->website()]); }
		m_pages.insert(page->website(), page);

		// Setup the layout
		if (!merged)
			m_layouts.insert(site, createImagesLayout(m_settings));

		// Load tags if necessary
		log(QString("Loading page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
		m_stop = false;
		if (m_settings->value("useregexfortags", true).toBool())
		{
			connect(page, SIGNAL(finishedLoadingTags(Page*)), this, SLOT(finishedLoadingTags(Page*)));
			page->loadTags();
		}

		// Start loading
		page->load();
	}
	if (merged && m_layouts.size() > 0)
	{ addLayout(m_layouts[nullptr], 1, 0); }
	m_page = 0;

	auto spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	ui_layoutResults->addItem(spacer, m_layouts.count() * 2, 0);

	if (merged && ui_progressMergeResults != nullptr)
	{
		ui_progressMergeResults->setValue(0);
		ui_progressMergeResults->setMaximum(m_pages.count());
	}
	if (ui_stackedMergeResults != nullptr)
	{ ui_stackedMergeResults->setCurrentIndex(merged ? 0 : 1); }

	emit changed(this);
}

void searchTab::addLayout(QLayout *layout, int row, int column)
{
	QWidget *layoutWidget = new QWidget;
	layoutWidget->setLayout(layout);
	ui_layoutResults->addWidget(layoutWidget, row, column);
}

FixedSizeGridLayout *searchTab::createImagesLayout(QSettings *settings)
{
	FixedSizeGridLayout *l = new FixedSizeGridLayout;
	l->setHorizontalSpacing(settings->value("Margins/horizontal", 6).toInt());
	l->setVerticalSpacing(settings->value("Margins/vertical", 6).toInt());

	bool fixedWidthLayout = m_settings->value("resultsFixedWidthLayout", false).toBool();
	if (fixedWidthLayout)
	{
		int borderSize = settings->value("borders", 3).toInt();
		l->setFixedWidth(FIXED_IMAGE_WIDTH + borderSize * 2);
	}

	return l;
}


void searchTab::setSources(QList<bool> sources)
{ m_selectedSources = sources; }

QList<bool> searchTab::sources()
{ return m_selectedSources; }
QStringList searchTab::selectedImages()
{ return m_selectedImages; }

QList<Tag> searchTab::results()
{ return m_tags; }
QString searchTab::wiki()
{ return m_wiki; }

void searchTab::onLoad()
{ }


void searchTab::firstPage()
{
	ui_spinPage->setValue(1);
	load();
}
void searchTab::previousPage()
{
	if (ui_spinPage->value() > 1)
	{
		ui_spinPage->setValue(ui_spinPage->value() - 1);
		load();
	}
}
void searchTab::nextPage()
{
	if (ui_spinPage->value() < ui_spinPage->maximum())
	{
		ui_spinPage->setValue(ui_spinPage->value() + 1);
		load();
	}
}
void searchTab::lastPage()
{
	ui_spinPage->setValue(m_pagemax);
	load();
}

void searchTab::setImagesPerPage(int ipp)
{ ui_spinImagesPerPage->setValue(ipp); }
void searchTab::setColumns(int columns)
{ ui_spinColumns->setValue(columns); }
void searchTab::setPostFilter(QString postfilter)
{ m_postFiltering->setText(postfilter); }

int searchTab::imagesPerPage()
{ return ui_spinImagesPerPage->value(); }
int searchTab::columns()
{ return ui_spinColumns->value(); }
QString searchTab::postFilter()
{ return m_postFiltering->toPlainText(); }
