#include "pool-tab.h"
#include "ui_pool-tab.h"
#include <QJsonArray>
#include "ui/textedit.h"
#include "searchwindow.h"
#include "mainwindow.h"
#include "models/page.h"
#include "models/site.h"
#include "downloader/download-query-group.h"
#include "helpers.h"


poolTab::poolTab(QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(sites, profile, parent), ui(new Ui::poolTab)
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

	QStringList sources = m_sites->keys();
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
	emit(closed(this));
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
	sites.append(m_sites->value(ui->comboSites->currentText()));
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
	QStringList postFilters;
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	for (auto tag : jsonPostFilters)
		postFilters.append(tag.toString());
	setPostFilter(postFilters.join(' '));

	// Tags
	QStringList tags;
	QJsonArray jsonTags = json["tags"].toArray();
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
	Site *site = m_sites->value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, tags, ui->spinPage->value(), perPage, perPage, postFiltering, site));
}
void poolTab::getAll()
{
	auto page = m_pages[ui->comboSites->currentText()].first();

	QString tags = "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed();
	int limit = m_sites->value(ui->comboSites->currentText())->contains("Urls/1/Limit") ? m_sites->value(ui->comboSites->currentText())->value("Urls/1/Limit").toInt() : 0;
	int perpage = qMin((limit > 0 ? limit : 200), qMax(page->images().count(), page->imagesCount()));
	int total = qMax(page->images().count(), page->imagesCount());
	QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
	Site *site = m_sites->value(ui->comboSites->currentText());

	emit batchAddGroup(DownloadQueryGroup(m_settings, tags, 1, perpage, total, postFiltering, site));
}


void poolTab::setTags(QString tags, bool preload)
{
	activateWindow();
	m_search->setText(tags);

	if (preload)
		load();
	else
		updateTitle();
}
void poolTab::setPool(int id, QString site)
{
	activateWindow();
	ui->spinPool->setValue(id);
	int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
	load();
}
void poolTab::setSite(QString site)
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
	setWindowTitle("Pool #" + QString::number(ui->spinPool->value()) + (search.isEmpty() ? "" : " - " + QString(search).replace("&", "&&")));
	emit titleChanged(this);
}
