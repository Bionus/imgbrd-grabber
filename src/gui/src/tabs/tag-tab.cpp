#include "tabs/tag-tab.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QRegularExpression>
#include <QSettings>
#include <ui_tag-tab.h>
#include "downloader/download-query-group.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "monitoring/monitor.h"
#include "monitoring/monitor-manager.h"
#include "search-window.h"
#include "ui/text-edit.h"

#define MONITOR_IMAGES_WARNING 10000


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
	ui->layoutFields->insertWidget(2, m_search, 1);
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);
	connect(ui->labelMeant, SIGNAL(linkActivated(QString)), this, SLOT(setTags(QString)));

	// Others
	ui->checkMergeResults->setChecked(m_settings->value("mergeresults", false).toBool());
	optionsChanged();
	ui->widgetPlus->hide();
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

void TagTab::closeEvent(QCloseEvent *event)
{
	m_settings->setValue("mergeresults", ui->checkMergeResults->isChecked());
	m_settings->sync();

	emit closed(this);
    event->accept();
}


void TagTab::load()
{
	updateTitle();

	QString search = m_search->toPlainText().trimmed();

	// Search an image directly by typing its MD5
	if (m_settings->value("enable_md5_fast_search", true).toBool()) {
		static const QRegularExpression md5Matcher("^[0-9A-F]{32}$", QRegularExpression::CaseInsensitiveOption);
		if (md5Matcher.match(search).hasMatch()) {
			search.prepend("md5:");
		}
	}

	const QStringList tags = search.split(" ", Qt::SkipEmptyParts);
	if (ui->comboEndpoint->isVisible() && !ui->comboEndpoint->currentData().toString().isEmpty()) {
		const QString endpoint = ui->comboEndpoint->currentData().toString();
		const QMap<QString, QVariant> input {{ "search", search }};
		loadTags(SearchQuery(endpoint, input));
	} else {
		loadTags(tags);
	}
}

void TagTab::write(QJsonObject &json) const
{
	json["type"] = QStringLiteral("tag");
	json["endpoint"] = ui->comboEndpoint->currentData().toString();
	json["tags"] = QJsonArray::fromStringList(m_search->toPlainText().split(' ', Qt::SkipEmptyParts));
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(postFilter());
	json["mergeResults"] = ui->checkMergeResults->isChecked();
	json["isLocked"] = isLocked();

	// Last urls
	QJsonObject lastUrls;
	for (auto it = m_pages.constBegin(); it != m_pages.constEnd(); ++it) {
		const QString &site = it.key();
		if (!it.value().isEmpty()) {
			QJsonObject siteUrls;
			const auto urls = it.value().last()->urls();
			for (auto urlIt = urls.constBegin(); urlIt != urls.constEnd(); ++urlIt) {
				siteUrls.insert(urlIt.key(), urlIt.value().toString());
			}
			lastUrls.insert(site, siteUrls);
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
	for (const QString &site : jsonLastUrls.keys()) {
		QJsonObject siteLastUrls = jsonLastUrls[site].toObject();
		QMap<QString, QString> siteUrls;
		for (const QString &api : siteLastUrls.keys()) {
			siteUrls[api] = siteLastUrls[api].toString();
		}
		m_lastUrls[site] = siteUrls;
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

	// Endpoint
	const QString endpoint = json["endpoint"].toString();
	const int endpointIndex = ui->comboEndpoint->findData(endpoint);
	if (endpointIndex >= 0) {
		ui->comboEndpoint->setCurrentIndex(endpointIndex);
	}

	// Tags
	QJsonArray jsonTags = json["tags"].toArray();
	QStringList tags;
	tags.reserve(jsonTags.count());
	for (auto tag : jsonTags) {
		tags.append(tag.toString());
	}
	setTags(tags.join(' '), preload);

	setLocked(json["isLocked"].toBool());

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
		const int total = imageCount > 0 ? qMax(currentCount, imageCount) : -1;
		const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
		if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0)) {
			continue;
		}

		const QStringList postFiltering = postFilter(true);
		emit batchAddGroup(DownloadQueryGroup(m_settings, page->search(), 1, perPage, total, postFiltering, page->site()));
	}
}
void TagTab::monitor()
{
	const QString search = m_search->toPlainText().trimmed();

	int totalCount = 0;
	for (const auto &page : getPagesToDownload()) {
		totalCount += page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
	}

	// Warn in case of adding huge monitors
	if (search.isEmpty()) {
		const int response = QMessageBox::warning(this, tr("Monitoring an empty search"), tr("You are about to add a monitor for an empty search, which can lead to a lots of results. Are you sure?"), QMessageBox::Yes | QMessageBox::No);
		if (response != QMessageBox::Yes) {
			return;
		}
	} else if (totalCount >= MONITOR_IMAGES_WARNING) {
		const int response = QMessageBox::warning(this, tr("Monitoring a big search"), tr("You are about to add a monitor for a search with a lot of results (%1). Are you sure?").arg(totalCount), QMessageBox::Yes | QMessageBox::No);
		if (response != QMessageBox::Yes) {
			return;
		}
	}

	const QStringList tags = search.split(" ", Qt::SkipEmptyParts);
	QList<Site*> sites = m_settings->value("Monitoring/emptySources", false).toBool() ? loadSites() : sourcesWithResults();
	Monitor monitor(m_settings, sites, tags, postFilter());
	m_profile->monitorManager()->add(monitor);
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

#include "models/api/api.h"
#include "models/api/api-endpoint.h"
void TagTab::setSources(const QList<Site *> &sources)
{
	SearchTab::setSources(sources);

	// Generate the list of all common endpoints across the selected sources
	bool init = false;
	QSet<QString> commonEndpoints;
	QMap<QString, QString> endpointNames;
	for (const Site *site : m_selectedSources) {
		// Merge all endpoints for that given source
		QMap<QString, ApiEndpoint*> sourceEndpoints;
		for (const Api *api : site->getApis()) {
			sourceEndpoints.insert(api->endpoints());
		}

		// Intersect all endpoints among sources
		const auto &keysList = sourceEndpoints.keys();
		const auto keys = QSet<QString>(keysList.begin(), keysList.end());
		if (!init) {
			commonEndpoints = keys;
			for (const QString &key : keys) {
				endpointNames.insert(key, sourceEndpoints[key]->name());
			}
			init = true;
		} else {
			commonEndpoints = commonEndpoints.intersect(keys);
		}
	}

	// Only display endpoints having a name
	QSet<QString> commonEndpointsWithName;
	for (const QString &endpoint : commonEndpoints) {
		if (!endpointNames[endpoint].isEmpty()) {
			commonEndpointsWithName.insert(endpoint);
		}
	}

	// Update the endpoint selector
	if (commonEndpointsWithName.isEmpty()) {
		ui->comboEndpoint->hide();
	} else {
		ui->comboEndpoint->show();
		ui->comboEndpoint->clear();
		if (!commonEndpointsWithName.contains("search")) {
			ui->comboEndpoint->addItem("Search", "");
		}
		for (const QString &key : commonEndpointsWithName) {
			ui->comboEndpoint->addItem(endpointNames[key], key);
		}
	}
}
