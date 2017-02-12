#include <QMessageBox>
#include <QMenu>
#include "tag-tab.h"
#include "ui_tag-tab.h"
#include "viewer/zoomwindow.h"
#include "searchwindow.h"
#include "helpers.h"


tagTab::tagTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(sites, profile, parent), ui(new Ui::tagTab), m_sized(false)
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

	emit closed(this);
	e->accept();
}


void tagTab::load()
{
	// Get the search values
	QString search = m_search->toPlainText().trimmed();
	if (m_settings->value("enable_md5_fast_search", true).toBool())
	{
		QRegExp md5Matcher("^[0-9A-F]{32}$", Qt::CaseInsensitive);
		if (md5Matcher.exactMatch(search))
			search.prepend("md5:");
	}
	QStringList tags = search.split(" ", QString::SkipEmptyParts);

	setWindowTitle(search.isEmpty() ? tr("Search") : QString(search).replace("&", "&&"));
	emit titleChanged(this);

	loadTags(tags);
}

QList<Site*> tagTab::loadSites()
{
	QList<Site*> sites;
	for (int i = 0; i < m_selectedSources.size(); i++)
		if (m_checkboxes.at(i)->isChecked())
			sites.append(m_sites->value(m_sites->keys().at(i)));
	return sites;
}

bool tagTab::validateImage(QSharedPointer<Image> img)
{
	Q_UNUSED(img);
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
		if (m_pages.contains(actuals.at(i)))
		{
			int perpage = unloaded ? ui->spinImagesPerPage->value() : (m_pages.value(actuals.at(i))->images().count() > ui->spinImagesPerPage->value() ? m_pages.value(actuals.at(i))->images().count() : ui->spinImagesPerPage->value());
			if (perpage <= 0 || m_pages.value(actuals.at(i))->images().count() <= 0)
				continue;

			QString search = m_pages.value(actuals.at(i))->search().join(' ');
			emit batchAddGroup(QStringList()
							   << search
							   << QString::number(ui->spinPage->value())
							   << QString::number(perpage)
							   << QString::number(perpage)
							   << m_settings->value("downloadblacklist").toString()
							   << actuals.at(i)
							   << m_settings->value("Save/filename").toString()
							   << m_settings->value("Save/path").toString()
							   << "");
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

	for (int i = 0; i < actuals.count(); i++)
	{
		int limit = m_pages.value(actuals.at(i))->highLimit();
		int v1 = qMin((limit > 0 ? limit : 200), qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount()));
		int v2 = qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount());
		if (v1 == 0 && v2 == 0)
			continue;

		QString search = m_pages.value(actuals.at(i))->search().join(' ');
		emit batchAddGroup(QStringList()
						   << search
						   << "1"
						   << QString::number(v1)
						   << QString::number(v2)
						   << m_settings->value("downloadblacklist").toString()
						   << actuals.at(i)
						   << m_settings->value("Save/filename").toString()
						   << m_settings->value("Save/path").toString()
						   << "");
	}
}


void tagTab::focusSearch()
{
	m_search->setFocus();
}

QString tagTab::tags()
{ return m_search->toPlainText(); }
