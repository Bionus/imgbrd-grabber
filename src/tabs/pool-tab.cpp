#include <QMessageBox>
#include <QMenu>
#include "pool-tab.h"
#include "ui_pool-tab.h"
#include "ui/QBouton.h"
#include "viewer/zoomwindow.h"
#include "searchwindow.h"
#include "mainwindow.h"


poolTab::poolTab(QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(sites, profile, parent), ui(new Ui::poolTab), m_sized(false)
{
	ui->setupUi(this);
	ui->widgetMeant->hide();

	// UI members for SearchTab class
	ui_checkMergeResults = nullptr;
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

	QStringList sources = m_sites->keys();
	for (QString source : sources)
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
	qDeleteAll(m_pages);
	m_pages.clear();
	m_images.clear();
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();
	for (Site *site : m_layouts.keys())
	{ clearLayout(m_layouts[site]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();

	emit(closed(this));
	e->accept();
}


void poolTab::load()
{
	// Get the search values
	QString search = m_search->toPlainText();
	QStringList tags = search.trimmed().split(" ", QString::SkipEmptyParts);
	tags.prepend("pool:"+QString::number(ui->spinPool->value()));

	setWindowTitle(search.isEmpty() ? tr("Search") : search.replace("&", "&&"));
	emit titleChanged(this);

	loadTags(tags);
}

QList<Site*> poolTab::loadSites()
{
	QList<Site*> sites;
	sites.append(m_sites->value(ui->comboSites->currentText()));
	return sites;
}

bool poolTab::validateImage(QSharedPointer<Image> img)
{
	Q_UNUSED(img);
	return true;
}


void poolTab::getPage()
{
	bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	int perpage = unloaded ? ui->spinImagesPerPage->value() : m_pages.value(ui->comboSites->currentText())->images().count();
	emit batchAddGroup(QStringList() << "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed() << QString::number(ui->spinPage->value()) << QString::number(perpage) << QString::number(perpage) << m_settings->value("downloadblacklist").toString() << ui->comboSites->currentText() << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
}
void poolTab::getAll()
{
	int limit = m_sites->value(ui->comboSites->currentText())->contains("Urls/1/Limit") ? m_sites->value(ui->comboSites->currentText())->value("Urls/1/Limit").toInt() : 0;
	emit batchAddGroup(QStringList() << "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed() << "1" << QString::number(qMin((limit > 0 ? limit : 200), qMax(m_pages.value(ui->comboSites->currentText())->images().count(), m_pages.value(ui->comboSites->currentText())->imagesCount()))) << QString::number(qMax(m_pages.value(ui->comboSites->currentText())->images().count(), m_pages.value(ui->comboSites->currentText())->imagesCount())) << m_settings->value("downloadblacklist").toString() << ui->comboSites->currentText() << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
}


void poolTab::setTags(QString tags)
{
	activateWindow();
	m_search->setText(tags);
	load();
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

QString poolTab::tags()
{ return m_search->toPlainText(); }
QString poolTab::site()
{ return ui->comboSites->currentText(); }
