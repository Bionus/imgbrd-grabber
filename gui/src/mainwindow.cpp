#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QSound>
#include <QShortcut>
#include <QDesktopServices>
#include <QCloseEvent>
#include <QCompleter>
#include <QNetworkProxy>
#include <QScrollBar>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
	#include <QStorageInfo>
#endif
#include <QMimeData>
#include <qmath.h>
#if defined(Q_OS_WIN)
	#include "windows.h"
	#include <float.h>
#endif
#include "ui_mainwindow.h"
#include "ui/QAffiche.h"
#include "settings/optionswindow.h"
#include "settings/startwindow.h"
#include "batch/addgroupwindow.h"
#include "batch/adduniquewindow.h"
#include "batch/batchwindow.h"
#include "aboutwindow.h"
#include "commands/commands.h"
#include "utils/blacklist-fix/blacklist-fix-1.h"
#include "utils/rename-existing/rename-existing-1.h"
#include "utils/empty-dirs-fix/empty-dirs-fix-1.h"
#include "utils/md5-fix/md5-fix.h"
#include "utils/tag-loader/tag-loader.h"
#include "models/filename.h"
#include "downloader/downloader.h"
#include "downloader/download-query-loader.h"
#include "downloader/download-query-image.h"
#include "downloader/download-query-group.h"
#include "downloader/image-downloader.h"
#include "updater/update-dialog.h"
#include "theme-loader.h"
#include "models/api.h"
#include "models/profile.h"
#include "models/page.h"
#include "models/favorite.h"
#include "tabs/tabs-loader.h"
#include "tabs/search-tab.h"
#include "tabs/tag-tab.h"
#include "tabs/pool-tab.h"
#include "tabs/favorites-tab.h"
#include "danbooru-downloader-importer.h"
#include "tag-context-menu.h"
#include "helpers.h"
#include "functions.h"


mainWindow::mainWindow(Profile *profile)
	: ui(new Ui::mainWindow), m_profile(profile), m_favorites(m_profile->getFavorites()), m_downloads(0), m_loaded(false), m_getAll(false), m_forcedTab(false), m_batchAutomaticRetries(0), m_showLog(true)
{ }
void mainWindow::init(QStringList args, QMap<QString,QString> params)
{
	m_settings = m_profile->getSettings();

	ThemeLoader themeLoader(savePath("themes/", true));
	themeLoader.setTheme(m_settings->value("theme", "Default").toString());
	ui->setupUi(this);

	m_showLog = m_settings->value("Log/show", true).toBool();
	if (!m_showLog)
	{ ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabLog)); }
	else
	{ connect(&Logger::getInstance(), &Logger::newLog, this, &mainWindow::logShow); }

	log("New session started.", Logger::Info);
	log(QString("Software version: %1.").arg(VERSION), Logger::Info);
	log(QString("Path: %1").arg(qApp->applicationDirPath()), Logger::Info);
	log(QString("Loading preferences from <a href=\"file:///%1\">%1</a>").arg(m_settings->fileName()), Logger::Info);

	bool crashed = m_settings->value("crashed", false).toBool();
	m_settings->setValue("crashed", true);
	m_settings->sync();

	// On first launch after setup, we restore the setup's language
	QString setupSettingsFile = savePath("innosetup.ini");
	if (QFile::exists(setupSettingsFile))
	{
		QSettings setupSettings(setupSettingsFile, QSettings::IniFormat);
		QString setupLanguage = setupSettings.value("language", "en").toString();

		QSettings associations(savePath("languages/languages.ini"), QSettings::IniFormat);
		associations.beginGroup("innosetup");
		QStringList keys = associations.childKeys();

		// Only if the setup language is available in Grabber
		if (keys.contains(setupLanguage))
		{
			m_settings->setValue("language", associations.value(setupLanguage).toString());
		}

		// Remove the setup settings file to not do this every time
		QFile::remove(setupSettingsFile);
	}

	// Load translations
	qApp->installTranslator(&m_translator);
	qApp->installTranslator(&m_qtTranslator);
	loadLanguage(m_settings->value("language", "English").toString());

	tabifyDockWidget(ui->dock_internet, ui->dock_wiki);
	tabifyDockWidget(ui->dock_wiki, ui->dock_kfl);
	tabifyDockWidget(ui->dock_kfl, ui->dock_favorites);
	ui->dock_internet->raise();

	ui->menuView->addAction(ui->dock_internet->toggleViewAction());
	ui->menuView->addAction(ui->dock_wiki->toggleViewAction());
	ui->menuView->addAction(ui->dock_kfl->toggleViewAction());
	ui->menuView->addAction(ui->dock_favorites->toggleViewAction());
	ui->menuView->addAction(ui->dockOptions->toggleViewAction());

	m_favorites = m_profile->getFavorites();

	if (m_settings->value("Proxy/use", false).toBool())
	{
		bool useSystem = m_settings->value("Proxy/useSystem", false).toBool();
		QNetworkProxyFactory::setUseSystemConfiguration(useSystem);

		if (!useSystem)
		{
			QNetworkProxy::ProxyType type = m_settings->value("Proxy/type", "http").toString() == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy;
			QNetworkProxy proxy(type, m_settings->value("Proxy/hostName").toString(), m_settings->value("Proxy/port").toInt());
			QNetworkProxy::setApplicationProxy(proxy);
			log(QString("Enabling application proxy on host \"%1\" and port %2.").arg(m_settings->value("Proxy/hostName").toString()).arg(m_settings->value("Proxy/port").toInt()), Logger::Info);
		}
		else
		{ log("Enabling system-wide proxy.", Logger::Info); }
	}

	m_progressdialog = nullptr;

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	log("Loading sources", Logger::Debug);
	loadSites();

	if (m_sites.size() == 0)
	{
		QMessageBox::critical(this, tr("No source found"), tr("No source found. Do you have a configuration problem? Try to reinstall the program."));
		qApp->quit();
		this->deleteLater();
		return;
	}
	else
	{
		QString srsc = "";
		for (int i = 0; i < m_sites.size(); ++i)
		{ srsc += (i != 0 ? ", " : "") + m_sites.keys().at(i) + " (" + m_sites.values().at(i)->type() + ")"; }
		log(QString("%1 source%2 found: %3").arg(m_sites.size()).arg(m_sites.size() > 1 ? "s" : "").arg(srsc), Logger::Info);
	}

	ui->actionClosetab->setShortcut(QKeySequence::Close);
	QShortcut *actionCloseTabW = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this);
	connect(actionCloseTabW, &QShortcut::activated, ui->actionClosetab, &QAction::trigger);

	QShortcut *actionFocusSearch = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
	connect(actionFocusSearch, &QShortcut::activated, this, &mainWindow::focusSearch);

	QShortcut *actionDeleteBatchGroups = new QShortcut(QKeySequence::Delete, ui->tableBatchGroups);
	actionDeleteBatchGroups->setContext(Qt::WidgetWithChildrenShortcut);
	connect(actionDeleteBatchGroups, &QShortcut::activated, this, &mainWindow::batchClearSelGroups);

	QShortcut *actionDeleteBatchUniques = new QShortcut(QKeySequence::Delete, ui->tableBatchUniques);
	actionDeleteBatchUniques->setContext(Qt::WidgetWithChildrenShortcut);
	connect(actionDeleteBatchUniques, &QShortcut::activated, this, &mainWindow::batchClearSelUniques);

	QShortcut *actionNextTab = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this);
	connect(actionNextTab, &QShortcut::activated, this, &mainWindow::tabNext);
	QShortcut *actionPrevTab = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this);
	connect(actionPrevTab, &QShortcut::activated, this, &mainWindow::tabPrev);

	ui->actionAddtab->setShortcut(QKeySequence::AddTab);
	ui->actionQuit->setShortcut(QKeySequence::Quit);
	ui->actionFolder->setShortcut(QKeySequence::Open);

	connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	// Action on first load
	if (m_settings->value("firstload", true).toBool())
	{
		this->onFirstLoad();
		m_settings->setValue("firstload", false);
	}

	// Crash restoration
	m_restore = m_settings->value("start", "none").toString() == "restore";
	if (crashed)
	{
		log("It seems that Imgbrd-Grabber hasn't shut down properly last time.", Logger::Warning);

		QString msg = tr("It seems that the application was not properly closed for its last use. Do you want to restore your last session?");
		QMessageBox dlg(QMessageBox::Question, "Grabber", msg, QMessageBox::Yes | QMessageBox::No);
		dlg.setWindowIcon(windowIcon());
		dlg.setDefaultButton(QMessageBox::Yes);

		int response = dlg.exec();
		m_restore = response == QMessageBox::Yes;
	}

	// Restore download lists
	if (m_restore)
	{ loadLinkList(m_profile->getPath() + "/restore.igl"); }

	// Loading last window state, size and position from the settings file
	restoreGeometry(m_settings->value("geometry").toByteArray());
	restoreState(m_settings->value("state").toByteArray());

	// Tab add button
	QPushButton *add = new QPushButton(QIcon(":/images/add.png"), "", this);
		add->setFlat(true);
		add->resize(QSize(12,12));
		connect(add, SIGNAL(clicked()), this, SLOT(addTab()));
		ui->tabWidget->setCornerWidget(add);

	// Favorites tab
	m_favoritesTab = new favoritesTab(&m_sites, m_profile, this);
	connect(m_favoritesTab, &searchTab::batchAddGroup, this, &mainWindow::batchAddGroup);
	connect(m_favoritesTab, SIGNAL(batchAddUnique(DownloadQueryImage)), this, SLOT(batchAddUnique(DownloadQueryImage)));
	connect(m_favoritesTab, &searchTab::titleChanged, this, &mainWindow::updateTabTitle);
	connect(m_favoritesTab, &searchTab::changed, this, &mainWindow::updateTabs);
	ui->tabWidget->insertTab(m_tabs.size(), m_favoritesTab, tr("Favorites"));
	ui->tabWidget->setCurrentIndex(0);

	// Load given files
	parseArgs(args, params);

	// Initial login and selected sources setup
	QStringList keys = m_sites.keys();
	QString sav = m_settings->value("sites", "1").toString();
	m_waitForLogin = 0;
	QList<Site*> requiredLogins;
	for (int i = 0; i < m_sites.count(); i++)
	{
		if (i < sav.count() && sav[i] == '1')
		{
			m_selectedSources.append(true);
			connect(m_sites[keys[i]], &Site::loggedIn, this, &mainWindow::initialLoginsFinished);
			requiredLogins.append(m_sites[keys[i]]);
		}
		else
		{ m_selectedSources.append(false); }
	}
	if (requiredLogins.isEmpty())
	{
		initialLoginsDone();
	}
	else
	{
		m_waitForLogin += requiredLogins.count();
		for (Site *site : requiredLogins)
			site->login();
	}

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	on_buttonInitSettings_clicked();

	QStringList sizes = m_settings->value("batch", "100,100,100,100,100,100,100,100,100").toString().split(',');
	int m = sizes.size() > ui->tableBatchGroups->columnCount() ? ui->tableBatchGroups->columnCount() : sizes.size();
	for (int i = 0; i < m; i++)
	{ ui->tableBatchGroups->horizontalHeader()->resizeSection(i, sizes.at(i).toInt()); }

	m_lineFolder_completer = QStringList(m_settings->value("Save/path").toString());
	ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer));
	//m_lineFilename_completer = QStringList(m_settings->value("Save/filename").toString());
	//ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	ui->comboFilename->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);

	connect(m_profile, &Profile::favoritesChanged, this, &mainWindow::updateFavorites);
	connect(m_profile, &Profile::keptForLaterChanged, this, &mainWindow::updateKeepForLater);
	connect(m_profile, &Profile::sitesChanged, this, &mainWindow::loadSites);
	updateFavorites();
	updateKeepForLater();

	m_currentTab = nullptr;
	log("End of initialization", Logger::Debug);
}

void mainWindow::parseArgs(QStringList args, QMap<QString,QString> params)
{
	// When we use Grabber to open a file
	QStringList tags;
	if (args.count() == 1 && QFile::exists(args[0]))
	{
		// Load an IGL file
		QFileInfo info(args[0]);
		if (info.suffix() == "igl")
		{
			loadLinkList(info.absoluteFilePath());
			ui->tabWidget->setCurrentIndex(m_tabs.size() + 1);
			m_forcedTab = true;
			return;
		}

		// Search any image by its MD5
		loadMd5(info.absoluteFilePath(), true, false, false);
		return;
	}

	// Other positional arguments are treated as tags
	tags.append(args);
	tags.append(params.value("tags").split(' ', QString::SkipEmptyParts));
	if (!tags.isEmpty() || m_settings->value("start", "none").toString() == "firstpage")
	{
		loadTag(tags.join(" "), true, false, false);
	}
}

void mainWindow::initialLoginsFinished()
{
	disconnect((Site*)sender(), &Site::loggedIn, this, &mainWindow::initialLoginsFinished);

	m_waitForLogin--;
	if (m_waitForLogin != 0)
		return;

	initialLoginsDone();
}

void mainWindow::initialLoginsDone()
{
	if (m_restore)
	{ loadTabs(m_profile->getPath() + "/tabs.txt"); }
	if (m_tabs.isEmpty())
	{ addTab(); }

	if (!m_forcedTab)
	{ ui->tabWidget->setCurrentIndex(0); }

	m_currentTab = ui->tabWidget->currentWidget();
	m_loaded = true;
}

void mainWindow::loadSites()
{
	QMap<QString, Site*> sites = Site::getAllSites(m_profile);

	QStringList current = m_sites.keys();
	QStringList news = sites.keys();

	for (int i = 0; i < sites.size(); ++i)
	{
		QString k = news[i];
		if (!current.contains(k))
		{ m_sites.insert(k, sites.value(k)); }
		/*else
		{ delete sites->value(k); }*/
	}
}

mainWindow::~mainWindow()
{
	qDeleteAll(m_sites);
	delete m_profile;
	delete ui;
}

void mainWindow::focusSearch()
{
	if (ui->tabWidget->widget(ui->tabWidget->currentIndex())->maximumWidth() != 16777214)
	{
		searchTab *tab = dynamic_cast<searchTab *>(ui->tabWidget->widget(ui->tabWidget->currentIndex()));
		tab->focusSearch();
	}
}

void mainWindow::onFirstLoad()
{
	// Save all default settings
	optionsWindow *ow = new optionsWindow(m_profile, this);
	ow->save();
	ow->deleteLater();

	// Detect and Danbooru Downloader settings
	DanbooruDownloaderImporter ddImporter;
	if (ddImporter.isInstalled())
	{
		int reponse = QMessageBox::question(this, "", tr("The Mozilla Firefox addon \"Danbooru Downloader\" has been detected on your system. Do you want to load its preferences?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			ddImporter.import(m_settings);
			return;
		}
	}

	// Open startup window
	startWindow *swin = new startWindow(&m_sites, m_profile, this);
	connect(swin, SIGNAL(languageChanged(QString)), this, SLOT(loadLanguage(QString)));
	connect(swin, &startWindow::settingsChanged, this, &mainWindow::on_buttonInitSettings_clicked);
	connect(swin, &startWindow::sourceChanged, this, &mainWindow::setSource);
	swin->show();
}

void mainWindow::addTab(QString tag, bool background, bool save)
{
	tagTab *w = new tagTab(&m_sites, m_profile, this);
	this->addSearchTab(w, background, save);

	if (!tag.isEmpty())
	{ w->setTags(tag); }
	else
	{ w->focusSearch(); }
}
void mainWindow::addPoolTab(int pool, QString site, bool background, bool save)
{
	poolTab *w = new poolTab(&m_sites, m_profile, this);
	this->addSearchTab(w, background, save);

	if (!site.isEmpty())
	{ w->setSite(site); }
	if (pool != 0)
	{ w->setPool(pool, site); }
	else
	{ w->focusSearch(); }
}
void mainWindow::addSearchTab(searchTab *w, bool background, bool save)
{
	if (m_tabs.size() > ui->tabWidget->currentIndex())
	{
		w->setSources(m_tabs[ui->tabWidget->currentIndex()]->sources());
		w->setImagesPerPage(m_tabs[ui->tabWidget->currentIndex()]->imagesPerPage());
		w->setColumns(m_tabs[ui->tabWidget->currentIndex()]->columns());
		w->setPostFilter(m_tabs[ui->tabWidget->currentIndex()]->postFilter());
	}
	connect(w, &searchTab::batchAddGroup, this, &mainWindow::batchAddGroup);
	connect(w, SIGNAL(batchAddUnique(const DownloadQueryImage &)), this, SLOT(batchAddUnique(const DownloadQueryImage &)));
	connect(w, &searchTab::titleChanged, this, &mainWindow::updateTabTitle);
	connect(w, &searchTab::changed, this, &mainWindow::updateTabs);
	connect(w, &searchTab::closed, this, &mainWindow::tabClosed);

	QString title = w->windowTitle();
	if (title.isEmpty())
	{ title = "New tab"; }

	int pos = m_loaded ? ui->tabWidget->currentIndex() + (!m_tabs.isEmpty()) : m_tabs.count();
	int index = ui->tabWidget->insertTab(pos, w, title);
	m_tabs.append(w);

	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "", this);
		closeTab->setFlat(true);
		closeTab->resize(QSize(8,8));
		connect(closeTab, SIGNAL(clicked()), w, SLOT(deleteLater()));
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);

	if (!background)
		ui->tabWidget->setCurrentIndex(index);

	if (save)
		saveTabs(m_profile->getPath() + "/tabs.txt");
}

bool mainWindow::saveTabs(QString filename)
{
	return TabsLoader::save(filename, m_tabs, (searchTab*)m_currentTab);
}
bool mainWindow::loadTabs(QString filename)
{
	QList<searchTab*> tabs;
	int currentTab;

	if (!TabsLoader::load(filename, tabs, currentTab, m_profile, m_sites, this))
		return false;

	for (auto tab : tabs)
		addSearchTab(tab, true, false);

	if (currentTab >= 0)
	{
		ui->tabWidget->setCurrentIndex(currentTab);
		m_forcedTab = true;
	}

	return true;
}
void mainWindow::updateTabTitle(searchTab *tab)
{
	ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), tab->windowTitle());
}
void mainWindow::updateTabs()
{
	if (m_loaded)
	{
		saveTabs(m_profile->getPath() + "/tabs.txt");
	}
}
void mainWindow::tabClosed(searchTab *tab)
{
	// Store closed tab information
	QJsonObject obj;
	tab->write(obj);
	m_closedTabs.append(obj);
	if (m_closedTabs.count() > CLOSED_TAB_HISTORY_MAX) {
		m_closedTabs.removeFirst();
	}
	ui->actionRestoreLastClosedTab->setEnabled(true);

	m_tabs.removeAll(tab);
}
void mainWindow::restoreLastClosedTab()
{
	if (m_closedTabs.isEmpty())
		return;

	QJsonObject infos = m_closedTabs.takeLast();
	searchTab *tab = TabsLoader::loadTab(infos, m_profile, m_sites, this);
	addSearchTab(tab);

	ui->actionRestoreLastClosedTab->setEnabled(!m_closedTabs.isEmpty());
}
void mainWindow::currentTabChanged(int tab)
{
	if (m_loaded && tab < m_tabs.size())
	{
		if (ui->tabWidget->widget(tab)->maximumWidth() != 16777214)
		{
			searchTab *tb = m_tabs[tab];
			if (m_currentTab != nullptr && m_currentTab == ui->tabWidget->currentWidget())
			{ return; }

			setTags(tb->results());
			setWiki(tb->wiki());
			m_currentTab = ui->tabWidget->currentWidget();
		}
	}
}

void mainWindow::setTags(QList<Tag> tags, searchTab *from)
{
	if (from != nullptr && m_tabs.indexOf(from) != ui->tabWidget->currentIndex())
		return;

	clearLayout(ui->dockInternetScrollLayout);
	m_currentTags = tags;

	QAffiche *taglabel = new QAffiche(QVariant(), 0, QColor(), this);
	taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	connect(taglabel, static_cast<void (QAffiche::*)(QString)>(&QAffiche::middleClicked), this, &mainWindow::loadTagTab);
	connect(taglabel, &QAffiche::linkHovered, this, &mainWindow::linkHovered);
	connect(taglabel, &QAffiche::linkActivated, this, &mainWindow::loadTagNoTab);
	taglabel->setText(Tag::Stylished(tags, m_profile, true, true).join("<br/>"));

	// Context menu
	taglabel->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(taglabel, &QWidget::customContextMenuRequested, this, &mainWindow::contextMenu);

	ui->dockInternetScrollLayout->addWidget(taglabel);
}

void mainWindow::closeCurrentTab()
{
	// Unclosable tabs have a maximum width of 16777214 (default: 16777215)
	auto currentTab = ui->tabWidget->currentWidget();
	if (currentTab->maximumWidth() != 16777214)
	{ currentTab->deleteLater(); }
}

void mainWindow::tabNext()
{
	int index = ui->tabWidget->currentIndex();
	int count = ui->tabWidget->count();
	ui->tabWidget->setCurrentIndex((index + 1) % count);
}
void mainWindow::tabPrev()
{
	int index = ui->tabWidget->currentIndex();
	int count = ui->tabWidget->count();
	ui->tabWidget->setCurrentIndex((index - 1 + count) % count);
}

void mainWindow::addTableItem(QTableWidget *table, int row, int col, QString text)
{
	QTableWidgetItem *item = new QTableWidgetItem(text);
	item->setToolTip(text);

	table->setItem(row, col, item);
}

void mainWindow::batchAddGroup(const DownloadQueryGroup &values)
{
	// Ignore downloads already present in the list
	if (m_groupBatchs.contains(values))
		return;

	m_groupBatchs.append(values);
	int pos = m_groupBatchs.count();

	ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount() + 1);
	int row = ui->tableBatchGroups->rowCount() - 1;
	m_allow = false;

	QTableWidgetItem *item = new QTableWidgetItem(getIcon(":/images/status/pending.png"), QString::number(pos));
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	ui->tableBatchGroups->setItem(row, 0, item);

	addTableItem(ui->tableBatchGroups, row, 1, values.tags);
	addTableItem(ui->tableBatchGroups, row, 2, values.site->url());
	addTableItem(ui->tableBatchGroups, row, 3, QString::number(values.page));
	addTableItem(ui->tableBatchGroups, row, 4, QString::number(values.perpage));
	addTableItem(ui->tableBatchGroups, row, 5, QString::number(values.total));
	addTableItem(ui->tableBatchGroups, row, 6, values.filename);
	addTableItem(ui->tableBatchGroups, row, 7, values.path);
	addTableItem(ui->tableBatchGroups, row, 8, values.getBlacklisted ? "true" : "false");

	QProgressBar *prog = new QProgressBar(this);
	prog->setTextVisible(false);
	prog->setMaximum(values.total);
	m_progressBars.append(prog);
	ui->tableBatchGroups->setCellWidget(ui->tableBatchGroups->rowCount()-1, 9, prog);

	m_allow = true;
	saveLinkList(m_profile->getPath() + "/restore.igl");
	updateGroupCount();
}
void mainWindow::updateGroupCount()
{
	int groups = 0;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
		groups += ui->tableBatchGroups->item(i, 5)->text().toInt();
	ui->labelGroups->setText(tr("Groups (%1/%2)").arg(ui->tableBatchGroups->rowCount()).arg(groups));
}
void mainWindow::batchAddUnique(const DownloadQueryImage &query, bool save)
{
	// Ignore downloads already present in the list
	if (m_batchs.contains(query))
		return;

	log(QString("Adding single image: %1").arg(query.values["file_url"]), Logger::Info);

	m_batchs.append(query);
	ui->tableBatchUniques->setRowCount(ui->tableBatchUniques->rowCount() + 1);

	int row = ui->tableBatchUniques->rowCount() - 1;
	addTableItem(ui->tableBatchUniques, row, 0, query.values["id"]);
	addTableItem(ui->tableBatchUniques, row, 1, query.values["md5"]);
	addTableItem(ui->tableBatchUniques, row, 2, query.values["rating"]);
	addTableItem(ui->tableBatchUniques, row, 3, query.values["tags"]);
	addTableItem(ui->tableBatchUniques, row, 4, query.values["file_url"]);
	addTableItem(ui->tableBatchUniques, row, 5, query.values["date"]);
	addTableItem(ui->tableBatchUniques, row, 6, query.site->name());
	addTableItem(ui->tableBatchUniques, row, 7, query.filename);
	addTableItem(ui->tableBatchUniques, row, 8, query.path);

	if (save)
	{ saveLinkList(m_profile->getPath() + "/restore.igl"); }
}
void mainWindow::saveFolder()
{
	QString path = m_settings->value("Save/path").toString().replace("\\", "/");
	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	QDir dir(path);
	if (dir.exists())
	{ showInGraphicalShell(path); }
}
void mainWindow::openSettingsFolder()
{
	QDir dir(savePath(""));
	if (dir.exists())
	{ showInGraphicalShell(dir.absolutePath()); }
}

void mainWindow::batchClear()
{
	// Don't do anything if there's nothing to clear
	if (ui->tableBatchGroups->rowCount() == 0 && ui->tableBatchUniques->rowCount() == 0)
		return;

	// Confirm deletion
	auto reponse = QMessageBox::question(this, tr("Confirmation"), tr("Are you sure you want to clear your download list?"), QMessageBox::Yes | QMessageBox::No);
	if (reponse != QMessageBox::Yes)
		return;

	m_batchs.clear();
	ui->tableBatchUniques->clearContents();
	ui->tableBatchUniques->setRowCount(0);
	m_groupBatchs.clear();
	ui->tableBatchGroups->clearContents();
	ui->tableBatchGroups->setRowCount(0);
	qDeleteAll(m_progressBars);
	m_progressBars.clear();
	updateGroupCount();
}
void mainWindow::batchClearSel()
{
	batchClearSelGroups();
	batchClearSelUniques();
}
void mainWindow::batchClearSelGroups()
{
	// Delete group batchs
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	QList<int> todelete = QList<int>();
	int count = selected.size();
	for (int i = 0; i < count; i++)
		if (!todelete.contains(selected.at(i)->row()))
			todelete.append(selected.at(i)->row());
	qSort(todelete);

	int rem = 0;
	for (int i : todelete)
	{
		int pos = i - rem;
		m_progressBars[pos]->deleteLater();
		m_progressBars.removeAt(pos);
		m_groupBatchs.removeAt(pos);
		ui->tableBatchGroups->removeRow(pos);
		rem++;
	}
	updateGroupCount();
}
void mainWindow::batchClearSelUniques()
{
	// Delete single image downloads
	QList<QTableWidgetItem *> selected = ui->tableBatchUniques->selectedItems();
	QList<int> todelete = QList<int>();
	int count = selected.size();
	for (int i = 0; i < count; i++)
	{ todelete.append(selected.at(i)->row()); }
	qSort(todelete);

	int rem = 0;
	for (int i : todelete)
	{
		int pos = i - rem;
		ui->tableBatchUniques->removeRow(pos);
		m_batchs.removeAt(pos);
		rem++;
	}
	updateGroupCount();
}

void mainWindow::batchMove(int diff)
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	if (selected.isEmpty())
		return;

	QSet<int> rows;
	for (QTableWidgetItem *item : selected)
		rows.insert(item->row());

	for (int sourceRow : rows)
	{
		int destRow = sourceRow + diff;
		if (destRow < 0 || destRow >= ui->tableBatchGroups->rowCount())
			return;

		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
		{
			QTableWidgetItem *sourceItem = ui->tableBatchGroups->takeItem(sourceRow, col);
			QTableWidgetItem *destItem = ui->tableBatchGroups->takeItem(destRow, col);

			ui->tableBatchGroups->setItem(sourceRow, col, destItem);
			ui->tableBatchGroups->setItem(destRow, col, sourceItem);
		}
	}

	QItemSelection selection;
	for (int i = 0; i < selected.count(); i++)
	{
		QModelIndex index = ui->tableBatchGroups->model()->index(selected.at(i)->row(), selected.at(i)->column());
		selection.select(index, index);
	}

	QItemSelectionModel* selectionModel = new QItemSelectionModel(ui->tableBatchGroups->model(), this);
	selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
	ui->tableBatchGroups->setSelectionModel(selectionModel);
}
void mainWindow::batchMoveUp()
{
	batchMove(-1);
}
void mainWindow::batchMoveDown()
{
	batchMove(1);
}

void mainWindow::updateBatchGroups(int y, int x)
{
	if (m_allow && x > 0)
	{
		QString val = ui->tableBatchGroups->item(y, x)->text();
		int toInt = val.toInt();

		switch (x)
		{
			case 1:	m_groupBatchs[y].tags = val;						break;
			case 3:	m_groupBatchs[y].page = toInt;						break;
			case 6:	m_groupBatchs[y].filename = val;					break;
			case 7:	m_groupBatchs[y].path = val;						break;
			case 8:	m_groupBatchs[y].postFiltering = val.split(' ', QString::SkipEmptyParts);	break;
			case 9:	m_groupBatchs[y].getBlacklisted = (val != "false");	break;

			case 2:
				if (!m_sites.contains(val))
				{
					error(this, tr("This source is not valid."));
					ui->tableBatchGroups->item(y, x)->setText(m_groupBatchs[y].site->url());
				}
				m_groupBatchs[y].site = m_sites.value(val);
				break;

			case 4:
				if (toInt < 1)
				{
					error(this, tr("The image per page value must be greater or equal to 1."));
					ui->tableBatchGroups->item(y, x)->setText(QString::number(m_groupBatchs[y].perpage));
				}
				else
				{ m_groupBatchs[y].perpage = toInt; }
				break;

			case 5:
				if (toInt < 0)
				{
					error(this, tr("The image limit must be greater or equal to 0."));
					ui->tableBatchGroups->item(y, x)->setText(QString::number(m_groupBatchs[y].total));
				}
				else
				{
					m_groupBatchs[y].total = toInt;
					m_progressBars[y]->setMaximum(toInt);
				}
				break;
		}

		saveLinkList(m_profile->getPath() + "/restore.igl");
	}
}

QList<Site*> mainWindow::getSelectedSites()
{
	if (m_tabs.count() > 0)
		m_selectedSources = m_tabs[0]->sources();

	QList<Site*> selected;
	for (int i = 0; i < m_selectedSources.count(); i++)
		if (m_selectedSources[i])
			selected.append(m_sites.values().at(i));

	return selected;
}
Site* mainWindow::getSelectedSiteOrDefault()
{
	QList<Site*> selected = getSelectedSites();

	if (selected.isEmpty())
		return m_sites.first();

	return selected.first();
}

void mainWindow::addGroup()
{
	QString selected = getSelectedSiteOrDefault()->name();

	AddGroupWindow *wAddGroup = new AddGroupWindow(selected, m_sites, m_profile, this);
	connect(wAddGroup, &AddGroupWindow::sendData, this, &mainWindow::batchAddGroup);
	wAddGroup->show();
}
void mainWindow::addUnique()
{
	QString selected = getSelectedSiteOrDefault()->name();

	AddUniqueWindow *wAddUnique = new AddUniqueWindow(selected, m_sites, m_profile, this);
	connect(wAddUnique, SIGNAL(sendData(DownloadQueryImage)), this, SLOT(batchAddUnique(DownloadQueryImage)));
	wAddUnique->show();
}

void mainWindow::updateFavorites()
{
	while (!ui->layoutFavoritesDock->isEmpty())
	{
		QWidget *wid = ui->layoutFavoritesDock->takeAt(0)->widget();
		wid->hide();
		wid->deleteLater();
	}

	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[qMax(ui->comboOrderFav->currentIndex(), 0)];
	bool reverse = (ui->comboAscFav->currentIndex() == 1);

	if (order == "note")
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByNote); }
	else if (order == "lastviewed")
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByLastviewed); }
	else
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByName); }
	if (reverse)
	{ m_favorites = reversed(m_favorites); }
	QString format = tr("MM/dd/yyyy");

	for (Favorite fav : m_favorites)
	{
		QLabel *lab = new QLabel(QString("<a href=\"%1\" style=\"color:black;text-decoration:none;\">%2</a>").arg(fav.getName(), fav.getName()), this);
		connect(lab, SIGNAL(linkActivated(QString)), this, SLOT(loadTag(QString)));
		lab->setToolTip("<img src=\""+fav.getImagePath()+"\" /><br/>"+tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %%<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(format)));
		ui->layoutFavoritesDock->addWidget(lab);
	}
}
void mainWindow::updateKeepForLater()
{
	QStringList kfl = m_profile->getKeptForLater();

	clearLayout(ui->dockKflScrollLayout);

	for (QString tag : kfl)
	{
		QAffiche *taglabel = new QAffiche(QString(tag), 0, QColor(), this);
		taglabel->setText(QString("<a href=\"%1\" style=\"color:black;text-decoration:none;\">%1</a>").arg(tag));
		taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
		connect(taglabel, static_cast<void (QAffiche::*)(QString)>(&QAffiche::middleClicked), this, &mainWindow::loadTagTab);
		connect(taglabel, &QAffiche::linkActivated, this, &mainWindow::loadTagNoTab);
		ui->dockKflScrollLayout->addWidget(taglabel);
	}
}


void mainWindow::logShow(QString msg)
{
	if (!m_showLog)
		return;

	ui->labelLog->appendHtml(msg);
	ui->labelLog->verticalScrollBar()->setValue(ui->labelLog->verticalScrollBar()->maximum());
}
void mainWindow::logClear()
{
	QFile logFile(m_profile->getPath() + "/main.log");
	if (logFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		logFile.resize(0);
		logFile.close();
	}

	if (m_showLog)
	{
		ui->labelLog->clear();
	}
}
void mainWindow::logOpen()
{ QDesktopServices::openUrl("file:///" + m_profile->getPath() + "/main.log"); }

void mainWindow::loadLanguage(const QString& rLanguage, bool shutup)
{
	if (m_currLang != rLanguage)
	{
		m_currLang = rLanguage;
		QLocale locale = QLocale(m_currLang);
		QLocale::setDefault(locale);

		m_translator.load(savePath("languages/"+m_currLang+".qm", true));
		m_qtTranslator.load(savePath("languages/qt/"+m_currLang+".qm", true));

		if (!shutup)
		{
			log(QString("Translating texts in %1...").arg(m_currLang), Logger::Info);
			ui->retranslateUi(this);
			DONE();
		}
	}
}

// Update interface language
void mainWindow::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::LocaleChange)
	{
		QString locale = QLocale::system().name();
			locale.truncate(locale.lastIndexOf('_'));
			loadLanguage(locale);
	}
	QMainWindow::changeEvent(event);
}

// Save tabs and settings on close
void mainWindow::closeEvent(QCloseEvent *e)
{
	// Confirm before closing if there is a batch download or multiple tabs
	if (m_settings->value("confirm_close", true).toBool() && (m_tabs.count() > 1 || m_getAll))
	{
		QMessageBox msgBox(this);
		msgBox.setText(tr("Are you sure you want to quit?"));
		msgBox.setIcon(QMessageBox::Warning);
		QCheckBox dontShowCheckBox(tr("Don't keep for later"));
		dontShowCheckBox.setCheckable(true);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
		msgBox.setCheckBox(&dontShowCheckBox);
#else
		msgBox.addButton(&dontShowCheckBox, QMessageBox::ResetRole);
#endif
		msgBox.addButton(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Cancel);
		int response = msgBox.exec();

		// Don't close on "cancel"
		if (response != QMessageBox::Yes)
		{
			e->ignore();
			return;
		}

		// Remember checkbox
		if (dontShowCheckBox.checkState() == Qt::Checked)
		{ m_settings->setValue("confirm_close", false); }
	}

	log("Saving...", Logger::Debug);
		saveLinkList(m_profile->getPath() + "/restore.igl");
		saveTabs(m_profile->getPath() + "/tabs.txt");
		m_settings->setValue("state", saveState());
		m_settings->setValue("geometry", saveGeometry());
		QStringList sizes = QStringList();
		for (int i = 0; i < ui->tableBatchGroups->columnCount(); i++)
		{ sizes.append(QString::number(ui->tableBatchGroups->horizontalHeader()->sectionSize(i))); }
		m_settings->setValue("batch", sizes.join(","));
		for (int i = 0; i < m_tabs.size(); i++)
		{ m_tabs.at(i)->deleteLater(); }
		m_settings->setValue("crashed", false);
		m_settings->sync();
		QFile::copy(m_settings->fileName(), m_profile->getPath() + "/old/settings."+QString(VERSION)+".ini");
		m_profile->sync();
	DONE();
	m_loaded = false;

	e->accept();
	qApp->quit();
}

void mainWindow::options()
{
	log("Opening options window...", Logger::Debug);

	optionsWindow *options = new optionsWindow(m_profile, this);
	connect(options, SIGNAL(languageChanged(QString)), this, SLOT(loadLanguage(QString)));
	connect(options, &optionsWindow::settingsChanged, this, &mainWindow::on_buttonInitSettings_clicked);
	connect(options, &QDialog::accepted, this, &mainWindow::optionsClosed);
	options->show();

	DONE();
}

void mainWindow::optionsClosed()
{
	for (searchTab* tab : m_tabs)
	{
		tab->optionsChanged();
		tab->updateCheckboxes();
	}
}

void mainWindow::setSource(QString source)
{
	if (m_tabs.isEmpty())
		return;

	QList<bool> sel;
	QStringList keys = m_sites.keys();
	for (QString key : keys)
	{ sel.append(key == source); }

	m_tabs[0]->saveSources(sel);
}

void mainWindow::aboutWebsite()
{
	QDesktopServices::openUrl(QUrl(PROJECT_WEBSITE_URL));
}
void mainWindow::aboutGithub()
{
	QDesktopServices::openUrl(QUrl(PROJECT_GITHUB_URL));
}
void mainWindow::aboutReportBug()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/issues/new"));
}

void mainWindow::aboutAuthor()
{
	AboutWindow *aw = new AboutWindow(QString(VERSION), this);
	aw->show();
}

/* Batch download */
void mainWindow::batchSel()
{
	getAll(false);
}
void mainWindow::getAll(bool all)
{
	// Initial checks
	if (m_getAll)
	{
		log("Batch download start cancelled because another one is already running.", Logger::Warning);
		return;
	}
	if (m_settings->value("Save/path").toString().isEmpty())
	{
		error(this, tr("You did not specify a save folder!"));
		return;
	}
	else if (m_settings->value("Save/filename").toString().isEmpty())
	{
		error(this, tr("You did not specify a filename!"));
		return;
	}
	log("Batch download started.", Logger::Info);

	if (m_progressdialog == nullptr)
	{
		m_progressdialog = new batchWindow(m_profile->getSettings(), this);
		connect(m_progressdialog, SIGNAL(paused()), this, SLOT(getAllPause()));
		connect(m_progressdialog, SIGNAL(rejected()), this, SLOT(getAllCancel()));
		connect(m_progressdialog, SIGNAL(skipped()), this, SLOT(getAllSkip()));
	}

	// Reinitialize variables
	m_getAll = true;
	ui->widgetDownloadButtons->setDisabled(m_getAll);
	m_getAllDownloaded = 0;
	m_getAllExists = 0;
	m_getAllIgnored = 0;
	m_getAll404s = 0;
	m_getAllErrors = 0;
	m_getAllSkipped = 0;
	m_downloaders.clear();
	m_getAllRemaining.clear();
	m_getAllFailed.clear();
	m_getAllDownloading.clear();
	m_getAllSkippedImages.clear();
	m_batchPending.clear();

	if (!all)
	{
		QList<int> tdl;
		for (QTableWidgetItem *item : ui->tableBatchUniques->selectedItems())
		{
			int row = item->row();
			if (tdl.contains(row))
				continue;
			tdl.append(row);

			DownloadQueryImage batch = m_batchs[row];
			Page *page = new Page(m_profile, batch.site, m_sites.values(), batch.values.value("tags").split(" "), 1, 1, QStringList(), false, this);
			m_getAllRemaining.append(QSharedPointer<Image>(new Image(batch.site, batch.values, m_profile, page)));
		}
	}
	else
	{
		for (DownloadQueryImage batch : m_batchs)
		{
			if (batch.values.value("file_url").isEmpty())
			{
				// If we cannot get the image's url, we try looking for it
				/*Page *page = new Page(m_sites[site], &m_sites, m_groupBatchs.at(i).at(0).split(' '), m_groupBatchs.at(i).at(1).toInt()+r, pp, QStringList(), false, this);
				connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
				page->load();
				m_groupBatchs[i][8] += (m_groupBatchs[i][8] == "" ? "" : "¤") + QString::number((int)page);
				m_progressdialog->setImagesCount(m_progressdialog->count() + 1);*/
			}
			else
			{
				QMap<QString, QString> dta = batch.values;
				dta.insert("filename", batch.filename);
				dta.insert("folder", batch.path);

				Page *page = new Page(m_profile, batch.site, m_sites.values(), batch.values["tags"].split(" "), 1, 1, QStringList(), false, this);
				m_getAllRemaining.append(QSharedPointer<Image>(new Image(batch.site, dta, m_profile, page)));
			}
		}
	}
	m_getAllLimit = m_batchs.size();

	m_allow = false;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
	{ ui->tableBatchGroups->item(i, 0)->setIcon(getIcon(":/images/status/pending.png")); }
	m_allow = true;
	m_profile->getCommands().before();
	m_batchDownloading.clear();

	QSet<int> todownload = QSet<int>();
	for (QTableWidgetItem *item : ui->tableBatchGroups->selectedItems())
		if (!todownload.contains(item->row()))
			todownload.insert(item->row());

	if (all || !todownload.isEmpty())
	{
		for (int j = 0; j < m_groupBatchs.count(); ++j)
		{
			if (all || todownload.contains(j))
			{
				if (m_progressBars.length() > j && m_progressBars[j] != nullptr)
				{
					m_progressBars[j]->setValue(0);
					m_progressBars[j]->setMinimum(0);
					// m_progressBars[j]->setMaximum(100);
				}

				DownloadQueryGroup b = m_groupBatchs[j];
				m_batchPending.insert(j, b);
				m_getAllLimit += b.total;
				m_batchDownloading.insert(j);
			}
		}
	}

	if (m_batchPending.isEmpty() && m_getAllRemaining.isEmpty())
	{
		m_getAll = false;
		ui->widgetDownloadButtons->setEnabled(true);
		return;
	}

	m_progressdialog->show();
	getAllLogin();
}

void mainWindow::getAllLogin()
{
	m_progressdialog->clear();
	m_progressdialog->setText(tr("Logging in, please wait..."));

	m_getAllLogins.clear();
	QQueue<Site*> logins;
	for (Downloader *downloader : m_downloaders)
	{
		for (Site *site : downloader->getSites())
		{
			if (!m_getAllLogins.contains(site))
			{
				m_getAllLogins.append(site);
				logins.enqueue(site);
			}
		}
	}

	if (m_getAllLogins.empty())
	{
		getAllFinishedLogins();
		return;
	}

	m_progressdialog->setImagesCount(m_getAllLogins.count());
	while (!logins.isEmpty())
	{
		Site *site = logins.dequeue();
		connect(site, &Site::loggedIn, this, &mainWindow::getAllFinishedLogin);
		site->login();
	}
}
void mainWindow::getAllFinishedLogin(Site *site, Site::LoginResult)
{
	if (m_getAllLogins.empty())
	{ return; }

	m_progressdialog->setImages(m_progressdialog->images() + 1);
	m_getAllLogins.removeAll(site);

	if (m_getAllLogins.empty())
	{ getAllFinishedLogins(); }
}

void mainWindow::getAllFinishedLogins()
{
	bool usePacking = m_settings->value("packing_enable", true).toBool();
	int realConstImagesPerPack = m_settings->value("packing_size", 1000).toInt();

	int total = 0;
	for (int j : m_batchPending.keys())
	{
		DownloadQueryGroup b = m_batchPending[j];

		int constImagesPerPack = usePacking ? realConstImagesPerPack : b.total;
		int pagesPerPack = qCeil((float)constImagesPerPack / b.perpage);
		int imagesPerPack = pagesPerPack * b.perpage;
		int packs = qCeil((float)b.total / imagesPerPack);
		total += b.total;

		int lastPageImages = b.total % imagesPerPack;
		if (lastPageImages == 0)
			lastPageImages = imagesPerPack;

		Downloader *previous = nullptr;
		for (int i = 0; i < packs; ++i)
		{
			Downloader *downloader = new Downloader(m_profile,
													b.tags.split(' '),
													b.postFiltering,
													QList<Site*>() << b.site,
													b.page + i * pagesPerPack,
													(i == packs - 1 ? lastPageImages : imagesPerPack),
													b.perpage,
													b.path,
													b.filename,
													nullptr,
													nullptr,
													b.getBlacklisted,
													m_profile->getBlacklist(),
													false,
													0,
													"",
													previous);
			downloader->setData(j);
			downloader->setQuit(false);

			connect(downloader, &Downloader::finishedImages, this, &mainWindow::getAllFinishedImages);
			connect(downloader, &Downloader::finishedImagesPage, this, &mainWindow::getAllFinishedPage);

			m_waitingDownloaders.enqueue(downloader);
			previous = downloader;
		}
	}

	m_getAllImagesCount = total;
	getNextPack();
}

void mainWindow::getNextPack()
{
	m_downloaders.clear();

	// If there are pending packs
	if (!m_waitingDownloaders.isEmpty())
	{
		m_downloaders.append(m_waitingDownloaders.dequeue());
		getAllGetPages();
	}

	// Only images to download
	else
	{
		m_batchAutomaticRetries = m_settings->value("Save/automaticretries", 0).toInt();
		getAllImages();
	}
}

void mainWindow::getAllGetPages()
{
	m_progressdialog->clear();
	m_progressdialog->setText(tr("Downloading pages, please wait..."));

	for (Downloader *downloader : m_downloaders)
	{
		m_progressdialog->setImagesCount(m_progressdialog->count() + downloader->pagesCount());
		downloader->getImages();
	}
}

/**
 * Called when a page have been loaded and parsed.
 *
 * @param page The loaded page
 */
void mainWindow::getAllFinishedPage(Page *page)
{
	Downloader *d = (Downloader*)QObject::sender();

	int pos = d->getData().toInt();
	m_groupBatchs[pos].unk += (m_groupBatchs[pos].unk == "" ? "" : "¤") + QString::number((quintptr)page);

	m_progressdialog->setImages(m_progressdialog->images() + 1);
}

/**
 * Called when a page have been loaded and parsed.
 *
 * @param images The images results on this page
 */
void mainWindow::getAllFinishedImages(QList<QSharedPointer<Image>> images)
{
	Downloader* downloader = (Downloader*)QObject::sender();
	m_downloaders.removeAll(downloader);
	m_downloadersDone.append(downloader);
	m_getAllIgnored += downloader->ignoredCount();

	m_getAllRemaining.append(images);

	int row = downloader->getData().toInt();
	m_progressBars[row]->setValue(0);
	m_progressBars[row]->setMaximum(images.count());

	// Update image count
	m_getAllImagesCount -= m_batchPending[row].total - images.count();

	if (m_downloaders.isEmpty())
	{
		m_batchAutomaticRetries = m_settings->value("Save/automaticretries", 0).toInt();
		getAllImages();
	}
}

/**
 * Called when all pages have been loaded and parsed from all sources.
 */
void mainWindow::getAllImages()
{
	// Si la limite d'images est dépassée, on retire celles en trop
	while (m_getAllRemaining.count() > m_getAllLimit && !m_getAllRemaining.isEmpty())
		m_getAllRemaining.takeLast()->deleteLater();

	log(QString("All images' urls have been received (%1).").arg(m_getAllRemaining.count()), Logger::Info);

	// We add the images to the download dialog
	int count = 0;
	m_progressdialog->setText(tr("Preparing images, please wait..."));
	m_progressdialog->setCount(m_getAllRemaining.count());
	for (int i = 0; i < m_getAllRemaining.count(); i++)
	{
		// We find the image's batch ID using its page
		int n = -1;
		for (int r = 0; r < m_groupBatchs.count(); r++)
		{
			if (m_groupBatchs[r].unk.split("¤", QString::SkipEmptyParts).contains(QString::number((qintptr)m_getAllRemaining[i]->page())))
			{
				n = r + 1;
				break;
			}
		}

		// We add the image
		m_progressdialog->addImage(m_getAllRemaining[i]->url(), n, m_getAllRemaining[i]->fileSize());
		connect(m_getAllRemaining[i].data(), SIGNAL(urlChanged(QString, QString)), m_progressdialog, SLOT(imageUrlChanged(QString, QString)));
		connect(m_getAllRemaining[i].data(), SIGNAL(urlChanged(QString, QString)), this, SLOT(imageUrlChanged(QString, QString)));

		m_progressdialog->setImages(i+1);
		count += m_getAllRemaining[i]->value();
	}

	// Set some values on the batch window
	m_progressdialog->updateColumns();
	m_progressdialog->setText(tr("Downloading images..."));
	m_progressdialog->setImagesCount(m_getAllImagesCount);
	m_progressdialog->setImages(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors);

	// Check whether we need to get the tags first (for the filename) or if we can just download the images directly
	// TODO: having one batch needing it currently causes all batches to need it, should mae it batch (Downloader) dependent
	m_mustGetTags = needExactTags(m_settings);
	for (int f = 0; f < m_groupBatchs.size() && !m_mustGetTags; f++)
	{
		Filename fn(m_groupBatchs[f].filename);
		Site *site = m_groupBatchs[f].site;
		Api *api = site->firstValidApi();
		QString apiName = api == nullptr ? "" : api->getName();
		if (fn.needExactTags(site, apiName))
			m_mustGetTags = true;
	}
	for (int f = 0; f < m_batchs.size() && !m_mustGetTags; f++)
	{
		Filename fn(m_batchs[f].filename);
		Site *site = m_batchs[f].site;
		Api *api = site->firstValidApi();
		QString apiName = api == nullptr ? "" : api->getName();
		if (fn.needExactTags(site, apiName))
			m_mustGetTags = true;
	}

	if (m_mustGetTags)
		log("Downloading images details.", Logger::Info);
	else
		log("Downloading images directly.", Logger::Info);

	// We start the simultaneous downloads
	for (int i = 0; i < qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10)); i++)
		_getAll();
}

bool mainWindow::needExactTags(QSettings *settings)
{
	auto logFiles = getExternalLogFiles(settings);
	for (int i : logFiles.keys())
	{
		Filename fn(logFiles[i]["content"].toString());
		if (fn.needExactTags())
			return true;
	}

	QStringList settingNames = QStringList()
		<< "Exec/tag_before"
		<< "Exec/image"
		<< "Exec/tag_after"
		<< "Exec/SQL/before"
		<< "Exec/SQL/tag_before"
		<< "Exec/SQL/image"
		<< "Exec/SQL/tag_after"
		<< "Exec/SQL/after";
	for (QString setting : settingNames)
	{
		QString value = settings->value(setting, "").toString();
		if (value.isEmpty())
			continue;

		Filename fn(value);
		if (fn.needExactTags())
			return true;
	}

	return false;
}

void mainWindow::_getAll()
{
	// We quit as soon as the user cancels
	if (m_progressdialog->cancelled())
		return;

	// If there are still images do download
	if (m_getAllRemaining.size() > 0)
	{
		// We take the first image to download
		QSharedPointer<Image> img = m_getAllRemaining.takeFirst();
		m_getAllDownloading.append(img);

		// Get the tags first if necessary
		bool hasUnknownTag = false;
		for (Tag tag : img->tags())
		{
			if (tag.type().name() == "unknown")
			{
				hasUnknownTag = true;
				break;
			}
		}
		if (m_mustGetTags && hasUnknownTag)
		{
			connect(img.data(), &Image::finishedLoadingTags, this, &mainWindow::getAllPerformTags);
			img->loadDetails();
		}
		else
		{
			// Row
			int site_id = m_progressdialog->batch(img->url());
			int row = getRowForSite(site_id);

			// Path
			QString path = m_settings->value("Save/filename").toString();
			QString pth = m_settings->value("Save/path").toString();
			if (site_id >= 0)
			{
				ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/downloading.png"));
				path = m_groupBatchs[site_id - 1].filename;
				pth = m_groupBatchs[site_id - 1].path;
			}

			QString imgPath = img->folder().isEmpty() ? pth : img->folder();
			QStringList paths = img->path(path, imgPath, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1, true, false, true, true, true);

			bool notexists = true;
			for (QString p : paths)
			{
				QFile f(p);
				if (f.exists())
				{ notexists = false; }
			}

			// If the file does not already exists
			if (notexists)
			{
				getAllGetImageIfNotBlacklisted(img, site_id);
			}

			// If the file already exists
			else
			{
				m_getAllExists++;
				log(QString("File already exists: <a href=\"file:///%1\">%1</a>").arg(paths.at(0)), Logger::Info);
				getAllImageOk(img, site_id);
			}
		}
	}

	// When the batch download finishes
	else if (m_getAllDownloading.isEmpty() && m_getAll)
	{ getAllFinished(); }
}

void mainWindow::getAllGetImageIfNotBlacklisted(QSharedPointer<Image> img, int site_id)
{
	// Check if image is blacklisted
	bool detected = !img->blacklisted(m_profile->getBlacklist()).isEmpty();

	if (detected && site_id >= 0 && !m_groupBatchs[site_id - 1].getBlacklisted)
	{
		m_getAllIgnored++;
		log("Image ignored.", Logger::Info);

		getAllImageOk(img, site_id);
	}
	else
	{ getAllGetImage(img); }
}

void mainWindow::getAllImageOk(QSharedPointer<Image> img, int site_id, bool del)
{
	//m_progressdialog->setValue(m_progressdialog->value() + img->value());
	m_progressdialog->setImages(m_progressdialog->images() + 1);

	if (site_id >= 0)
	{
		int row = getRowForSite(site_id);
		m_progressBars[site_id - 1]->setValue(m_progressBars[site_id - 1]->value() + 1);
		if (m_progressBars[site_id - 1]->value() >= m_progressBars[site_id - 1]->maximum())
		{ ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/ok.png")); }
	}

	img->unload();
	m_downloadTimeLast.remove(img->url());
	m_getAllDownloading.removeAll(img);

	if (del)
	{ m_progressdialog->loadedImage(img->url()); }

	_getAll();
}

void mainWindow::imageUrlChanged(QString before, QString after)
{
	m_downloadTimeLast.insert(after, m_downloadTimeLast[before]);
	m_downloadTimeLast.remove(before);
	m_downloadTime.insert(after, m_downloadTime[before]);
	m_downloadTime.remove(before);
}
void mainWindow::getAllProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	Image *img = static_cast<Image*>(sender());
	QString url = img->url();
	if (img->fileSize() == 0)
	{
		img->setFileSize(bytesTotal);
		m_progressdialog->sizeImage(url, bytesTotal);
	}

	if (!m_downloadTimeLast.contains(url) || m_downloadTimeLast[url] == nullptr)
		return;

	if (m_downloadTimeLast[url]->elapsed() >= 1000)
	{
		m_downloadTimeLast[url]->restart();
		int elapsed = m_downloadTime[url]->elapsed();
		float speed = elapsed != 0 ? (bytesReceived * 1000) / elapsed : 0;
		m_progressdialog->speedImage(url, speed);
	}

	m_progressdialog->statusImage(url, bytesTotal != 0 ? (bytesReceived * 100) / bytesTotal : 0);
}
void mainWindow::getAllPerformTags()
{
	if (m_progressdialog->cancelled())
		return;

	log("Tags received", Logger::Info);

	QSharedPointer<Image> img;
	for (QSharedPointer<Image> i : m_getAllDownloading)
		if (i.data() == sender())
			img = i;
	if (img.isNull())
		return;

	// Row
	int site_id = m_progressdialog->batch(img->url());
	int row = getRowForSite(site_id);

	// Getting path
	QString path = m_settings->value("Save/filename").toString();
	QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		path = m_groupBatchs[site_id - 1].filename;
		p = m_groupBatchs[site_id - 1].page;
	}

	// Save path
	p.replace("\\", "/");
	if (p.right(1) == "/")
	{ p = p.left(p.length()-1); }

	int cnt = m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1;
	QStringList paths = img->path(path, p, cnt, true, false, true, true, true);
	QString pth = paths.at(0); // FIXME

	QFile f(pth);
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".png");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".gif");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".jpeg");	}
	if (!f.exists())
	{
		getAllGetImageIfNotBlacklisted(img, site_id);
	}
	else
	{
		//m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_progressdialog->images()+1);
		m_getAllExists++;
		log(QString("File already exists: <a href=\"file:///%1\">%1</a>").arg(f.fileName()), Logger::Info);
		m_progressdialog->loadedImage(img->url());
		if (site_id >= 0)
		{
			m_progressBars[site_id - 1]->setValue(m_progressBars[site_id - 1]->value()+1);
			if (m_progressBars[site_id - 1]->value() >= m_progressBars[site_id - 1]->maximum())
			{ ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/ok.png")); }
		}
		m_downloadTimeLast.remove(img->url());
		m_getAllDownloading.removeAll(img);
		_getAll();
	}
}

int mainWindow::getRowForSite(int site_id)
{
	return site_id - 1;
}

void mainWindow::getAllGetImage(QSharedPointer<Image> img)
{
	// If there is already a downloader for this image, we simply restart it
	if (m_getAllImageDownloaders.contains(img))
	{
		m_getAllImageDownloaders[img]->save();
		return;
	}

	// Row
	int site_id = m_progressdialog->batch(img->url());
	int row = getRowForSite(site_id);

	// Path
	QString filename = img->filename().isEmpty() ? m_settings->value("Save/filename").toString() : img->filename();
	QString path = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/status/downloading.png"));
		filename = m_groupBatchs[site_id - 1].filename;
		path = m_groupBatchs[site_id - 1].path;
	}

	// Track download progress
	m_progressdialog->loadingImage(img->url());
	m_downloadTime.insert(img->url(), new QTime);
	m_downloadTime[img->url()]->start();
	m_downloadTimeLast.insert(img->url(), new QTime);
	m_downloadTimeLast[img->url()]->start();
	connect(img.data(), &Image::downloadProgressImage, this, &mainWindow::getAllProgress, Qt::UniqueConnection);

	// Start loading and saving image
	log(QString("Loading image from <a href=\"%1\">%1</a> %2").arg(img->fileUrl().toString()).arg(m_getAllDownloading.size()), Logger::Info);
	int count = m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1;
	auto imgDownloader = new ImageDownloader(img, filename, path, count, true, false, this);
	connect(imgDownloader, &ImageDownloader::saved, this, &mainWindow::getAllGetImageSaved);
	imgDownloader->save();
	m_getAllImageDownloaders[img] = imgDownloader;
}

void mainWindow::getAllGetImageSaved(QSharedPointer<Image> img, QMap<QString, Image::SaveResult> result)
{
	// Delete ImageDownloader to prevent leaks
	m_getAllImageDownloaders[img]->deleteLater();
	m_getAllImageDownloaders.remove(img);

	// Save error count to compare it later on
	bool del = true;
	bool diskError = false;
	auto res = result.first();

	// Disk writing errors
	for (QString path : result.keys())
	{
		if (result[path] == Image::SaveResult::Error)
		{
			diskError = true;
			m_getAllErrors++;
			m_progressdialog->pause();

			bool isDriveFull = false;
			#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
				QDir destinationDir = QFileInfo(path).absoluteDir();
				QStorageInfo storage(destinationDir);
				isDriveFull = storage.bytesAvailable() < img->fileSize() || storage.bytesAvailable() < 20 * 1024 * 1024;
			#endif

			QString msg;
			if (isDriveFull)
			{ msg = tr("Is seems that the destination directory is full.\n%1\nPlease solve the issue before resuming the download.").arg(path); }
			else
			{ msg = tr("An error occured saving the image.\n%1\nPlease solve the issue before resuming the download.").arg(path); }
			QMessageBox::critical(m_progressdialog, tr("Error"), msg);
		}
	}

	if (res == Image::SaveResult::NotFound)
	{ m_getAll404s++; }
	else if (res == Image::SaveResult::NetworkError)
	{ m_getAllErrors++; }
	else if (res == Image::SaveResult::AlreadyExists)
	{ m_getAllExists++; }
	else if (res == Image::SaveResult::Ignored)
	{ m_getAllIgnored++; }
	else if (!diskError)
	{
		m_getAllDownloaded++;
		m_progressdialog->loadedImage(img->url());
	}
	else
	{
		m_progressdialog->errorImage(img->url());
		m_getAllFailed.append(img);
		del = false;
	}

	int site_id = m_progressdialog->batch(img->url());
	getAllImageOk(img, site_id, del);
}

void mainWindow::getAllCancel()
{
	log("Cancelling downloads...", Logger::Info);
	m_progressdialog->cancel();
	for (QSharedPointer<Image> image : m_getAllDownloading)
	{
		image->abortTags();
		image->abortImage();
	}
	for (Downloader *downloader : m_downloaders)
	{
		downloader->cancel();
	}
	m_getAll = false;
	ui->widgetDownloadButtons->setEnabled(true);
	DONE();
}

void mainWindow::getAllSkip()
{
	log("Skipping downloads...", Logger::Info);

	int count = m_getAllDownloading.count();
	for (QSharedPointer<Image> image : m_getAllDownloading)
	{
		image->abortTags();
		image->abortImage();
	}
	m_getAllSkippedImages.append(m_getAllDownloading);
	m_getAllDownloading.clear();

	m_getAllSkipped += count;
	for (int i = 0; i < count; ++i)
		_getAll();

	DONE();
}

void mainWindow::getAllFinished()
{
	if (!m_waitingDownloaders.isEmpty())
	{
		getNextPack();
		return;
	}

	log("Images download finished.", Logger::Info);
	m_progressdialog->setValue(m_progressdialog->maximum());

	// Delete objects
	for (Downloader *d : m_downloadersDone)
	{
		d->clear();
	}
	qDeleteAll(m_downloadersDone);
	m_downloadersDone.clear();

	// Information about downloads
	if (m_getAllErrors <= 0 || m_batchAutomaticRetries <= 0)
	{
		QMessageBox::information(
			this,
			tr("Getting images"),
			QString(
				tr("%n file(s) downloaded successfully.", "", m_getAllDownloaded)+"\r\n"+
				tr("%n file(s) ignored.", "", m_getAllIgnored)+"\r\n"+
				tr("%n file(s) already existing.", "", m_getAllExists)+"\r\n"+
				tr("%n file(s) not found on the server.", "", m_getAll404s)+"\r\n"+
				tr("%n file(s) skipped.", "", m_getAllSkipped)+"\r\n"+
				tr("%n error(s).", "", m_getAllErrors)
			)
		);
	}

	// Retry in case of error
	int failedCount = m_getAllErrors + m_getAllSkipped;
	if (failedCount > 0)
	{
		int reponse = QMessageBox::No;
		if (m_batchAutomaticRetries > 0)
		{
			m_batchAutomaticRetries--;
			reponse = QMessageBox::Yes;
		}
		else
		{
			int totalCount = m_getAllDownloaded + m_getAllIgnored + m_getAllExists + m_getAll404s + m_getAllErrors + m_getAllSkipped;
			reponse = QMessageBox::question(this, tr("Getting images"), tr("Errors occured during the images download. Do you want to restart the download of those images? (%1/%2)").arg(failedCount).arg(totalCount), QMessageBox::Yes | QMessageBox::No);
		}

		if (reponse == QMessageBox::Yes)
		{
			m_getAll = true;
			m_progressdialog->clear();
			m_getAllRemaining.clear();
			m_getAllRemaining.append(m_getAllFailed);
			m_getAllRemaining.append(m_getAllSkippedImages);
			m_getAllFailed.clear();
			m_getAllSkippedImages.clear();
			m_getAllDownloaded = 0;
			m_getAllExists = 0;
			m_getAllIgnored = 0;
			m_getAll404s = 0;
			m_getAllErrors = 0;
			m_getAllSkipped = 0;
			m_progressdialog->show();
			getAllImages();
			return;
		}
	}

	// Final action
	switch (m_progressdialog->endAction())
	{
		case 1:	m_progressdialog->close();				break;
		case 2:	openTray();								break;
		case 3:	saveFolder();							break;
		case 4:	QSound::play(":/sounds/finished.wav");	break;
		case 5: shutDown();								break;
	}
	activateWindow();
	m_getAll = false;

	// Remove after download and retries are finished
	if (m_progressdialog->endRemove())
	{
		int rem = 0;
		for (int i : m_batchDownloading)
		{
			int pos = i - rem;
			m_progressBars[pos]->deleteLater();
			m_progressBars.removeAt(pos);
			m_groupBatchs.removeAt(pos);
			ui->tableBatchGroups->removeRow(pos);
			rem++;
		}
	}

	// End of batch download
	m_profile->getCommands().after();
	ui->widgetDownloadButtons->setEnabled(true);
	log("Batch download finished", Logger::Info);
}

void mainWindow::getAllPause()
{
	if (m_progressdialog->isPaused())
	{
		log("Pausing downloads...", Logger::Info);
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			m_getAllDownloading[i]->abortTags();
			m_getAllDownloading[i]->abortImage();
		}
		m_getAll = false;
	}
	else
	{
		log("Recovery of downloads...", Logger::Info);
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			getAllGetImage(m_getAllDownloading[i]);
		}
		m_getAll = true;
	}
	DONE();
}

void mainWindow::blacklistFix()
{
	BlacklistFix1 *win = new BlacklistFix1(m_profile, m_sites, this);
	win->show();
}
void mainWindow::emptyDirsFix()
{
	EmptyDirsFix1 *win = new EmptyDirsFix1(m_profile, this);
	win->show();
}
void mainWindow::md5FixOpen()
{
	md5Fix *win = new md5Fix(m_profile, this);
	win->show();
}
void mainWindow::renameExisting()
{
	RenameExisting1 *win = new RenameExisting1(m_profile, m_sites, this);
	win->show();
}
void mainWindow::utilTagLoader()
{
	TagLoader *win = new TagLoader(m_profile, m_sites, this);
	win->show();
}

void mainWindow::on_buttonSaveLinkList_clicked()
{
	QString lastDir = m_settings->value("linksLastDir", "").toString();
	QString save = QFileDialog::getSaveFileName(this, tr("Save link list"), QDir::toNativeSeparators(lastDir), tr("Imageboard-Grabber links (*.igl)"));
	if (save.isEmpty())
	{ return; }

	save = QDir::toNativeSeparators(save);
	m_settings->setValue("linksLastDir", save.section(QDir::toNativeSeparators("/"), 0, -2));

	if (saveLinkList(save))
	{ QMessageBox::information(this, tr("Save link list"), tr("Link list saved successfully!")); }
	else
	{ QMessageBox::critical(this, tr("Save link list"), tr("Error opening file.")); }
}
bool mainWindow::saveLinkList(QString filename)
{
	return DownloadQueryLoader::save(filename, m_batchs, m_groupBatchs);
}

void mainWindow::on_buttonLoadLinkList_clicked()
{
	QString load = QFileDialog::getOpenFileName(this, tr("Load link list"), QString(), tr("Imageboard-Grabber links (*.igl)"));
	if (load.isEmpty())
	{ return; }

	if (loadLinkList(load))
	{ QMessageBox::information(this, tr("Load link list"), tr("Link list loaded successfully!")); }
	else
	{ QMessageBox::critical(this, tr("Load link list"), tr("Error opening file.")); }
}
bool mainWindow::loadLinkList(QString filename)
{
	QList<DownloadQueryImage> newBatchs;
	QList<DownloadQueryGroup> newGroupBatchs;

	if (!DownloadQueryLoader::load(filename, newBatchs, newGroupBatchs, m_sites))
		return false;

	log(tr("Loading %n download(s)", "", newBatchs.count() + newGroupBatchs.count()), Logger::Info);

	m_allow = false;
	for (auto queryImage : newBatchs)
	{
		batchAddUnique(queryImage, false);
	}
	for (auto queryGroup : newGroupBatchs)
	{
		ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount() + 1);
		QString last = queryGroup.unk;
		int max = last.right(last.indexOf("/")+1).toInt(), val = last.left(last.indexOf("/")).toInt();

		int row = ui->tableBatchGroups->rowCount() - 1;
		addTableItem(ui->tableBatchGroups, row, 1, queryGroup.tags);
		addTableItem(ui->tableBatchGroups, row, 2, queryGroup.site->url());
		addTableItem(ui->tableBatchGroups, row, 3, QString::number(queryGroup.page));
		addTableItem(ui->tableBatchGroups, row, 4, QString::number(queryGroup.perpage));
		addTableItem(ui->tableBatchGroups, row, 5, QString::number(queryGroup.total));
		addTableItem(ui->tableBatchGroups, row, 6, queryGroup.filename);
		addTableItem(ui->tableBatchGroups, row, 7, queryGroup.path);
		addTableItem(ui->tableBatchGroups, row, 8, queryGroup.postFiltering.join(' '));
		addTableItem(ui->tableBatchGroups, row, 9, queryGroup.getBlacklisted ? "true" : "false");

		queryGroup.unk = "true";
		m_groupBatchs.append(queryGroup);
		QTableWidgetItem *it = new QTableWidgetItem(getIcon(":/images/status/"+QString(val == max ? "ok" : (val > 0 ? "downloading" : "pending"))+".png"), "");
		it->setFlags(it->flags() ^ Qt::ItemIsEditable);
		it->setTextAlignment(Qt::AlignCenter);
		ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, 0, it);

		QProgressBar *prog = new QProgressBar(this);
		prog->setMaximum(queryGroup.total);
		prog->setValue(val < 0 || val > max ? 0 : val);
		prog->setMinimum(0);
		prog->setTextVisible(false);
		m_progressBars.append(prog);
		ui->tableBatchGroups->setCellWidget(ui->tableBatchGroups->rowCount()-1, 9, prog);
	}
	m_allow = true;
	updateGroupCount();

	return true;
}

void mainWindow::setWiki(QString wiki)
{
	ui->labelWiki->setText("<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>" + wiki);
}

QIcon& mainWindow::getIcon(QString path)
{
	if (!m_icons.contains(path))
		m_icons.insert(path, QIcon(path));

	return m_icons[path];
}

void mainWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{
		ui->lineFolder->setText(folder);
		updateCompleters();
		saveSettings();
	}
}
void mainWindow::on_buttonSaveSettings_clicked()
{
	QString folder = fixFilename("", ui->lineFolder->text());
	if (!QDir(folder).exists())
		QDir::root().mkpath(folder);

	m_settings->setValue("Save/path_real", folder);
	m_settings->setValue("Save/filename_real", ui->comboFilename->currentText());
	saveSettings();
}
void mainWindow::on_buttonInitSettings_clicked()
{
	// Reload filename history
	QFile f(m_profile->getPath() + "/filenamehistory.txt");
	QStringList filenames;
	if (f.open(QFile::ReadOnly | QFile::Text))
	{
		QString line;
		while ((line = f.readLine()) > 0)
		{
			QString l = line.trimmed();
			if (!l.isEmpty() && !filenames.contains(l))
			{
				filenames.append(l);
				ui->comboFilename->addItem(l);
			}
		}
		f.close();
	}

	// Update quick settings dock
	ui->lineFolder->setText(m_settings->value("Save/path_real").toString());
	ui->comboFilename->setCurrentText(m_settings->value("Save/filename_real").toString());

	// Save settings
	saveSettings();
}
void mainWindow::updateCompleters()
{
	if (ui->lineFolder->text() != m_settings->value("Save/path").toString())
	{
		m_lineFolder_completer.append(ui->lineFolder->text());
		ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer));
	}
	/*if (ui->labelFilename->text() != m_settings->value("Save/filename").toString())
	{
		m_lineFilename_completer.append(ui->lineFilename->text());
		ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	}*/
}
void mainWindow::saveSettings()
{
	// Filename combobox
	QString txt = ui->comboFilename->currentText();
	for (int i = ui->comboFilename->count() - 1; i >= 0; --i)
		if (ui->comboFilename->itemText(i) == txt)
			ui->comboFilename->removeItem(i);
	ui->comboFilename->insertItem(0, txt);
	ui->comboFilename->setCurrentIndex(0);
	QString message;
	Filename fn(ui->comboFilename->currentText());
	fn.isValid(m_profile, &message);
	ui->labelFilename->setText(message);

	// Save filename history
	QFile f(m_profile->getPath() + "/filenamehistory.txt");
	if (f.open(QFile::WriteOnly | QFile::Text | QFile::Truncate))
	{
		for (int i = qMax(0, ui->comboFilename->count() - 50); i < ui->comboFilename->count(); ++i)
			f.write(QString(ui->comboFilename->itemText(i) + "\n").toUtf8());
		f.close();
	}

	// Update settings
	QString folder = fixFilename("", ui->lineFolder->text());
	m_settings->setValue("Save/path", folder);
	m_settings->setValue("Save/filename", ui->comboFilename->currentText());
	m_settings->sync();
}



void mainWindow::loadMd5(QString path, bool newTab, bool background, bool save)
{
	QFile file(path);
	if (file.open(QFile::ReadOnly))
	{
		QString md5 = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
		file.close();

		loadTag("md5:" + md5, newTab, background, save);
	}
}
void mainWindow::loadTag(QString tag, bool newTab, bool background, bool save)
{
	if (tag.startsWith("http://") || tag.startsWith("https://"))
	{
		QDesktopServices::openUrl(tag);
		return;
	}

	if (newTab)
		addTab(tag, background, save);
	else if (m_tabs.count() > 0 && ui->tabWidget->currentIndex() < m_tabs.count())
		m_tabs[ui->tabWidget->currentIndex()]->setTags(tag);
}
void mainWindow::loadTagTab(QString tag)
{ loadTag(tag.isEmpty() ? m_link : tag, true); }
void mainWindow::loadTagNoTab(QString tag)
{ loadTag(tag.isEmpty() ? m_link : tag, false); }
void mainWindow::linkHovered(QString tag)
{
	m_link = tag;
}
void mainWindow::contextMenu()
{
	if (m_link.isEmpty())
		return;

	TagContextMenu *menu = new TagContextMenu(m_link, m_currentTags, QUrl(), m_profile, false, this);
	connect(menu, &TagContextMenu::openNewTab, this, &mainWindow::openInNewTab);
	menu->exec(QCursor::pos());
}
void mainWindow::openInNewTab()
{
	addTab(m_link);
}


void mainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData* mimeData = event->mimeData();

	// Drop a text containing an URL
	if (mimeData->hasText())
	{
		QString url = mimeData->text();
		if (isUrl(url))
		{
			event->acceptProposedAction();
			return;
		}
	}

	// Drop URLs
	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			QString path = urlList.at(i).toLocalFile();
			QFileInfo fileInfo(path);
			if (fileInfo.exists() && fileInfo.isFile())
			{
				event->acceptProposedAction();
				return;
			}
		}
	}
}

void mainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	// Drop a text containing an URL
	if (mimeData->hasText())
	{
		QString url = mimeData->text();
		if (isUrl(url))
		{
			QEventLoop loopLoad;
			QNetworkReply *reply = m_networkAccessManager.get(QNetworkRequest(QUrl(url)));
			connect(reply, &QNetworkReply::finished, &loopLoad, &QEventLoop::quit);
			loopLoad.exec();

			if (reply->error() == QNetworkReply::NoError)
			{
				QString md5 = QCryptographicHash::hash(reply->readAll(), QCryptographicHash::Md5).toHex();
				loadTag("md5:" + md5, true, false);
			}
			return;
		}
	}

	// Drop URLs
	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			loadMd5(urlList.at(i).toLocalFile(), true, false);
			return;
		}
	}
}
