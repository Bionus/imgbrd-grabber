#include "tag-tab.h"
#include "ui_tag-tab.h"
#include <QJsonArray>
#include "ui/textedit.h"
#include "models/page.h"
#include "models/site.h"
#include "downloader/download-query-group.h"
#include "searchwindow.h"
#include "helpers.h"


tagTab::tagTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(sites, profile, parent), ui(new Ui::tagTab)
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

tagTab::~tagTab()
{
	close();
	delete ui;
}

void tagTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), m_profile, this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void tagTab::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("mergeresults", ui->checkMergeResults->isChecked());
	m_settings->sync();

	emit closed(this);
	e->accept();
}


void tagTab::load()
{
	// Get the search values
	QString search = m_search->toPlainText().trimmed();
	if (m_settings->value("enable_md5_fast_search", true).toBool())
	{
		static QRegularExpression md5Matcher("^[0-9A-F]{32}$", QRegularExpression::CaseInsensitiveOption);
		if (md5Matcher.match(search).hasMatch())
			search.prepend("md5:");
	}
	QStringList tags = search.split(" ", QString::SkipEmptyParts);

	setWindowTitle(search.isEmpty() ? tr("Search") : QString(search).replace("&", "&&"));
	emit titleChanged(this);

	loadTags(tags);
}

void tagTab::write(QJsonObject &json) const
{
	json["type"] = QString("tag");
	json["tags"] = QJsonArray::fromStringList(m_search->toPlainText().split(' ', QString::SkipEmptyParts));
	json["page"] = ui->spinPage->value();
	json["perpage"] = ui->spinImagesPerPage->value();
	json["columns"] = ui->spinColumns->value();
	json["postFiltering"] = QJsonArray::fromStringList(m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts));
	json["mergeResults"] = ui->checkMergeResults->isChecked();

	// Sites
	QJsonArray sites;
	for (Site *site : loadSites())
		sites.append(site->url());
	json["sites"] = sites;
}

bool tagTab::read(const QJsonObject &json)
{
	ui->spinPage->setValue(json["page"].toInt());
	ui->spinImagesPerPage->setValue(json["perpage"].toInt());
	ui->spinColumns->setValue(json["columns"].toInt());
	ui->checkMergeResults->setChecked(json["mergeResults"].toBool());

	// Post filtering
	QStringList postFilters;
	QJsonArray jsonPostFilters = json["postFiltering"].toArray();
	for (auto tag : jsonPostFilters)
		postFilters.append(tag.toString());
	setPostFilter(postFilters.join(' '));

	// Sources
	QStringList selectedSources;
	QJsonArray jsonSelectedSources = json["sites"].toArray();
	for (auto site : jsonSelectedSources)
		selectedSources.append(site.toString());
	QList<bool> selectedSourcesBool;
	for (Site *site : *m_sites)
		selectedSourcesBool.append(selectedSources.contains(site->url()));
	saveSources(selectedSourcesBool, false);

	// Tags
	QStringList tags;
	QJsonArray jsonTags = json["tags"].toArray();
	for (auto tag : jsonTags)
		tags.append(tag.toString());
	setTags(tags.join(' '));

	return true;
}


void tagTab::setTags(QString tags)
{
	activateWindow();
	m_search->setText(tags);
	load();
}

void tagTab::getPage()
{
	if (m_pages.empty())
		return;

	QStringList actuals, keys = m_sites->keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{ actuals.append(keys.at(i)); }
	}
	bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	for (int i = 0; i < actuals.count(); i++)
	{
		if (m_pages.contains(actuals[i]))
		{
			auto page = m_pages[actuals[i]].first();

			int perpage = unloaded ? ui->spinImagesPerPage->value() : (page->images().count() > ui->spinImagesPerPage->value() ? page->images().count() : ui->spinImagesPerPage->value());
			if (perpage <= 0 || page->images().count() <= 0)
				continue;

			QString search = page->search().join(' ');
			QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
			emit batchAddGroup(DownloadQueryGroup(m_settings, search, ui->spinPage->value(), perpage, perpage, postFiltering, m_sites->value(actuals.at(i))));
		}
	}
}
void tagTab::getAll()
{
	if (m_pages.empty())
		return;

	QStringList actuals, keys = m_sites->keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
			actuals.append(keys.at(i));
	}

	for (QString actual : actuals)
	{
		Page *page = m_pages[actual].first();

		int highLimit = page->highLimit();
		int currentCount = page->images().count();
		int total = qMax(currentCount, page->imagesCount());
		int perPage = highLimit > 0 ? qMin(highLimit, total) : currentCount;
		if (perPage == 0 && total == 0)
			continue;

		QString search = page->search().join(' ');
		QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
		emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, m_sites->value(actual)));
	}
}


void tagTab::focusSearch()
{
	m_search->setFocus();
}

QString tagTab::tags() const
{ return m_search->toPlainText(); }


void tagTab::changeEvent(QEvent *event)
{
	// Automatically retranslate this tab on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}
