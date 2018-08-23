#include "tabs/gallery-tab.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <ui_gallery-tab.h>
#include "downloader/download-query-group.h"
#include "logger.h"
#include "main-window.h"
#include "models/page.h"
#include "models/site.h"
#include "search-window.h"
#include "ui/textedit.h"

#define MAX_TAB_NAME_LENGTH 40


GalleryTab::GalleryTab(Site *site, QString name, QUrl url, Profile *profile, MainWindow *parent)
	: SearchTab(profile, parent), ui(new Ui::GalleryTab), m_site(site), m_name(std::move(name)), m_url(std::move(url))
{
	ui->setupUi(this);
	ui->labelGalleryName->setText(m_name);

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

	load();
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
	loadTags(QStringList() << "gallery:" + m_url.toString());
}

QList<Site*> GalleryTab::loadSites() const
{
	QList<Site*> sites;
	sites.append(m_site);
	return sites;
}

void GalleryTab::write(QJsonObject &json) const
{
	json["type"] = QStringLiteral("gallery");
	json["name"] = m_name;
	json["url"] = m_url.toString();
	json["site"] = m_site->url();
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts));
}

bool GalleryTab::read(const QJsonObject &json, bool preload)
{
	m_name = json["name"].toString();
	m_url = QUrl(json["url"].toString());
	m_site = m_sites[json["site"].toString()];

	ui->labelGalleryName->setText(m_name);
	ui->spinPage->setValue(json["page"].toInt());
	ui->spinImagesPerPage->setValue(json["perpage"].toInt());
	ui->spinColumns->setValue(json["columns"].toInt());

	// Post filtering
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	QStringList postFilters;
	postFilters.reserve(jsonPostFilters.count());
	for (auto tag : jsonPostFilters)
		postFilters.append(tag.toString());
	setPostFilter(postFilters.join(' '));

	// Tags
	QJsonArray jsonTags = json["tags"].toArray();
	QStringList tags;
	tags.reserve(jsonTags.count());
	for (auto tag : jsonTags)
		tags.append(tag.toString());
	setTags(tags.join(' '), preload);

	return true;
}


// TODO(Bionus)
void GalleryTab::getPage()
{
	log("Not implemented yet", Logger::Warning);

	/*const auto &page = m_pages[ui->comboSites->currentText()].first();

	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	const int perPage = unloaded ? ui->spinImagesPerPage->value() : page->pageImageCount();
	const QString tags = "pool:" + QString::number(ui->spinPool->value()) + " " + m_search->toPlainText() + " " + m_settings->value("add").toString().trimmed();
	const QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, tags, ui->spinPage->value(), perPage, perPage, postFiltering, site));*/
}
// TODO(Bionus)
void GalleryTab::getAll()
{
	log("Not implemented yet", Logger::Warning);

	/*const auto &page = m_pages[ui->comboSites->currentText()].first();

	const int highLimit = page->highLimit();
	const int currentCount = page->pageImageCount();
	const int imageCount = page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
	const int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
	const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
	if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0))
		return;

	const QString search = "pool:" + QString::number(ui->spinPool->value()) + " " + m_search->toPlainText() + " " + m_settings->value("add").toString().trimmed();
	const QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, site));*/
}


// TODO(Bionus)
void GalleryTab::setTags(const QString &tags, bool preload)
{
	activateWindow();
	// m_search->setText(tags);

	if (preload)
		load();
	else
		updateTitle();
}

void GalleryTab::focusSearch()
{
	/* Nothing to focus */
}

QString GalleryTab::tags() const
{ return "gallery:" + m_url.toString(); }


void GalleryTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void GalleryTab::updateTitle()
{
	setWindowTitle(m_name.length() > MAX_TAB_NAME_LENGTH ? m_name.left(MAX_TAB_NAME_LENGTH - 3) + "..." : m_name);
	emit titleChanged(this);
}
