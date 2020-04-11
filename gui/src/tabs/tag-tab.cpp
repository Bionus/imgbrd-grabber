#include "tabs/tag-tab.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <QSettings>
#include <ui_tag-tab.h>
#include "downloader/download-query-group.h"
#include "models/page.h"
#include "models/site.h"
#include "search-window.h"
#include "ui/text-edit.h"


TagTab::TagTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent)
	: SearchTab(profile, downloadQueue, parent, "Tag"), ui(new Ui::TagTab)
{
	ui->setupUi(this);
	ui->widgetMeant->hide();

	// UI members for SearchTab class
	ui_checkMergeResults = ui->checkMergeResults;
	ui_progressMergeResults = ui->progressMergeResults;
	ui_stackedMergeResults = ui->stackedMergeResults;
	ui_spinPage = ui->spinPage;
	ui_spinImagesPerPage = ui->spinImagesPerPage;
	ui_spinColumns = ui->spinColumns;
	ui_widgetMeant = ui->widgetMeant;
	ui_labelMeant = ui->labelMeant;
	ui_layoutResults = ui->layoutResults;
	ui_layoutSourcesList = ui->layoutSourcesList;
	ui_buttonHistoryBack = ui->buttonHistoryBack;
	ui_buttonHistoryNext = ui->buttonHistoryNext;
	ui_buttonNextPage = ui->buttonNextPage;
	ui_buttonLastPage = ui->buttonLastPage;
	ui_buttonGetAll = ui->buttonGetAll;
	ui_buttonGetPage = ui->buttonGetpage;
	ui_buttonGetSel = ui->buttonGetSel;
	ui_buttonFirstPage = ui->buttonFirstPage;
	ui_buttonPreviousPage = ui->buttonPreviousPage;
	ui_buttonEndlessLoad = ui->buttonEndlessLoad;
	ui_scrollAreaResults = ui->scrollAreaResults;

	// Search fields
	m_search = createAutocomplete();
	m_postFiltering = createAutocomplete();
	ui->layoutFields->insertWidget(1, m_search, 1);
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);
	connect(ui->labelMeant, SIGNAL(linkActivated(QString)), this, SLOT(setTags(QString)));

	// Others
	ui->checkMergeResults->setChecked(m_settings->value("mergeresults", false).toBool());
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	updateCheckboxes();
	m_search->setFocus();

	init();
}

TagTab::~TagTab()
{
	close();
	delete ui;
}

void TagTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), m_profile, this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void TagTab::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("mergeresults", ui->checkMergeResults->isChecked());
	m_settings->sync();

	emit closed(this);
	e->accept();
}


void TagTab::load()
{
	updateTitle();

	QString search = m_search->toPlainText().trimmed();

	// Search an image directly by typing its MD5
	if (m_settings->value("enable_md5_fast_search", true).toBool()) {
		static QRegularExpression md5Matcher("^[0-9A-F]{32}$", QRegularExpression::CaseInsensitiveOption);
		if (md5Matcher.match(search).hasMatch()) {
			search.prepend("md5:");
		}
	}

	const QStringList tags = search.split(" ", QString::SkipEmptyParts);
	loadTags(tags);
}

void TagTab::write(QJsonObject &json) const
{
	json["type"] = QStringLiteral("tag");
	json["tags"] = QJsonArray::fromStringList(m_search->toPlainText().split(' ', QString::SkipEmptyParts));
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(postFilter());
	json["mergeResults"] = ui->checkMergeResults->isChecked();

	// Last urls
	QJsonObject lastUrls;
	for (const QString &site : m_pages.keys()) {
		if (!m_pages[site].isEmpty()) {
			lastUrls.insert(site, m_pages[site].last()->url().toString());
		}
	}
	json["lastUrls"] = lastUrls;

	// Sites
	QJsonArray sites;
	for (Site *site : loadSites()) {
		sites.append(site->url());
	}
	json["sites"] = sites;
}

bool TagTab::read(const QJsonObject &json, bool preload)
{
	ui->spinPage->setValue(json["page"].toInt());
	ui->spinImagesPerPage->setValue(json["perpage"].toInt());
	ui->spinColumns->setValue(json["columns"].toInt());
	ui->checkMergeResults->setChecked(json["mergeResults"].toBool());

	// Last urls
	QJsonObject jsonLastUrls = json["lastUrls"].toObject();
	for (const QString &key : jsonLastUrls.keys()) {
		m_lastUrls[key] = jsonLastUrls[key].toString();
	}

	// Post filtering
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	QStringList postFilters;
	postFilters.reserve(jsonPostFilters.count());
	for (auto tag : jsonPostFilters) {
		postFilters.append(tag.toString());
	}
	setPostFilter(postFilters);

	// Sources
	QJsonArray jsonSelectedSources = json["sites"].toArray();
	QStringList selectedSources;
	selectedSources.reserve(jsonSelectedSources.count());
	for (auto site : jsonSelectedSources) {
		selectedSources.append(site.toString());
	}
	QList<Site*> selectedSourcesObj;
	for (Site *site : m_sites) {
		if (selectedSources.contains(site->url())) {
			selectedSourcesObj.append(site);
		}
	}
	saveSources(selectedSourcesObj, false);

	// Tags
	QJsonArray jsonTags = json["tags"].toArray();
	QStringList tags;
	tags.reserve(jsonTags.count());
	for (auto tag : jsonTags) {
		tags.append(tag.toString());
	}
	setTags(tags.join(' '), preload);

	return true;
}


void TagTab::setTags(const QString &tags, bool preload)
{
	m_search->setText(tags);

	if (preload) {
		activateWindow();
		load();
	} else {
		updateTitle();
	}
}

void TagTab::getPage()
{
	if (m_pages.empty()) {
		return;
	}

	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();

	QList<QSharedPointer<Page>> pages = this->getPagesToDownload();
	for (const QSharedPointer<Page> &page : pages) {
		const int perPage = unloaded ? ui->spinImagesPerPage->value() : (page->pageImageCount() > ui->spinImagesPerPage->value() ? page->pageImageCount() : ui->spinImagesPerPage->value());
		if (perPage <= 0 || page->pageImageCount() <= 0) {
			continue;
		}

		const QStringList postFiltering = postFilter(true);
		emit batchAddGroup(DownloadQueryGroup(m_settings, page->search(), ui->spinPage->value(), perPage, perPage, postFiltering, page->site()));
	}
}
void TagTab::getAll()
{
	if (m_pages.empty()) {
		return;
	}

	QList<QSharedPointer<Page>> pages = this->getPagesToDownload();
	for (const QSharedPointer<Page> &page : pages) {
		const int highLimit = page->highLimit();
		const int currentCount = page->pageImageCount();
		const int imageCount = page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
		const int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
		const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
		if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0)) {
			continue;
		}

		const QStringList postFiltering = postFilter(true);
		emit batchAddGroup(DownloadQueryGroup(m_settings, page->search(), 1, perPage, total, postFiltering, page->site()));
	}
}


void TagTab::focusSearch()
{
	m_search->setFocus();
}

QString TagTab::tags() const
{ return m_search->toPlainText(); }


void TagTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
		updateTitle();
	}

	QWidget::changeEvent(event);
}

void TagTab::updateTitle()
{
	QString search = m_search->toPlainText().trimmed();
	setWindowTitle(search.isEmpty() ? tr("Search") : QString(search).replace("&", "&&"));
	emit titleChanged(this);
}
