#include "tabs/pool-tab.h"
#include <QCloseEvent>
#include <QJsonArray>
#include <QSettings>
#include <ui_pool-tab.h>
#include "downloader/download-query-group.h"
#include "main-window.h"
#include "models/page.h"
#include "models/site.h"
#include "search-window.h"
#include "ui/text-edit.h"


PoolTab::PoolTab(Profile *profile, MainWindow *parent)
	: SearchTab(profile, parent), ui(new Ui::PoolTab)
{
	ui->setupUi(this);
	ui->widgetMeant->hide();

	// UI members for SearchTab class
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
	ui_scrollAreaResults = ui->scrollAreaResults;

	QStringList sources = m_sites.keys();
	for (const QString &source : sources)
	{ ui->comboSites->addItem(source); }

	// Search field
	m_search = createAutocomplete();
	m_postFiltering = createAutocomplete();
	ui->layoutFields->insertWidget(3, m_search, 1);
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);
	connect(ui->labelMeant, SIGNAL(linkActivated(QString)), this, SLOT(setTags(QString)));

	// Others
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	updateCheckboxes();
	m_search->setFocus();
}

PoolTab::~PoolTab()
{
	close();
	delete ui;
}

void PoolTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), m_profile, this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void PoolTab::closeEvent(QCloseEvent *e)
{
	emit closed(this);
	e->accept();
}


void PoolTab::load()
{
	updateTitle();

	// Get the search values
	QString search = m_search->toPlainText();
	QStringList tags = search.trimmed().split(" ", QString::SkipEmptyParts);
	tags.prepend("pool:" + QString::number(ui->spinPool->value()));

	loadTags(tags);
}

QList<Site*> PoolTab::loadSites() const
{
	QList<Site*> sites;
	sites.append(m_sites.value(ui->comboSites->currentText()));
	return sites;
}

void PoolTab::write(QJsonObject &json) const
{
	json["type"] = QStringLiteral("pool");
	json["pool"] = ui->spinPool->value();
	json["site"] = ui->comboSites->currentText();
	json["tags"] = QJsonArray::fromStringList(m_search->toPlainText().split(' ', QString::SkipEmptyParts));
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts));
}

bool PoolTab::read(const QJsonObject &json, bool preload)
{
	ui->spinPool->setValue(json["pool"].toInt());
	ui->comboSites->setCurrentText(json["site"].toString());
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


void PoolTab::getPage()
{
	const auto &page = m_pages[ui->comboSites->currentText()].first();

	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	const int perPage = unloaded ? ui->spinImagesPerPage->value() : page->pageImageCount();
	const QString tags = "pool:" + QString::number(ui->spinPool->value()) + " " + m_search->toPlainText() + " " + m_settings->value("add").toString().trimmed();
	const QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, tags, ui->spinPage->value(), perPage, perPage, postFiltering, site));
}
void PoolTab::getAll()
{
	const auto &page = m_pages[ui->comboSites->currentText()].first();

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

	emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, site));
}


void PoolTab::setTags(const QString &tags, bool preload)
{
	m_search->setText(tags);

	if (preload) {
		activateWindow();
		load();
	} else {
		updateTitle();
	}
}
void PoolTab::setPool(int id, const QString &site)
{
	activateWindow();
	ui->spinPool->setValue(id);
	const int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
	load();
}
void PoolTab::setSite(const QString &site)
{
	const int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
}

void PoolTab::focusSearch()
{
	ui->spinPool->setFocus();
}

QString PoolTab::tags() const
{ return m_search->toPlainText(); }


void PoolTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void PoolTab::updateTitle()
{
	QString search = m_search->toPlainText().trimmed();
	setWindowTitle("Pool #" + QString::number(ui->spinPool->value()) + (search.isEmpty() ? QString() : " - " + QString(search).replace("&", "&&")));
	emit titleChanged(this);
}
