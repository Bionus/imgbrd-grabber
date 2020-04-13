#include "tabs/gallery-tab.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <QSettings>
#include <ui_gallery-tab.h>
#include "downloader/download-query-group.h"
#include "logger.h"
#include "main-window.h"
#include "models/image.h"
#include "models/monitor.h"
#include "models/monitor-manager.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "search-window.h"
#include "ui/text-edit.h"

#define MAX_TAB_NAME_LENGTH 40


GalleryTab::GalleryTab(Site *site, QSharedPointer<Image> gallery, Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent)
	: GalleryTab(profile, downloadQueue, parent)
{
	m_site = site;
	m_gallery = std::move(gallery);

	ui->labelGalleryName->setText(m_gallery->name());

	load();
}

GalleryTab::GalleryTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent)
	: SearchTab(profile, downloadQueue, parent, "Gallery"), ui(new Ui::GalleryTab)
{
	ui->setupUi(this);

	// UI members for SearchTab class
	ui_spinPage = ui->spinPage;
	ui_spinImagesPerPage = ui->spinImagesPerPage;
	ui_spinColumns = ui->spinColumns;
	ui_layoutResults = ui->layoutResults;
	ui_buttonHistoryBack = ui->buttonHistoryBack;
	ui_buttonHistoryNext = ui->buttonHistoryNext;
	ui_buttonNextPage = ui->buttonNextPage;
	ui_buttonLastPage = ui->buttonLastPage;
	ui_buttonGetAll = ui->buttonGetAll;
	ui_buttonGetPage = ui->buttonGetpage;
	ui_buttonGetSel = ui->buttonGetSel;
	ui_buttonFirstPage = ui->buttonFirstPage;
	ui_buttonPreviousPage = ui->buttonPreviousPage;
	ui_scrollAreaResults = ui->scrollAreaResults;

	// Post-filtering
	m_postFiltering = createAutocomplete();
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);

	// Others
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
}

GalleryTab::~GalleryTab()
{
	close();
	delete ui;
}

void GalleryTab::closeEvent(QCloseEvent *e)
{
	emit closed(this);
	e->accept();
}


void GalleryTab::load()
{
	updateTitle();
	loadTags(m_gallery);
}

QList<Site*> GalleryTab::loadSites() const
{
	QList<Site*> sites;
	sites.append(m_site);
	return sites;
}

void GalleryTab::write(QJsonObject &json) const
{
	QJsonObject jsonGallery;
	m_gallery->write(jsonGallery);
	json["gallery"] = jsonGallery;

	json["type"] = QStringLiteral("gallery");
	json["site"] = m_site->url();
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(postFilter());
}

bool GalleryTab::read(const QJsonObject &json, bool preload)
{
	const QString site = json["site"].toString();
	if (!m_sites.contains(site)) {
		return false;
	}
	m_site = m_sites[site];

	m_gallery = QSharedPointer<Image>(new Image(m_profile));
	if (!m_gallery->read(json["gallery"].toObject(), m_profile->getSites())) {
		m_gallery->deleteLater();
		return false;
	}

	ui->labelGalleryName->setText(m_gallery->name());
	ui->spinPage->setValue(json["page"].toInt());
	ui->spinImagesPerPage->setValue(json["perpage"].toInt());
	ui->spinColumns->setValue(json["columns"].toInt());

	// Post filtering
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	QStringList postFilters;
	postFilters.reserve(jsonPostFilters.count());
	for (auto tag : jsonPostFilters) {
		postFilters.append(tag.toString());
	}
	setPostFilter(postFilters);

	setTags("", preload);
	return true;
}


void GalleryTab::getPage()
{
	if (!m_pages.contains(m_site->url())) {
		return;
	}

	const auto &page = m_pages[m_site->url()].first();

	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	const int perPage = unloaded ? ui->spinImagesPerPage->value() : page->pageImageCount();
	const QStringList postFiltering = postFilter(true);

	emit batchAddGroup(DownloadQueryGroup(m_settings, m_gallery, ui->spinPage->value(), perPage, perPage, postFiltering, m_site));
}
void GalleryTab::getAll()
{
	if (!m_pages.contains(m_site->url())) {
		return;
	}

	const auto &page = m_pages[m_site->url()].first();

	const int highLimit = page->highLimit();
	const int currentCount = page->pageImageCount();
	const int imageCount = page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
	const int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
	const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
	if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0)) {
		return;
	}

	const QStringList postFiltering = postFilter(true);
	emit batchAddGroup(DownloadQueryGroup(m_settings, m_gallery, 1, perPage, total, postFiltering, m_site));
}

void GalleryTab::monitor()
{
	Monitor monitor(loadSites().first(), 24 * 60 * 60, QDateTime(), true, QString(), QString(), 0, true, m_gallery, postFilter());
	m_profile->monitorManager()->add(monitor);
}


void GalleryTab::setTags(const QString &tags, bool preload)
{
	Q_UNUSED(tags)

	if (preload) {
		activateWindow();
		load();
	} else {
		updateTitle();
	}
}

void GalleryTab::focusSearch()
{
	/* Nothing to focus */
}

QString GalleryTab::tags() const
{ return QString(); }


void GalleryTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void GalleryTab::updateTitle()
{
	const QString &name = m_gallery->name();
	setWindowTitle(name.length() > MAX_TAB_NAME_LENGTH ? name.left(MAX_TAB_NAME_LENGTH - 3) + "..." : name);
	emit titleChanged(this);
}
