#include "tabs/pool-tab.h"
#include <QJsonArray>
#include <ui_pool-tab.h>
#include "downloader/download-query-group.h"
#include "helpers.h"
#include "mainwindow.h"
#include "models/page.h"
#include "models/site.h"
#include "searchwindow.h"
#include "ui/textedit.h"


poolTab::poolTab(Profile *profile, mainWindow *parent)
	: searchTab(profile, parent), ui(new Ui::poolTab)
{
	ui->setupUi(this);
	ui->widgetMeant->hide();

	// UI members for SearchTab class
	ui_checkMergeResults = nullptr;
	ui_progressMergeResults = nullptr;
	ui_stackedMergeResults = nullptr;
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
	ui_buttonEndlessLoad = nullptr;
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

poolTab::~poolTab()
{
	close();
	delete ui;
}

void poolTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), m_profile, this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void poolTab::closeEvent(QCloseEvent *e)
{
	emit closed(this);
	e->accept();
}


void poolTab::load()
{
	updateTitle();

	// Get the search values
	QString search = m_search->toPlainText();
	QStringList tags = search.trimmed().split(" ", QString::SkipEmptyParts);
	tags.prepend("pool:"+QString::number(ui->spinPool->value()));

	loadTags(tags);
}

QList<Site*> poolTab::loadSites() const
{
	QList<Site*> sites;
	sites.append(m_sites.value(ui->comboSites->currentText()));
	return sites;
}

void poolTab::write(QJsonObject &json) const
{
	json["type"] = QString("pool");
	json["pool"] = ui->spinPool->value();
	json["site"] = ui->comboSites->currentText();
	json["tags"] = QJsonArray::fromStringList(m_search->toPlainText().split(' ', QString::SkipEmptyParts));
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts));
}

bool poolTab::read(const QJsonObject &json, bool preload)
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


void poolTab::getPage()
{
	auto page = m_pages[ui->comboSites->currentText()].first();

	bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	int perPage = unloaded ? ui->spinImagesPerPage->value() : page->images().count();
	QString tags = "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed();
	QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, tags, ui->spinPage->value(), perPage, perPage, postFiltering, site));
}
void poolTab::getAll()
{
	QSharedPointer<Page> page = m_pages[ui->comboSites->currentText()].first();

	int highLimit = page->highLimit();
	int currentCount = page->images().count();
	int imageCount = page->imagesCount();
	int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
	int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
	if (perPage == 0 && total == 0)
		return;

	QString search = "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed();
	QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites.value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, site));
}


void poolTab::setTags(const QString &tags, bool preload)
{
	activateWindow();
	m_search->setText(tags);

	if (preload)
		load();
	else
		updateTitle();
}
void poolTab::setPool(int id, const QString &site)
{
	activateWindow();
	ui->spinPool->setValue(id);
	int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
	load();
}
void poolTab::setSite(const QString &site)
{
	int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
}

void poolTab::focusSearch()
{
	ui->spinPool->focusWidget();
}

QString poolTab::tags() const
{ return m_search->toPlainText(); }


void poolTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void poolTab::updateTitle()
{
	QString search = m_search->toPlainText().trimmed();
	setWindowTitle("Pool #" + QString::number(ui->spinPool->value()) + (search.isEmpty() ? QString() : " - " + QString(search).replace("&", "&&")));
	emit titleChanged(this);
}
