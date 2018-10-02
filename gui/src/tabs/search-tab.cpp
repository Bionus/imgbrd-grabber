#include "tabs/search-tab.h"
#include <QCompleter>
#include <QEventLoop>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSet>
#include <QtMath>
#include <algorithm>
#include "downloader/download-query-image.h"
#include "downloader/image-downloader.h"
#include "functions.h"
#include "helpers.h"
#include "image-context-menu.h"
#include "main-window.h"
#include "models/api/api.h"
#include "models/favorite.h"
#include "models/filename.h"
#include "models/filtering/post-filter.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "sources/sources-window.h"
#include "ui/fixed-size-grid-layout.h"
#include "ui/QBouton.h"
#include "ui/text-edit.h"
#include "ui/verticalscrollarea.h"
#include "viewer/zoom-window.h"

#define FIXED_IMAGE_WIDTH 150


SearchTab::SearchTab(Profile *profile, MainWindow *parent)
	: QWidget(parent), m_profile(profile), m_lastPageMaxId(0), m_lastPageMinId(0), m_sites(profile->getSites()), m_favorites(profile->getFavorites()), m_parent(parent), m_settings(profile->getSettings()), m_pagemax(-1), m_stop(true), m_from_history(false), m_history_cursor(0), m_lastTags(QString())
{
	setAttribute(Qt::WA_DeleteOnClose);

	// Checkboxes
	m_checkboxesSignalMapper = new QSignalMapper(this);
	connect(m_checkboxesSignalMapper, SIGNAL(mapped(QString)), this, SLOT(toggleSource(QString)));

	// Auto-complete list
	m_completion.append(profile->getAutoComplete());
	m_completion.append(profile->getCustomAutoComplete());

	// Favorite tags
	m_completion.reserve(m_completion.count() + m_favorites.count());
	for (const Favorite &fav : qAsConst(m_favorites))
		m_completion.append(fav.getName());

	// Modifiers
	for (auto it = m_sites.constBegin(); it != m_sites.constEnd(); ++it)
	{
		Site *site = it.value();
		const QStringList modifiers = site->getApis().first()->modifiers();
		m_completion.append(modifiers);
	}

	m_completion.removeDuplicates();
	m_completion.sort();

	setSelectedSources(m_settings);
}

void SearchTab::init()
{
	m_endlessLoadingEnabled = true;
	m_endlessLoadOffset = 0;
	const QString infinite = m_settings->value("infiniteScroll", "disabled").toString();

	// Always hide scroll button before results are loaded
	if (ui_buttonEndlessLoad != nullptr)
		ui_buttonEndlessLoad->hide();

	if (infinite == "scroll")
		connect(ui_scrollAreaResults, &VerticalScrollArea::endOfScrollReached, this, &SearchTab::endlessLoad);

	if (infinite != "disabled" && ui_checkMergeResults != nullptr)
		connect(ui_checkMergeResults, &QCheckBox::toggled, this, &SearchTab::setMergeResultsMode);
}

SearchTab::~SearchTab()
{
	m_pages.clear();
	m_images.clear();
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();

	for (QLayout *layout : qAsConst(m_siteLayouts))
	{ clearLayout(layout); }
	qDeleteAll(m_siteLayouts);
	m_siteLayouts.clear();
	m_layouts.clear();

	m_boutons.clear();
}


void SearchTab::setSelectedSources(QSettings *settings)
{
	QStringList sav = settings->value("sites").toStringList();
	for (const QString &key : sav)
	{
		if (!m_sites.contains(key))
			continue;

		m_selectedSources.append(m_sites.value(key));
	}
}

void SearchTab::optionsChanged()
{
	log(QStringLiteral("Updating settings for tab \"%1\".").arg(windowTitle()), Logger::Debug);
	// ui->retranslateUi(this);

	ui_spinImagesPerPage->setValue(m_settings->value("limit", 20).toInt());
	ui_spinColumns->setValue(m_settings->value("columns", 1).toInt());

	/*QPalette p = ui->widgetResults->palette();
	p.setColor(ui->widgetResults->backgroundRole(), QColor(m_settings->value("serverBorderColor", "#000000").toString()));
	ui->widgetResults->setPalette(p);*/
	ui_layoutResults->setHorizontalSpacing(m_settings->value("Margins/main", 10).toInt());
}

void SearchTab::setTagsFromPages(const QMap<QString, QList<QSharedPointer<Page>>> &pages)
{
	// Tags for this page
	QList<Tag> tagList;
	QStringList tagsGot;
	for (const auto &ps : pages)
	{
		QList<Tag> tags = ps.last()->tags();
		for (const Tag &tag : tags)
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
					const int index = tagsGot.indexOf(tag.text());
					tagList[index].setCount(tagList[index].count() + tag.count());
				}
				else
				{
					tagList.append(tag);
					tagsGot.append(tag.text());
				}
			}
		}
	}

	// We sort tags by frequency
	std::sort(tagList.begin(), tagList.end(), sortTagsByCount);

	m_tags = tagList;
	m_parent->setTags(m_tags, this);
}

QStringList SearchTab::reasonsToFail(Page *page, const QStringList &completion, QString *meant)
{
	QStringList reasons = QStringList();

	// If the request yielded no source, the server may be offline
	if (!page->hasSource())
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

			int lev = qCeil((tag.length() - 1) / 4.0);
			for (const QString &comp : completion)
			{
				// Ignore tags that are too long
				if (abs(comp.length() - tag.length()) > lev)
					continue;

				const int d = levenshtein(tag, comp);
				if (d < lev)
				{
					if (results[tag].isEmpty())
					{ c++; }
					results[tag] = "<b>" + comp + "</b>";
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
			*meant = QString(R"(<a href="%1" style="color:black;text-decoration:none;">%2</a>)").arg(cl.join(" ").toHtmlEscaped(), res.join(" "));
		}
	}

	return reasons;
}

void SearchTab::clear()
{
	// Reset loading variables
	m_stop = true;
	m_pagemax = -1;
	m_endlessLoadOffset = 0;

	// Clear page details
	m_tags.clear();
	m_parent->setTags(m_tags, this);
	m_parent->setWiki(QString(), this);

	// Clear layout
	for (int i = 0; i < ui_layoutResults->rowCount(); ++i)
	{ ui_layoutResults->setRowMinimumHeight(i, 0); }
	for (QLayout *layout : qAsConst(m_siteLayouts))
	{ clearLayout(layout); }
	qDeleteAll(m_siteLayouts);
	m_siteLayouts.clear();
	m_layouts.clear();
	m_boutons.clear();
	qDeleteAll(m_siteLabels);
	m_siteLabels.clear();
	clearLayout(ui_layoutResults);

	// Abort current loadings
	for (const auto &pages : qAsConst(m_pages))
	{
		for (const auto &page : pages)
		{
			page->abort();
			page->abortTags();
		}
	}
	for (auto it = m_thumbnailsLoading.constBegin(); it != m_thumbnailsLoading.constEnd(); ++it)
	{
		QNetworkReply *reply = it.key();
		if (reply->isRunning())
		{ reply->abort(); }
	}

	m_pages.clear();
	m_images.clear();

	m_selectedImagesPtrs.clear();
	m_thumbnailsLoading.clear();
	m_validImages.clear();
}

TextEdit *SearchTab::createAutocomplete()
{
	auto *ret = new TextEdit(m_profile, this);
	connect(ret, &TextEdit::returnPressed, this, &SearchTab::load);
	connect(ret, &TextEdit::addedFavorite, this, &SearchTab::setFavoriteImage);

	// Add auto-complete if necessary
	if (m_settings->value("autocompletion", true).toBool())
	{
		auto *completer = new QCompleter(m_completion, ret);
		completer->setCaseSensitivity(Qt::CaseInsensitive);

		ret->setCompleter(completer);
	}

	return ret;
}

void SearchTab::setMergeResultsMode(bool merged)
{
	// Restore endless loading mode
	if (merged == m_pageMergedMode)
	{
		setEndlessLoadingMode(m_endlessLoadingEnabledPast);
	}

	// Disable endless loading
	else
	{
		m_endlessLoadingEnabledPast = m_endlessLoadingEnabled;
		setEndlessLoadingMode(false);
	}
}

void SearchTab::setEndlessLoadingMode(bool enabled)
{
	// Toggle endless loading button
	if (ui_buttonEndlessLoad != nullptr && m_settings->value("infiniteScroll", "disabled") == "button")
		ui_buttonEndlessLoad->setVisible(enabled);

	m_endlessLoadingEnabled = enabled;
}

void SearchTab::finishedLoading(Page *page)
{
	if (m_stop)
		return;

	m_lastPage = page->page();
	m_lastPageMinId = page->minId();
	m_lastPageMaxId = page->maxId();

	// Filter images depending on tabs
	QList<QSharedPointer<Image>> validImages;
	QString error;
	for (const QSharedPointer<Image> &img : page->images())
		if (validateImage(img, error))
			validImages.append(img);
		else if (!error.isEmpty())
			log(error);
	m_validImages.insert(page, validImages);

	// Remove already existing images for merged results
	const bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	const QList<QSharedPointer<Image>> imgs = merged ? mergeResults(page->page(), validImages) : validImages;

	m_images.append(imgs);

	updatePaginationButtons(page);
	addResultsPage(page, imgs, merged);

	if (!m_settings->value("useregexfortags", true).toBool())
		setTagsFromPages(m_pages);

	postLoading(page, imgs);
}

void SearchTab::failedLoading(Page *page)
{
	if (m_stop)
		return;

	const bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	addResultsPage(page, QList<QSharedPointer<Image>>(), merged);

	postLoading(page, page->images());
}

void SearchTab::httpsRedirect(Page *page)
{
	QSettings *settings = m_profile->getSettings();

	const QString action = settings->value("ssl_autocorrect", "ask").toString();
	bool setSsl = action == "always";

	if (action == "ask")
	{
		QMessageBox box(this);
		box.setWindowTitle(tr("HTTPS redirection detected"));
		box.setText(tr("An HTTP to HTTPS redirection has been detected for the website %1. Do you want to enable SSL on it? The recommended setting is 'yes'.").arg(page->site()->url()));
		QPushButton *yes = box.addButton(QMessageBox::Yes);
		QPushButton *always = box.addButton(tr("Always"), QMessageBox::YesRole);
		QPushButton *neverWebsite = box.addButton(tr("Never for that website"), QMessageBox::NoRole);
		QPushButton *never = box.addButton(tr("Never"), QMessageBox::NoRole);
		box.exec();

		if (box.clickedButton() == yes)
		{ setSsl = true; }
		else if (box.clickedButton() == always)
		{
			setSsl = true;
			settings->setValue("ssl_autocorrect", "always");
		}
		else if (box.clickedButton() == neverWebsite)
		{ page->site()->setSetting("ssl_never_correct", true, false); }
		else if (box.clickedButton() == never)
		{ settings->setValue("ssl_autocorrect", "never"); }
	}

	if (setSsl)
	{
		log(QStringLiteral("[%1] Enabling HTTPS").arg(page->site()->url()), Logger::Info);
		page->site()->setSetting("ssl", true, false);
	}
}

void SearchTab::postLoading(Page *page, const QList<QSharedPointer<Image>> &imgs)
{
	m_page++;

	const bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	const bool finished = m_page == m_pages.count() || (merged && ui_progressMergeResults != nullptr && ui_progressMergeResults->value() == ui_progressMergeResults->maximum());

	if (merged)
	{
		// Increase the progress bar status
		if (ui_progressMergeResults != nullptr)
			ui_progressMergeResults->setValue(ui_progressMergeResults->value() + 1);

		// Hide progress bar when we load the last page
		if (ui_stackedMergeResults != nullptr && finished)
			ui_stackedMergeResults->setCurrentIndex(1);

		// Create the label when loading the first page
		if (m_page == 1 && m_siteLabels.isEmpty())
		{
			QLabel *txt = new QLabel(this);
			txt->setOpenExternalLinks(true);
			setMergedLabelText(txt, m_images);
			m_siteLabels.insert(nullptr, txt);

			ui_layoutResults->addWidget(txt, 0, 0);
			ui_layoutResults->setRowMinimumHeight(0, txt->sizeHint().height() + 10);
		}
		else
		{ setMergedLabelText(m_siteLabels[nullptr], m_images); }
	}

	// Load thumbnails
	for (const auto &img : imgs)
	{
		const QUrl thumbnailUrl = img->url(Image::Size::Thumbnail);
		if (thumbnailUrl.isValid())
		{ loadImageThumbnail(page, img, thumbnailUrl); }
	}

	// Re-enable endless loading if all sources have reached the last page
	if (finished)
	{
		bool allFinished = true;
		for (auto ps : qAsConst(m_pages))
		{
			const int pagesCount = ps.first()->pagesCount();
			const int imagesPerPage = ps.first()->imagesPerPage();
			if (ps.last()->page() < pagesCount && ps.last()->pageImageCount() >= imagesPerPage)
				allFinished = false;
		}
		if (!allFinished)
		{
			setEndlessLoadingMode(true);
		}
	}

	ui_buttonGetAll->setDisabled(m_images.empty());
	ui_buttonGetPage->setDisabled(m_images.empty());
	ui_buttonGetSel->setDisabled(m_images.empty());
}

void SearchTab::updatePaginationButtons(Page *page)
{
	// Update max page counter
	int pageCount = page->pagesCount();
	int maxPages = page->maxPagesCount();
	if (pageCount <= 0 && maxPages > 0)
		pageCount = maxPages;
	if (pageCount > m_pagemax || m_pagemax == -1)
		m_pagemax = pageCount;

	// Update page spinbox max value
	ui_spinPage->setMaximum(page->imagesCount() == -1 || page->pagesCount() == -1 ? 100000 : qMax(1, m_pagemax));

	// Enable/disable buttons
	ui_buttonNextPage->setEnabled(m_pagemax > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1 || (page->imagesCount() == 0 && page->pageImageCount() > 0));
	ui_buttonLastPage->setEnabled(m_pagemax > ui_spinPage->value() || page->imagesCount() == -1 || page->pagesCount() == -1);
}

void SearchTab::finishedLoadingTags(Page *page)
{
	setTagsFromPages(m_pages);

	// Wiki
	if (!page->wiki().isEmpty())
	{
		m_wiki = page->wiki();
		m_parent->setWiki(m_wiki, this);
	}

	updatePaginationButtons(page);

	// Update image and page count
	QList<QSharedPointer<Image>> imgs;
	QString error;
	for (const QSharedPointer<Image> &img : page->images())
		if (validateImage(img, error))
			imgs.append(img);

	if (ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked() && m_siteLabels.contains(nullptr))
		setMergedLabelText(m_siteLabels[nullptr], m_images);
	else if (m_siteLabels.contains(page->site()))
		setPageLabelText(m_siteLabels[page->site()], page, imgs);
}

void SearchTab::loadImageThumbnail(Page *page, QSharedPointer<Image> img, const QUrl &url)
{
	Site *site = page->site();

	QNetworkReply *reply = site->get(site->fixUrl(url.toString()), page, "preview");
	reply->setParent(this);

	m_thumbnailsLoading[reply] = std::move(img);
	connect(reply, &QNetworkReply::finished, this, &SearchTab::finishedLoadingPreview);
}

void SearchTab::finishedLoadingPreview()
{
	if (m_stop)
		return;

	auto *reply = qobject_cast<QNetworkReply*>(sender());

	// Aborted
	if (reply->error() == QNetworkReply::OperationCanceledError)
	{
		reply->deleteLater();
		return;
	}

	// Try to find associated image
	QSharedPointer<Image> img;
	if (m_thumbnailsLoading.contains(reply))
	{
		img = m_thumbnailsLoading[reply];
		m_thumbnailsLoading.remove(reply);
	}
	else
	{
		log(QStringLiteral("Could not find image related to loaded thumbnail '%1'").arg(reply->url().toString()), Logger::Error);
		return;
	}

	// Check redirection
	QUrl redirection = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty())
	{
		loadImageThumbnail(img->page(), img, redirection);
		reply->deleteLater();
		return;
	}

	// Loading error
	if (reply->error() != QNetworkReply::NoError)
	{
		const QString ext = getExtension(reply->url());
		if (ext != "jpg")
		{
			log(QStringLiteral("Error loading thumbnail (%1), new try with extension JPG").arg(reply->errorString()), Logger::Warning);
			const QUrl newUrl = setExtension(reply->url(), "jpg");
			loadImageThumbnail(img->page(), img, newUrl);
		}
		else
		{
			log(QStringLiteral("Error loading thumbnail (%1)").arg(reply->errorString()), Logger::Error);
		}

		reply->deleteLater();
		return;
	}

	// Load preview from result
	QPixmap preview;
	preview.loadFromData(reply->readAll());
	reply->deleteLater();
	if (preview.isNull())
	{
		log(QStringLiteral("One of the thumbnails is empty (`%1`).").arg(img->url(Image::Size::Thumbnail).toString()), Logger::Error);
		if (img->hasTag(QStringLiteral("flash")))
		{ preview.load(QStringLiteral(":/images/flash.png")); }
		else
		{ return; }
	}
	img->setPreviewImage(preview);

	// Download whitelist images on thumbnail view
	Blacklist whitelistedTags;
	for (const QString &tag : m_settings->value("whitelistedtags").toString().split(" ", QString::SkipEmptyParts))
	{ whitelistedTags.add(tag); }
	QStringList detected = m_profile->getBlacklist().match(img->tokens(m_profile));
	QStringList whitelisted = whitelistedTags.match(img->tokens(m_profile));
	if (!whitelisted.isEmpty() && m_settings->value("whitelist_download", "image").toString() == "page")
	{
		bool download = false;
		if (!detected.isEmpty())
		{
			const int reponse = QMessageBox::question(this, "Grabber", tr("Some tags from the image are in the whitelist: %1. However, some tags are in the blacklist: %2. Do you want to download it anyway?").arg(whitelisted.join(", "), detected.join(", ")), QMessageBox::Yes | QMessageBox::Open | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{ download = true; }
			else if (reponse == QMessageBox::Open)
			{ openImage(img); }
		}
		else
		{ download = true; }

		if (download)
		{
			auto downloader = new ImageDownloader(m_profile, img, m_settings->value("Save/filename").toString(), m_settings->value("Save/path").toString(), 1, true, true, true, this);
			downloader->save();
			connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
		}
	}

	const bool merge = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked() && !m_images.empty();
	addResultsImage(img, img->page(), merge);
}

/**
 * Get the proportion (from 0 to 1) of known tag types in a given image.
 */
double getImageKnownTagProportion(const QSharedPointer<Image> &img)
{
	if (img->tags().isEmpty())
		return 0;

	int known = 0;
	for (const Tag &tag : img->tags())
	{
		if (!tag.type().isUnknown())
			known++;
	}

	return (static_cast<double>(known) / static_cast<double>(img->tags().count()));
}

QList<QSharedPointer<Image>> SearchTab::mergeResults(int page, const QList<QSharedPointer<Image>> &results)
{
	QMap<QString, double> pageMd5s;
	for (const QSharedPointer<Image> &img : qAsConst(m_images))
	{
		QString md5 = img->md5();
		if (md5.isEmpty())
			continue;

		const double proportion = getImageKnownTagProportion(img);
		pageMd5s[md5] = proportion;
		addMergedMd5(page, md5);
	}

	QMap<QString, int> imgMd5s;
	for (int i = 0; i < m_images.count(); ++i)
		imgMd5s.insert(m_images[i]->md5(), i);

	QList<QSharedPointer<Image>> ret;
	for (const QSharedPointer<Image> &img : results)
	{
		QString md5 = img->md5();
		const double proportion = getImageKnownTagProportion(img);

		if (md5.isEmpty() || ((!pageMd5s.contains(md5) || proportion > pageMd5s[md5]) && !containsMergedMd5(page, md5)))
		{
			if (pageMd5s.contains(md5) && proportion > pageMd5s[md5])
			{
				m_images[imgMd5s[md5]] = img;
				pageMd5s[md5] = proportion;
			}
			else
			{
				ret.append(img);

				if (!md5.isEmpty())
				{
					pageMd5s[md5] = proportion;
					addMergedMd5(page, md5);
				}
			}
		}
	}

	return ret;
}

void SearchTab::addMergedMd5(int page, const QString &md5)
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

bool SearchTab::containsMergedMd5(int page, const QString &md5)
{
	for (const QPair<int, QSet<QString>> &pair : qAsConst(m_mergedMd5s))
	{
		// We only check the sets before the page was loaded
		if (pair.first == page)
			break;

		if (pair.second.contains(md5))
			return true;
	}

	return false;
}

void SearchTab::addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, const QString &noResultsMessage)
{
	if (merged)
		return;

	const int pos = m_pages.keys().indexOf(page->website());
	if (pos < 0)
		return;

	const int page_x = pos % ui_spinColumns->value();
	const int page_y = (pos / ui_spinColumns->value()) * 2;

	Site *site = page->site();
	if (!m_siteLabels.contains(site))
	{
		QLabel *txt = new QLabel(this);
		txt->setOpenExternalLinks(true);
		m_siteLabels.insert(site, txt);

		ui_layoutResults->addWidget(txt, page_y, page_x);
		ui_layoutResults->setRowMinimumHeight(page_y, txt->sizeHint().height() + 10);
	}
	setPageLabelText(m_siteLabels[site], page, imgs, noResultsMessage);

	if (m_siteLayouts.contains(page->site()) && m_pages.value(page->website()).count() == 1)
	{ addLayout(m_siteLayouts[page->site()], page_y + 1, page_x); }
}
void SearchTab::setMergedLabelText(QLabel *txt, const QList<QSharedPointer<Image>> &imgs)
{
	int maxPage = 0;
	int sumImages = 0;
	int firstPage = ui_spinPage->value() + m_endlessLoadOffset;
	int lastPage = ui_spinPage->value() + m_endlessLoadOffset;

	for (const auto &ps : qAsConst(m_pages))
	{
		const QSharedPointer<Page> first = ps.first();
		const int imagesCount = first->imagesCount();
		if (imagesCount > 0)
			sumImages += first->imagesCount();

		for (const QSharedPointer<Page> &p : ps)
		{
			const int pagesCount = p->pagesCount();
			if (pagesCount > maxPage)
				maxPage = pagesCount;

			if (p->page() < firstPage)
				firstPage = p->page();
			if (p->page() > lastPage)
				lastPage = p->page();
		}
	}

	QString links;
	if (m_pages.count() > 5)
	{ links = "Multiple sources"; }
	else
	{
		for (const auto &ps : qAsConst(m_pages))
		{
			const auto &p = ps.last();
			links += QString(!links.isEmpty() ? ", " : QString()) + "<a href=\"" + p->url().toString().toHtmlEscaped() + "\">" + p->site()->name() + "</a>";
		}
	}

	const QString page = firstPage != lastPage ? QStringLiteral("%1-%2").arg(firstPage).arg(lastPage) : QString::number(lastPage);
	const QString countLabel = tr("Page %1 of %2 (%3 of %4)").arg(page).arg(maxPage).arg(imgs.count()).arg(tr("max %1").arg(sumImages));
	txt->setText(QString(links + " - " + countLabel));
}
void SearchTab::setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, const QString &noResultsMessage)
{
	const int pageCount = page->pagesCount();
	const int imageCount = page->imagesCount();

	int firstPage = imgs.count() > 0 ? page->page() : 0;
	int lastPage = imgs.count() > 0 ? page->page() : 0;
	int totalCount = 0;
	for (const QSharedPointer<Page> &p : m_pages[page->website()])
	{
		if (p->images().count() == 0)
			continue;
		if (p->page() < firstPage || firstPage == 0)
			firstPage = p->page();
		if (p->page() > lastPage)
			lastPage = p->page();
		totalCount += p->images().count();
	}

	// No results message
	if (totalCount == 0)
	{
		QString meant;
		QStringList reasons = reasonsToFail(page, m_completion, &meant);
		if (!meant.isEmpty() && ui_widgetMeant != nullptr)
		{
			ui_widgetMeant->show();
			ui_labelMeant->setText(meant);
		}
		const QString msg = noResultsMessage == nullptr ? tr("No result") : noResultsMessage;
		txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+msg+(reasons.count() > 0 ? "<br/>"+tr("Possible reasons: %1").arg(reasons.join(", ")) : QString()));
	}
	else
	{
		const QString pageLabel = firstPage != lastPage ? QString("%1-%2").arg(firstPage).arg(lastPage) : QString::number(lastPage);
		const QString pageCountStr = pageCount > 0
			? (page->pagesCount(false) == -1 ? "~" : QString()) + QString::number(pageCount)
			: (page->maxPagesCount() == -1 ? "?" : tr("max %1").arg(page->maxPagesCount()));
		const QString imageCountStr = imageCount > 0
			? (page->imagesCount(false) == -1 ? "~" : QString()) + QString::number(imageCount)
			: (page->maxImagesCount() == -1 ? "?" : tr("max %1").arg(page->maxImagesCount()));

		const QString countLabel = tr("Page %1 of %2 (%3 of %4)").arg(pageLabel, pageCountStr).arg(totalCount).arg(imageCountStr);
		txt->setText("<a href=\"" + page->url().toString().toHtmlEscaped() + "\">" + page->site()->name() + "</a> - " + countLabel);
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
		txt->setText(txt->text() + "<br/>" + page->errors().join("<br/>"));
	}
}

QBouton *SearchTab::createImageThumbnail(int position, const QSharedPointer<Image> &img)
{
	const QColor &color = img->color();

	const bool resizeInsteadOfCropping = m_settings->value("resizeInsteadOfCropping", true).toBool();
	const bool resultsScrollArea = m_settings->value("resultsScrollArea", true).toBool();
	const bool fixedWidthLayout = m_settings->value("resultsFixedWidthLayout", false).toBool();
	const int borderSize = m_settings->value("borders", 3).toInt();
	const qreal upscale = m_settings->value("thumbnailUpscale", 1.0).toDouble();

	QBouton *l = new QBouton(position, resizeInsteadOfCropping, resultsScrollArea, borderSize, color, this);
	l->setCheckable(true);
	l->setChecked(m_selectedImages.contains(img->url()));
	l->setInvertToggle(m_settings->value("invertToggle", false).toBool());
	l->setToolTip(img->tooltip());
	if (img->previewImage().isNull())
	{ l->scale(QPixmap(":/images/noimage.png"), upscale); }
	else
	{ l->scale(img->previewImage(), upscale); }
	l->setFlat(true);

	QString counter = img->counter();
	if (!counter.isEmpty())
	{ l->setCounter(counter); }

	l->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(l, &QWidget::customContextMenuRequested, this, [this, position, img]{ thumbnailContextMenu(position, img); });

	if (fixedWidthLayout)
	{
		const int dim = qFloor(FIXED_IMAGE_WIDTH * upscale + borderSize * 2);
		l->setFixedSize(dim, dim);
	}

	connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
	connect(l, SIGNAL(toggled(int, bool, bool)), this, SLOT(toggleImage(int, bool, bool)));

	return l;
}

QString getImageAlreadyExists(Image *img, Profile *profile)
{
	QSettings *settings = profile->getSettings();
	const QString path = settings->value("Save/path").toString().replace("\\", "/");
	const QString fn = settings->value("Save/filename").toString();

	if (Filename(fn).needExactTags(img->parentSite()) == 0)
	{
		QStringList files = img->path(fn, path, 0, true, true, true, true);
		for (const QString &file : files)
		{
			if (QFile(file).exists())
				return file;
		}
	}

	return profile->md5Exists(img->md5());
}

void SearchTab::thumbnailContextMenu(int position, const QSharedPointer<Image> &img)
{
	QMenu *menu = new ImageContextMenu(m_settings, img, m_parent, this);
	QAction *first = menu->actions().first();

	// Save image
	auto *mapperSave = new QSignalMapper(this);
	connect(mapperSave, SIGNAL(mapped(int)), this, SLOT(contextSaveImage(int)));
	QAction *actionSave;
	if (!getImageAlreadyExists(img.data(), m_profile).isEmpty())
	{ actionSave = new QAction(QIcon(":/images/status/error.png"), tr("Delete"), menu); }
	else
	{ actionSave = new QAction(QIcon(":/images/icons/save.png"), tr("Save"), menu); }
	menu->insertAction(first, actionSave);
	connect(actionSave, SIGNAL(triggered()), mapperSave, SLOT(map()));
	mapperSave->setMapping(actionSave, position);

	// Save image as...
	auto *mapperSaveAs = new QSignalMapper(this);
	connect(mapperSaveAs, SIGNAL(mapped(int)), this, SLOT(contextSaveImageAs(int)));
	QAction *actionSaveAs = new QAction(QIcon(":/images/icons/save-as.png"), tr("Save as..."), menu);
	menu->insertAction(first, actionSaveAs);
	connect(actionSaveAs, SIGNAL(triggered()), mapperSaveAs, SLOT(map()));
	mapperSaveAs->setMapping(actionSaveAs, position);

	if (!m_selectedImagesPtrs.empty())
	{
		QAction *actionSaveSelected = new QAction(QIcon(":/images/icons/save.png"), tr("Save selected"), menu);
		connect(actionSaveSelected, &QAction::triggered, this, &SearchTab::contextSaveSelected);
		menu->insertAction(first, actionSaveSelected);
	}

	menu->insertSeparator(first);

	menu->exec(QCursor::pos());
}
void SearchTab::contextSaveImage(int position)
{
	QSharedPointer<Image> image = m_images.at(position);
	Image *img = image.data();

	QString already = getImageAlreadyExists(img, m_profile);
	if (!already.isEmpty())
	{ QFile(already).remove(); }
	else
	{
		const QString fn = m_settings->value("Save/filename").toString();
		const QString path = m_settings->value("Save/path").toString();

		auto downloader = new ImageDownloader(m_profile, image, fn, path, 1, true, true, true, this);
		connect(downloader, &ImageDownloader::downloadProgress, this, &SearchTab::contextSaveImageProgress);
		connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
		downloader->save();
	}
}
void SearchTab::contextSaveImageAs(int position)
{
	QSharedPointer<Image> image = m_images.at(position);
	Image *img = image.data();

	QString fn = m_settings->value("Save/filename").toString();
	QString tmpPath;

	// If the MD5 is required for the filename, we first download the image
	if (fn.contains("%md5") && image->md5().isEmpty())
	{
		tmpPath = QDir::temp().absoluteFilePath("grabber-saveAs-" + QString::number(qrand(), 16));

		QEventLoop loop;
		ImageDownloader downloader(m_profile, image, QStringList() << tmpPath, 1, true, true, true, this);
		connect(&downloader, &ImageDownloader::saved, &loop, &QEventLoop::quit);
		downloader.save();
		loop.exec();
	}

	Filename format(fn);
	QStringList filenames = format.path(*img, m_profile);
	const QString filename = filenames.first().section(QDir::separator(), -1);
	const QString lastDir = m_settings->value("Zoom/lastDir").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty())
	{
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Zoom/lastDir", path.section(QDir::separator(), 0, -2));

		if (!tmpPath.isEmpty())
		{ QFile::rename(tmpPath, path); }
		else
		{
			auto downloader = new ImageDownloader(m_profile, image, QStringList() << path, 1, true, true, true, this);
			connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
			downloader->save();
		}
	}
	else if (!tmpPath.isEmpty())
	{ QFile::remove(tmpPath); }
}
void SearchTab::contextSaveSelected()
{
	const QString fn = m_settings->value("Save/filename").toString();
	const QString path = m_settings->value("Save/path").toString();

	for (const QSharedPointer<Image> &img : qAsConst(m_selectedImagesPtrs))
	{
		auto downloader = new ImageDownloader(m_profile, img, fn, path, 1, true, true, true, this);
		connect(downloader, &ImageDownloader::downloadProgress, this, &SearchTab::contextSaveImageProgress);
		connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
		downloader->save();
	}
}
void SearchTab::contextSaveImageProgress(QSharedPointer<Image> img, qint64 v1, qint64 v2)
{
	if (m_boutons.contains(img.data()))
	{ m_boutons[img.data()]->setProgress(v1, v2); }
}

void SearchTab::addResultsImage(const QSharedPointer<Image> &img, Page *page, bool merge)
{
	// Early return if the layout has already been removed
	Page *layoutKey = merge && m_layouts.contains(nullptr) ? nullptr : page;
	if (!m_layouts.contains(layoutKey))
		return;

	// Calculate image absolute position
	int absolutePosition = m_images.indexOf(img);
	if (absolutePosition < 0 && !img->md5().isEmpty())
	{
		int j = 0;
		for (const QSharedPointer<Image> &i : page->images())
		{
			if (i->md5() == img->md5())
			{
				absolutePosition = j;
				break;
			}
			j++;
		}
	}

	// Calculate relative position compared to validated images
	int relativePosition = merge
		? absolutePosition
		: m_validImages[page].indexOf(img);

	QBouton *button = createImageThumbnail(absolutePosition, img);
	m_boutons.insert(img.data(), button);

	FixedSizeGridLayout *layout = m_layouts[layoutKey];
	layout->insertWidget(relativePosition, button);
}

void SearchTab::addHistory(const QString &tags, int page, int ipp, int cols)
{
	QMap<QString, QString> srch;
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
void SearchTab::historyBack()
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
void SearchTab::historyNext()
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

void SearchTab::getSel()
{
	if (m_selectedImagesPtrs.empty())
		return;

	for (const QSharedPointer<Image> &img : qAsConst(m_selectedImagesPtrs))
	{
		emit batchAddUnique(DownloadQueryImage(m_settings, *img, img->parentSite()));
	}

	m_selectedImagesPtrs.clear();
	m_selectedImages.clear();
	for (QBouton *l : qAsConst(m_boutons))
	{ l->setChecked(false); }
}

void SearchTab::updateCheckboxes()
{
	if (ui_layoutSourcesList == nullptr)
		return;

	log(QStringLiteral("Updating checkboxes."));

	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();

	const int n = m_settings->value("Sources/Letters", 3).toInt();
	int m = n;

	for (auto it = m_sites.constBegin(); it != m_sites.constEnd(); ++it)
	{
		Site *site = it.value();
		QString url = site->url();

		if (url.startsWith("www."))
		{ url = url.right(url.length() - 4); }
		else if (url.startsWith("chan."))
		{ url = url.right(url.length() - 5); }

		if (n < 0)
		{
			m = url.indexOf('.');
			if (n < -1 && url.indexOf('.', m + 1) != -1)
			{ m = url.indexOf('.', m + 1); }
		}

		QCheckBox *c = new QCheckBox(url.left(m), this);
			c->setChecked(m_selectedSources.contains(site));
			m_checkboxesSignalMapper->setMapping(c, it.key());
			connect(c, SIGNAL(toggled(bool)), m_checkboxesSignalMapper, SLOT(map()));
			ui_layoutSourcesList->addWidget(c);

		m_checkboxes.append(c);
	}

	DONE();
}

void SearchTab::webZoom(int id)
{
	if (id < 0 || id >= m_images.count())
		return;

	const QSharedPointer<Image> &image = m_images.at(id);

	QStringList detected = m_profile->getBlacklist().match(image->tokens(m_profile));
	if (!detected.isEmpty())
	{
		const int reply = QMessageBox::question(parentWidget(), tr("Blacklist"), tr("%n tag figuring in the blacklist detected in this image: %1. Do you want to display it anyway?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No)
		{ return; }
	}

	openImage(image);
}

void SearchTab::openImage(const QSharedPointer<Image> &image)
{
	if (image->isGallery())
	{
		QString galleryId = image->md5();
		if (galleryId.isEmpty())
		{ galleryId = QString::number(image->id()); }
		m_parent->addGalleryTab(image->parentSite(), image->name(), galleryId);
		return;
	}

	if (m_settings->value("Zoom/singleWindow", false).toBool() && !m_lastZoomWindow.isNull())
	{
		m_lastZoomWindow->reuse(m_images, image, image->page()->site());
		m_lastZoomWindow->activateWindow();
		return;
	}

	ZoomWindow *zoom = new ZoomWindow(m_images, image, image->page()->site(), m_profile, m_parent);
	connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
	connect(zoom, SIGNAL(poolClicked(int, QString)), m_parent, SLOT(addPoolTab(int, QString)));
	zoom->show();

	m_lastZoomWindow = zoom;
}


void SearchTab::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::XButton1)
	{ previousPage(); }
	else if (e->button() == Qt::XButton2)
	{ nextPage(); }
}


void SearchTab::selectImage(const QSharedPointer<Image> &img)
{
	if (!m_selectedImagesPtrs.contains(img))
	{
		m_selectedImagesPtrs.append(img);
		m_selectedImages.append(img->url());
	}
}

void SearchTab::unselectImage(const QSharedPointer<Image> &img)
{
	if (m_selectedImagesPtrs.contains(img))
	{
		const int pos = m_selectedImagesPtrs.indexOf(img);
		m_selectedImagesPtrs.removeAt(pos);
		m_selectedImages.removeAt(pos);
	}
}

void SearchTab::toggleImage(const QSharedPointer<Image> &img)
{
	const bool selected = m_selectedImagesPtrs.contains(img);
	m_boutons[img.data()]->setChecked(!selected);

	if (selected)
	{
		const int pos = m_selectedImagesPtrs.indexOf(img);
		m_selectedImagesPtrs.removeAt(pos);
		m_selectedImages.removeAt(pos);
	}
	else
	{
		m_selectedImagesPtrs.append(img);
		m_selectedImages.append(img->url());
	}
}

void SearchTab::toggleImage(int id, bool toggle, bool range)
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



void SearchTab::openSourcesWindow()
{
	auto adv = new SourcesWindow(m_profile, m_selectedSources, this);
	connect(adv, SIGNAL(valid(QList<Site*>)), this, SLOT(saveSources(QList<Site*>)));
	adv->show();
}

void SearchTab::saveSources(const QList<Site*> &sel, bool canLoad)
{
	log(QStringLiteral("Saving sources..."));

	QStringList sav;
	sav.reserve(sel.count());
	for (Site *enabled : sel)
	{ sav.append(enabled->url()); }
	m_settings->setValue("sites", sav);
	m_selectedSources = sel;

	// Log into new sources
	for (Site *site : sel)
	{ site->login(); }

	updateCheckboxes();

	DONE();

	m_mergedMd5s.clear();
	if (m_history.isEmpty() && canLoad)
	{ load(); }
}


void SearchTab::loadTags(QStringList tags)
{
	log(QStringLiteral("Loading results..."));

	// Enable or disable scroll mode
	const bool resultsScrollArea = m_settings->value("resultsScrollArea", true).toBool();
	ui_scrollAreaResults->setScrollEnabled(resultsScrollArea);

	// Append "additional tags" setting
	tags.append(m_settings->value("add").toString().trimmed().split(" ", QString::SkipEmptyParts));

	// Save previous pages
	m_lastPages.clear();
	for (Site *sel : qAsConst(m_selectedSources))
	{
		const QString &site = sel->url();
		if (m_pages.contains(site))
			m_lastPages.insert(site, m_pages[site].last());
	}

	clear();

	// Disable download buttons
	ui_buttonGetAll->setEnabled(false);
	ui_buttonGetPage->setEnabled(false);
	ui_buttonGetSel->setEnabled(false);

	// Disable pagination buttons
	ui_buttonNextPage->setEnabled(false);
	ui_buttonLastPage->setEnabled(false);

	// Get the search values
	const QString search = tags.join(" ");

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

	const bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	m_pageMergedMode = merged;
	if (merged)
		m_layouts.insert(nullptr, createImagesLayout(m_settings));

	loadPage();

	emit changed(this);
}

void SearchTab::endlessLoad()
{
	if (!m_endlessLoadingEnabled)
		return;

	const bool rememberPage = m_settings->value("infiniteScrollRememberPage", false).toBool();

	if (rememberPage)
		ui_spinPage->setValue(ui_spinPage->value() + 1);
	else
		m_endlessLoadOffset++;

	loadPage();
}

void SearchTab::loadPage()
{
	const bool merged = ui_checkMergeResults != nullptr && ui_checkMergeResults->isChecked();
	const int perpage = ui_spinImagesPerPage->value();
	const int currentPage = ui_spinPage->value() + m_endlessLoadOffset;
	const QStringList tags = m_lastTags.split(' ');
	setEndlessLoadingMode(false);

	for (Site *site : loadSites())
	{
		// Load results
		Page *page = new Page(m_profile, site, m_sites.values(), tags, currentPage, perpage, m_postFiltering->toPlainText().split(" ", QString::SkipEmptyParts), false, this, 0, m_lastPage, m_lastPageMinId, m_lastPageMaxId);
		connect(page, &Page::finishedLoading, this, &SearchTab::finishedLoading);
		connect(page, &Page::failedLoading, this, &SearchTab::failedLoading);
		connect(page, &Page::httpsRedirect, this, &SearchTab::httpsRedirect);

		// Keep pointer to the new page
		if (m_lastPages.contains(page->website()))
		{ page->setLastPage(m_lastPages[page->website()].data()); }
		if (!m_pages.contains(page->website()))
		{ m_pages.insert(page->website(), QList<QSharedPointer<Page>>()); }
		m_pages[page->website()].append(QSharedPointer<Page>(page));

		// Setup the layout
		if (!merged)
		{
			FixedSizeGridLayout *pageLayout = createImagesLayout(m_settings);
			m_layouts.insert(page, pageLayout);
			if (!m_siteLayouts.contains(site))
			{ m_siteLayouts.insert(site, new QVBoxLayout()); }
			m_siteLayouts[site]->addLayout(pageLayout);
		}

		// Load tags if necessary
		m_stop = false;
		if (m_settings->value("useregexfortags", true).toBool())
		{
			connect(page, &Page::finishedLoadingTags, this, &SearchTab::finishedLoadingTags);
			page->loadTags();
		}

		// Start loading
		page->load();
	}
	if (merged && !m_layouts.empty() && m_endlessLoadOffset == 0)
	{ addLayout(m_layouts[nullptr], 1, 0); }
	m_page = 0;

	if (merged && ui_progressMergeResults != nullptr)
	{
		ui_progressMergeResults->setValue(0);
		ui_progressMergeResults->setMaximum(m_pages.count());
	}
	if (ui_stackedMergeResults != nullptr)
	{ ui_stackedMergeResults->setCurrentIndex(merged ? 0 : 1); }
}

void SearchTab::addLayout(QLayout *layout, int row, int column)
{
	QWidget *layoutWidget = new QWidget;
	layoutWidget->setLayout(layout);
	ui_layoutResults->addWidget(layoutWidget, row, column);
}

FixedSizeGridLayout *SearchTab::createImagesLayout(QSettings *settings)
{
	const int hSpace = settings->value("Margins/horizontal", 6).toInt();
	const int vSpace = settings->value("Margins/vertical", 6).toInt();
	auto *l = new FixedSizeGridLayout(hSpace, vSpace);

	const bool fixedWidthLayout = settings->value("resultsFixedWidthLayout", false).toBool();
	if (fixedWidthLayout)
	{
		const int borderSize = settings->value("borders", 3).toInt();
		const qreal upscale = settings->value("thumbnailUpscale", 1.0).toDouble();
		l->setFixedWidth(qFloor(FIXED_IMAGE_WIDTH * upscale + borderSize * 2));
	}

	return l;
}


bool SearchTab::validateImage(const QSharedPointer<Image> &img, QString &error)
{
	QStringList detected = m_profile->getBlacklist().match(img->tokens(m_profile));
	if (!detected.isEmpty() && m_settings->value("hideblacklisted", false).toBool())
	{
		error = QStringLiteral("Image #%1 ignored. Reason: %2.").arg(img->id()).arg("\"" + detected.join(", ") + "\"");
		return false;
	}

	return true;
}

QList<Site*> SearchTab::loadSites() const
{ return m_selectedSources; }


void SearchTab::setSources(const QList<Site*> &sources)
{ m_selectedSources = sources; }
void SearchTab::toggleSource(const QString &url)
{
	Site *site = m_sites.value(url);

	const int removed = m_selectedSources.removeAll(site);
	if (removed == 0)
		m_selectedSources.append(site);
}
void SearchTab::setFavoriteImage(const QString &name)
{
	for (Favorite &fav : m_favorites)
	{
		if (fav.getName() == name)
		{
			fav.setImage(m_images.first()->previewImage());
			m_profile->emitFavorite();
		}
	}
}

QList<Site*> SearchTab::sources()
{ return m_selectedSources; }

const QList<Tag> &SearchTab::results() const
{ return m_tags; }
const QString &SearchTab::wiki() const
{ return m_wiki; }

void SearchTab::onLoad()
{ }


void SearchTab::firstPage()
{
	ui_spinPage->setValue(1);
	load();
}
void SearchTab::previousPage()
{
	if (ui_spinPage->value() > 1)
	{
		ui_spinPage->setValue(ui_spinPage->value() - 1);
		load();
	}
}
void SearchTab::nextPage()
{
	if (ui_spinPage->value() < ui_spinPage->maximum())
	{
		ui_spinPage->setValue(ui_spinPage->value() + 1);
		load();
	}
}
void SearchTab::lastPage()
{
	ui_spinPage->setValue(m_pagemax);
	load();
}

void SearchTab::setImagesPerPage(int ipp)
{ ui_spinImagesPerPage->setValue(ipp); }
void SearchTab::setColumns(int columns)
{ ui_spinColumns->setValue(columns); }
void SearchTab::setPostFilter(const QString &postfilter)
{ m_postFiltering->setText(postfilter); }

int SearchTab::imagesPerPage()
{ return ui_spinImagesPerPage->value(); }
int SearchTab::columns()
{ return ui_spinColumns->value(); }
QString SearchTab::postFilter()
{ return m_postFiltering->toPlainText(); }
