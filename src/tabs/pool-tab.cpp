#include <QMessageBox>
#include <QMenu>
#include "pool-tab.h"
#include "ui_pool-tab.h"
#include "ui/QBouton.h"
#include "viewer/zoomwindow.h"
#include "searchwindow.h"
#include "mainwindow.h"


poolTab::poolTab(int id, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(id, sites, profile, parent), ui(new Ui::poolTab), m_id(id), m_lastTags(QString()), m_sized(false)
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
	for (int i = 0; i < m_layouts.size(); i++)
	{ clearLayout(m_layouts[i]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();

	emit(closed(this));
	e->accept();
}



void poolTab::load()
{
	log(tr("Chargement des résultats..."));
	clear();

	if (!m_from_history)
	{ addHistory(m_search->toPlainText(), ui->spinPage->value(), ui->spinImagesPerPage->value(), ui->spinColumns->value()); }
	m_from_history = false;

	if (m_search->toPlainText() != m_lastTags && !m_lastTags.isNull() && m_history_cursor == m_history.size() - 1)
	{ ui->spinPage->setValue(1); }
	m_lastTags = m_search->toPlainText();

	ui->widgetMeant->hide();
	ui->buttonFirstPage->setEnabled(ui->spinPage->value() > 1);
	ui->buttonPreviousPage->setEnabled(ui->spinPage->value() > 1);
	setWindowTitle(m_search->toPlainText().isEmpty() ? tr("Recherche") : m_search->toPlainText().replace("&", "&&"));
	emit titleChanged(this);

	QStringList tags = m_search->toPlainText().trimmed().split(" ", QString::SkipEmptyParts);
	tags.append(m_settings->value("add").toString().trimmed().split(" ", QString::SkipEmptyParts));
	tags.prepend("pool:"+QString::number(ui->spinPool->value()));
	int perpage = ui->spinImagesPerPage->value();
	Page *page = new Page(m_profile, m_sites->value(ui->comboSites->currentText()), m_sites->values(), tags, ui->spinPage->value(), perpage, m_postFiltering->toPlainText().split(" ", QString::SkipEmptyParts), true, this);
	log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
	connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
	m_pages.insert(page->website(), page);
	QGridLayout *l = new QGridLayout;
	l->setHorizontalSpacing(m_settings->value("Margins/horizontal", 6).toInt());
	l->setVerticalSpacing(m_settings->value("Margins/vertical", 6).toInt());
	m_layouts.append(l);
	m_stop = false;
	page->load();
	if (m_settings->value("useregexfortags", true).toBool())
	{
		connect(page, SIGNAL(finishedLoadingTags(Page*)), this, SLOT(finishedLoadingTags(Page*)));
		page->loadTags();
	}

	m_page = 0;

	emit changed(this);
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

void poolTab::firstPage()
{
	ui->spinPage->setValue(1);
	load();
}
void poolTab::previousPage()
{
	if (ui->spinPage->value() > 1)
	{
		ui->spinPage->setValue(ui->spinPage->value()-1);
		load();
	}
}
void poolTab::nextPage()
{
	if (ui->spinPage->value() < ui->spinPage->maximum())
	{
		ui->spinPage->setValue(ui->spinPage->value()+1);
		load();
	}
}
void poolTab::lastPage()
{
	ui->spinPage->setValue(m_pagemax);
	load();
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

void poolTab::setImagesPerPage(int ipp)
{ ui->spinImagesPerPage->setValue(ipp); }
void poolTab::setColumns(int columns)
{ ui->spinColumns->setValue(columns); }
void poolTab::setPostFilter(QString postfilter)
{ m_postFiltering->setText(postfilter); }

int poolTab::imagesPerPage()	{ return ui->spinImagesPerPage->value();	}
int poolTab::columns()			{ return ui->spinColumns->value();			}
QString poolTab::postFilter()	{ return m_postFiltering->toPlainText();	}
QString poolTab::tags()			{ return m_search->toPlainText();			}
QString poolTab::site()			{ return ui->comboSites->currentText();		}
