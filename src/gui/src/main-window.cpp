#include "main-window.h"
#include <QCloseEvent>
#include <QCompleter>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QNetworkProxy>
#include <QShortcut>
#include <QStringList>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
	#include <QSysInfo>
#endif
#include <QTimer>
#include <algorithm>
#if defined(Q_OS_WIN)
	#include <cfloat>
	#include "Windows.h"
#endif
#include <qmath.h>
#include <ui_main-window.h>
#include "about-window.h"
#include "analytics.h"
#include "danbooru-downloader-importer.h"
#include "docks/favorites-dock.h"
#include "docks/keep-for-later-dock.h"
#include "docks/settings-dock.h"
#include "docks/tags-dock.h"
#include "docks/wiki-dock.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "downloader/download-queue.h"
#include "functions.h"
#include "helpers.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/favorite.h"
#include "models/filename.h"
#include "models/filtering/post-filter.h"
#include "models/profile.h"
#include "monitoring-center.h"
#include "network/network-reply.h"
#include "settings/options-window.h"
#include "settings/start-window.h"
#include "tabs/downloads-tab.h"
#include "tabs/favorites-tab.h"
#include "tabs/gallery-tab.h"
#include "tabs/log-tab.h"
#include "tabs/monitors-tab.h"
#include "tabs/pool-tab.h"
#include "tabs/search-tab.h"
#include "tabs/tabs-loader.h"
#include "tabs/tag-tab.h"
#include "tag-context-menu.h"
#include "tags/tag-stylist.h"
#include "theme-loader.h"
#include "ui/QAffiche.h"
#include "ui/tab-selector.h"
#include "utils/blacklist-fix/blacklist-fix-1.h"
#include "utils/empty-dirs-fix/empty-dirs-fix-1.h"
#include "utils/logging.h"
#include "utils/md5-database-converter/md5-database-converter.h"
#include "utils/md5-fix/md5-fix.h"
#include "utils/rename-existing/rename-existing-1.h"
#include "utils/tag-loader/tag-loader.h"


MainWindow::MainWindow(Profile *profile)
	: ui(new Ui::MainWindow), m_profile(profile), m_favorites(m_profile->getFavorites()), m_loaded(false), m_languageLoader(savePath("languages/", true, false)), m_currentTab(nullptr)
{}
void MainWindow::init(const QStringList &args, const QMap<QString, QString> &params)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_settings = m_profile->getSettings();
	auto sites = m_profile->getSites();

	m_themeLoader = new ThemeLoader(savePath("themes/", true, false), m_settings, this);
	m_themeLoader->setTheme(m_settings->value("theme", "Default").toString());
	ui->setupUi(this);

	if (m_settings->value("Log/show", true).toBool()) {
		m_logTab = new LogTab(this);
		ui->tabWidget->addTab(m_logTab, m_logTab->windowTitle());
	}

	logSystemInformation(m_profile);

	bool crashed = m_settings->value("crashed", false).toBool();
	m_settings->setValue("crashed", true);
	m_settings->sync();

	// On first launch after setup, we restore the setup's language
	QString setupSettingsFile = savePath("innosetup.ini");
	if (QFile::exists(setupSettingsFile)) {
		QSettings setupSettings(setupSettingsFile, QSettings::IniFormat);
		QString setupLanguage = setupSettings.value("language", "en").toString();

		QSettings associations(savePath("languages/languages.ini"), QSettings::IniFormat);
		associations.beginGroup("innosetup");
		QStringList keys = associations.childKeys();

		// Only if the setup language is available in Grabber
		if (keys.contains(setupLanguage)) {
			m_settings->setValue("language", associations.value(setupLanguage).toString());
		}

		// Remove the setup settings file to not do this every time
		QFile::remove(setupSettingsFile);
	}

	// Load translations
	m_languageLoader.install(qApp);
	m_languageLoader.setLanguage(m_settings->value("language", "English").toString(), m_settings->value("useSystemLocale", true).toBool());

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

	if (m_settings->value("Proxy/use", false).toBool()) {
		bool useSystem = m_settings->value("Proxy/useSystem", false).toBool();
		QNetworkProxyFactory::setUseSystemConfiguration(useSystem);

		if (!useSystem) {
			const QNetworkProxy::ProxyType type = m_settings->value("Proxy/type", "http").toString() == "http"
				? QNetworkProxy::HttpProxy
				: QNetworkProxy::Socks5Proxy;
			const QNetworkProxy proxy(
				type,
				m_settings->value("Proxy/hostName").toString(),
				m_settings->value("Proxy/port").toInt(),
				m_settings->value("Proxy/user").toString(),
				m_settings->value("Proxy/password").toString()
			);
			QNetworkProxy::setApplicationProxy(proxy);
			log(QStringLiteral("Enabling application proxy on host \"%1\" and port %2.").arg(m_settings->value("Proxy/hostName").toString()).arg(m_settings->value("Proxy/port").toInt()), Logger::Info);
		} else {
			log(QStringLiteral("Enabling system-wide proxy."), Logger::Info);
		}
	}

	log(QStringLiteral("Loading sources"), Logger::Debug);
	if (sites.empty()) {
		QMessageBox::critical(this, tr("No source found"), tr("No source found. Do you have a configuration problem? Try to reinstall the program."));
		qApp->quit();
		this->deleteLater();
		return;
	}

	QString srsc;
	QStringList keys = sites.keys();
	for (const QString &key : keys) {
		srsc += (!srsc.isEmpty() ? ", " : "") + key + " (" + sites.value(key)->type() + ")";
	}
	log(QStringLiteral("%1 source%2 found: %3").arg(sites.size()).arg(sites.size() > 1 ? "s" : "", srsc), Logger::Info);

	// System tray icon
	if (m_settings->value("Monitoring/enableTray", false).toBool()) {
		auto quitAction = new QAction(tr("&Quit"), this);
		connect(quitAction, &QAction::triggered, this, &MainWindow::trayClose);

		auto trayIconMenu = new QMenu(this);
		trayIconMenu->addAction(quitAction);

		m_trayIcon = new QSystemTrayIcon(this);
		m_trayIcon->setContextMenu(trayIconMenu);
		m_trayIcon->setIcon(windowIcon());
		m_trayIcon->show();

		connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
		connect(m_trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::trayMessageClicked);
	} else {
		m_trayIcon = nullptr;
	}

	m_settings->beginGroup("Main/Shortcuts");
		ui->actionClosetab->setShortcut(getKeySequence(m_settings, "keyCloseTab", Qt::CTRL + Qt::Key_W));

		QShortcut *actionFocusSearch = new QShortcut(getKeySequence(m_settings, "keyFocusSearch", Qt::CTRL + Qt::Key_L), this);
			connect(actionFocusSearch, &QShortcut::activated, this, &MainWindow::focusSearch);

		QShortcut *actionPrevTab = new QShortcut(getKeySequence(m_settings, "keyPrevTab", Qt::CTRL + Qt::Key_PageDown), this);
			connect(actionPrevTab, &QShortcut::activated, this, &MainWindow::tabPrev);
		QShortcut *actionNextTab = new QShortcut(getKeySequence(m_settings, "keyNextTab", Qt::CTRL + Qt::Key_PageUp), this);
			connect(actionNextTab, &QShortcut::activated, this, &MainWindow::tabNext);

		ui->actionAddtab->setShortcut(getKeySequence(m_settings, "keyNewTab", QKeySequence::AddTab, Qt::CTRL + Qt::Key_T));
		ui->actionQuit->setShortcut(getKeySequence(m_settings, "keyQuit", QKeySequence::Quit, Qt::CTRL + Qt::Key_Q));
		ui->actionFolder->setShortcut(getKeySequence(m_settings, "keyBrowseSave", QKeySequence::Open, Qt::CTRL + Qt::Key_O));
	m_settings->endGroup();

	connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
	connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

	// "Settings" dock
	m_settingsDock = new SettingsDock(m_profile, this);
	connect(this, &MainWindow::tabChanged, m_settingsDock, &SettingsDock::tabChanged);
	ui->dockSettingsLayout->addWidget(m_settingsDock);

	// "Favorites" dock
	auto *favoritesDock = new FavoritesDock(m_profile, this);
	connect(favoritesDock, &FavoritesDock::open, this, &MainWindow::loadTagNoTab);
	connect(favoritesDock, &FavoritesDock::openInNewTab, this, &MainWindow::loadTagTab);
	connect(this, &MainWindow::tabChanged, favoritesDock, &FavoritesDock::tabChanged);
	ui->dockFavoritesLayout->addWidget(favoritesDock);

	// "Keep for later" dock
	auto *kflDock = new KeepForLaterDock(m_profile, this);
	connect(kflDock, &KeepForLaterDock::open, this, &MainWindow::loadTagNoTab);
	connect(kflDock, &KeepForLaterDock::openInNewTab, this, &MainWindow::loadTagTab);
	connect(this, &MainWindow::tabChanged, kflDock, &KeepForLaterDock::tabChanged);
	ui->dockKflLayout->addWidget(kflDock);

	// "Wiki" dock
	auto *wikiDock = new WikiDock(this);
	connect(wikiDock, &WikiDock::open, this, &MainWindow::loadTagNoTab);
	connect(this, &MainWindow::tabChanged, wikiDock, &WikiDock::tabChanged);
	ui->dockWikiLayout->addWidget(wikiDock);

	// "Tags" dock
	auto *tagsDock = new TagsDock(m_profile, this);
	connect(tagsDock, &TagsDock::open, this, &MainWindow::loadTagNoTab);
	connect(tagsDock, &TagsDock::openInNewTab, this, &MainWindow::loadTagTab);
	connect(this, &MainWindow::tabChanged, tagsDock, &TagsDock::tabChanged);
	ui->dockTagsLayout->addWidget(tagsDock);

	// Action on first load
	if (m_settings->value("firstload", true).toBool()) {
		this->onFirstLoad();
		m_settings->setValue("firstload", false);
	}

	// Crash restoration
	m_restore = m_settings->value("start", "restore").toString() == "restore";
	if (crashed) {
		log(QStringLiteral("It seems that Imgbrd-Grabber hasn't shut down properly last time."), Logger::Warning);

		QString msg = tr("It seems that the application was not properly closed for its last use. Do you want to restore your last session?");
		QMessageBox dlg(QMessageBox::Question, QStringLiteral("Grabber"), msg, QMessageBox::Yes | QMessageBox::No);
		dlg.setWindowIcon(windowIcon());
		dlg.setDefaultButton(QMessageBox::Yes);

		int response = dlg.exec();
		m_restore = response == QMessageBox::Yes;
	}

	// Loading last window state, size and position from the settings file
	restoreGeometry(m_settings->value("geometry").toByteArray());
	restoreState(m_settings->value("state").toByteArray());

	// Download queue
	const int maxConcurrency = qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10));
	m_downloadQueue = new DownloadQueue(maxConcurrency, this);

	// Tab bar context menu
	ui->tabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->tabWidget->tabBar(), &QTabBar::customContextMenuRequested, this, &MainWindow::tabContextMenuRequested);

	// Monitors tab
	m_monitoringCenter = new MonitoringCenter(m_profile, m_downloadQueue, m_trayIcon, this);
	m_monitorsTab = new MonitorsTab(m_profile, m_profile->monitorManager(), m_monitoringCenter, this);
	ui->tabWidget->insertTab(m_tabs.size(), m_monitorsTab, m_monitorsTab->windowTitle());
	ui->tabWidget->setCurrentIndex(0);

	// Downloads tab
	m_downloadsTab = new DownloadsTab(m_profile, m_downloadQueue, this);
	ui->tabWidget->insertTab(m_tabs.size(), m_downloadsTab, m_downloadsTab->windowTitle());
	ui->tabWidget->setCurrentIndex(0);

	// "File" actions to load/save downloads list
	connect(ui->actionSaveDownloadsList, &QAction::triggered, m_downloadsTab, &DownloadsTab::saveFile);
	connect(ui->actionLoadDownloadsList, &QAction::triggered, m_downloadsTab, &DownloadsTab::loadFile);

	// Restore download lists
	if (m_restore) {
		m_downloadsTab->loadLinkList(m_profile->getPath() + "/restore.igl");
	}

	// Favorites tab
	m_favoritesTab = new FavoritesTab(m_profile, m_downloadQueue, this);
	connect(m_favoritesTab, &SearchTab::batchAddGroup, m_downloadsTab, &DownloadsTab::batchAddGroup);
	connect(m_favoritesTab, SIGNAL(batchAddUnique(DownloadQueryImage)), m_downloadsTab, SLOT(batchAddUnique(DownloadQueryImage)));
	connect(m_favoritesTab, &SearchTab::titleChanged, this, &MainWindow::updateTabTitle);
	connect(m_favoritesTab, &SearchTab::changed, this, &MainWindow::updateTabs);
	ui->tabWidget->insertTab(m_tabs.size(), m_favoritesTab, m_favoritesTab->windowTitle());
	ui->tabWidget->setCurrentIndex(0);
	favoritesDock->tabChanged(m_favoritesTab);

	// Tab corner widget
	QWidget *cornerWidget = new QWidget(this);
	QLayout *layout = new QHBoxLayout(cornerWidget);
	layout->setContentsMargins(0, 0, 6, 0);
	layout->setSpacing(0);
	ui->tabWidget->setCornerWidget(cornerWidget);

	// Last tab button
	QPushButton *lastTab = new QPushButton(QIcon(":/images/back.png"), "", this);
		lastTab->setFlat(true);
		lastTab->resize(QSize(15, 12));
		layout->addWidget(lastTab);

	// Add tab button
	QPushButton *add = new QPushButton(QIcon(":/images/add.png"), "", this);
		add->setFlat(true);
		add->resize(QSize(12, 12));
		connect(add, SIGNAL(clicked()), this, SLOT(addTab()));
		layout->addWidget(add);

	// Tab selector
	m_tabSelector = new TabSelector(ui->tabWidget, lastTab, this);
		m_tabSelector->setShowTabCount(true); // TODO(Bionus): add a setting to disable tab count
		m_tabSelector->setFlat(true);
		m_tabSelector->markStaticTab(m_favoritesTab);
		m_tabSelector->markStaticTab(m_downloadsTab);
		if (m_logTab != nullptr)
		{ m_tabSelector->markStaticTab(m_logTab); }
		layout->addWidget(m_tabSelector);

	// Load given files
	parseArgs(args, params);

	// Get list of selected sources
	QStringList sav = m_settings->value("sites", "").toStringList();
	for (const QString &key : sav) {
		if (!sites.contains(key)) {
			continue;
		}

		Site *site = sites.value(key);
		connect(site, &Site::loggedIn, this, &MainWindow::initialLoginsFinished);
		m_selectedSites.append(site);
	}

	// Initial login on selected sources
	m_waitForLogin = 0;
	if (m_selectedSites.isEmpty()) {
		initialLoginsDone();
	} else {
		m_waitForLogin += m_selectedSites.count();
		for (Site *site : qAsConst(m_selectedSites)) {
			site->login();
		}
	}

	log(QStringLiteral("End of initialization"), Logger::Debug);
}

void MainWindow::parseArgs(const QStringList &args, const QMap<QString, QString> &params)
{
	// When we use Grabber to open a file
	QStringList tags;
	if (args.count() == 1 && QFile::exists(args[0])) {
		// Load an IGL file
		QFileInfo info(args[0]);
		if (info.suffix() == QLatin1String("igl")) {
			m_downloadsTab->loadLinkList(info.absoluteFilePath());
			m_forcedTab = "downloads";
			return;
		}

		// Search any image by its MD5
		loadMd5(info.absoluteFilePath(), true, false, false);
		return;
	}

	// Other positional arguments are treated as tags
	tags.append(args);
	tags.append(params.value("tags").split(' ', Qt::SkipEmptyParts));
	if (!tags.isEmpty() || m_settings->value("start", "restore").toString() == "firstpage") {
		loadTag(tags.join(' '), true, false, false);
	}
}

void MainWindow::initialLoginsFinished()
{
	auto site = qobject_cast<Site*>(sender());
	disconnect(site, &Site::loggedIn, this, &MainWindow::initialLoginsFinished);

	m_waitForLogin--;
	if (m_waitForLogin != 0) {
		return;
	}

	initialLoginsDone();
}

void MainWindow::initialLoginsDone()
{
	if (m_restore) {
		if (QFile::exists(m_profile->getPath() + "/tabs.txt")) {
			QFile::rename(m_profile->getPath() + "/tabs.txt", m_profile->getPath() + "/tabs.json");
		}
		loadTabs(m_profile->getPath() + "/tabs.json");
	}
	if (m_tabs.isEmpty()) {
		addTab();
	}

	if ((QMetaType::Type) m_forcedTab.type() == QMetaType::QString) {
		QString name = m_forcedTab.toString();
		if (name == "favorites") {
			ui->tabWidget->setCurrentWidget(m_favoritesTab);
		} else if (name == "downloads") {
			ui->tabWidget->setCurrentWidget(m_downloadsTab);
		} else if (name == "monitors") {
			ui->tabWidget->setCurrentWidget(m_monitorsTab);
		} else if (name == "log") {
			ui->tabWidget->setCurrentWidget(m_logTab);
		}
	} else {
		ui->tabWidget->setCurrentIndex(qMax(0, m_forcedTab.toInt()));
	}
	m_forcedTab.clear();

	setCurrentTab(ui->tabWidget->currentWidget());
	m_loaded = true;

	m_monitoringCenter->start();
}

MainWindow::~MainWindow()
{
	m_profile->deleteLater();

	delete ui;
	ui = nullptr;
}

void MainWindow::focusSearch()
{
	auto *tab = dynamic_cast<SearchTab*>(ui->tabWidget->currentWidget());
	if (tab != nullptr) {
		tab->focusSearch();
	}
}

void MainWindow::onFirstLoad()
{
	// Save all default settings
	auto *ow = new OptionsWindow(m_profile, m_themeLoader, this);
	ow->save();
	ow->deleteLater();

	// Detect and Danbooru Downloader settings
	DanbooruDownloaderImporter ddImporter;
	if (ddImporter.isInstalled()) {
		int reponse = QMessageBox::question(this, "", tr("The Mozilla Firefox addon \"Danbooru Downloader\" has been detected on your system. Do you want to load its preferences?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes) {
			ddImporter.import(m_settings);
			return;
		}
	}

	// Open startup window
	auto *swin = new StartWindow(m_profile, this);
	connect(swin, &StartWindow::languageChanged, &m_languageLoader, &LanguageLoader::setLanguage);
	connect(swin, &StartWindow::settingsChanged, m_settingsDock, &SettingsDock::reset);
	connect(swin, &StartWindow::sourceChanged, this, &MainWindow::setSource);
	swin->show();
}

void MainWindow::addTab(const QString &tag, bool background, bool save, SearchTab *source)
{
	auto *w = new TagTab(m_profile, m_downloadQueue, this);
	this->addSearchTab(w, background, save, source);

	if (!tag.isEmpty()) {
		w->setTags(tag);
	} else {
		w->focusSearch();
	}
}
void MainWindow::addPoolTab(int pool, const QString &site, bool background, bool save, SearchTab *source)
{
	auto *w = new PoolTab(m_profile, m_downloadQueue, this);
	this->addSearchTab(w, background, save, source);

	if (!site.isEmpty()) {
		w->setSite(site);
	}
	if (pool != 0) {
		w->setPool(pool, site);
	} else {
		w->focusSearch();
	}
}
void MainWindow::addGalleryTab(Site *site, QSharedPointer<Image> gallery, bool background, bool save, SearchTab *source)
{
	auto *w = new GalleryTab(site, std::move(gallery), m_profile, m_downloadQueue, this);
	this->addSearchTab(w, background, save, source);
}
void MainWindow::addSearchTab(SearchTab *w, bool background, bool save, SearchTab *source)
{
	if (source != nullptr) {
		w->setSources(source->sources());
		w->setImagesPerPage(source->imagesPerPage());
		w->setColumns(source->columns());
		w->setPostFilter(source->postFilter());
	}
	connect(w, &SearchTab::batchAddGroup, m_downloadsTab, &DownloadsTab::batchAddGroup);
	connect(w, SIGNAL(batchAddUnique(DownloadQueryImage)), m_downloadsTab, SLOT(batchAddUnique(DownloadQueryImage)));
	connect(w, &SearchTab::titleChanged, this, &MainWindow::updateTabTitle);
	connect(w, &SearchTab::changed, this, &MainWindow::updateTabs);
	connect(w, &SearchTab::closed, this, &MainWindow::tabClosed);

	QString title = w->windowTitle();
	if (title.isEmpty()) {
		title = tr("New tab");
	}

	int pos = m_loaded ? ui->tabWidget->currentIndex() + (!m_tabs.isEmpty() ? 1 : 0) : m_tabs.count();
	int index = ui->tabWidget->insertTab(pos, w, title);
	m_tabs.append(w);

	m_tabSelector->updateCounter();

	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "", this);
		closeTab->setFlat(true);
		closeTab->resize(QSize(8, 8));
		connect(closeTab, &QPushButton::clicked, w, &SearchTab::deleteLater);
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);

	if (!background) {
		ui->tabWidget->setCurrentIndex(index);
	}

	if (save) {
		saveTabs(m_profile->getPath() + "/tabs.json");
	}
}

bool MainWindow::saveTabs(const QString &filename)
{
	return TabsLoader::save(filename, m_tabs, ui->tabWidget->currentWidget());
}
bool MainWindow::loadTabs(const QString &filename)
{
	QList<SearchTab*> tabs;
	QVariant currentTab;

	if (!TabsLoader::load(filename, tabs, currentTab, m_profile, m_downloadQueue, this)) {
		return false;
	}

	bool preload = m_settings->value("preloadAllTabs", false).toBool();
	for (auto tab : qAsConst(tabs)) {
		addSearchTab(tab, true, false);
		if (!preload) {
			m_tabsWaitingForPreload.append(tab);
		}
	}

	m_forcedTab = currentTab;
	return true;
}
void MainWindow::updateTabTitle(SearchTab *tab)
{
	int index = ui->tabWidget->indexOf(tab);
	const QString oldText = ui->tabWidget->tabText(index);
	const QString newText = tab->windowTitle();
	if (newText != oldText) {
		ui->tabWidget->setTabText(index, newText);
	}
}
void MainWindow::updateTabs()
{
	if (m_loaded) {
		saveTabs(m_profile->getPath() + "/tabs.json");
	}
}
void MainWindow::tabClosed(SearchTab *tab)
{
	if (ui == nullptr) {
		return;
	}

	// Store closed tab information
	QJsonObject obj;
	tab->write(obj);
	m_closedTabs.push(obj);
	if (m_closedTabs.count() > CLOSED_TAB_HISTORY_MAX) {
		m_closedTabs.removeFirst();
	}

	ui->actionRestoreLastClosedTab->setEnabled(true);

	m_tabs.removeAll(tab);
	m_tabSelector->updateCounter();
}
void MainWindow::restoreLastClosedTab()
{
	if (m_closedTabs.isEmpty()) {
		return;
	}

	QJsonObject infos = m_closedTabs.pop();
	SearchTab *tab = TabsLoader::loadTab(infos, m_profile, m_downloadQueue, this, true);
	addSearchTab(tab);

	ui->actionRestoreLastClosedTab->setEnabled(!m_closedTabs.isEmpty());
}
void MainWindow::currentTabChanged(int tab)
{
	Q_UNUSED(tab);

	if (!m_loaded) {
		return;
	}

	setCurrentTab(ui->tabWidget->currentWidget());
}

void MainWindow::setCurrentTab(QWidget *widget)
{
	// Check if it's a special kind of tabs for analytics
	if (qobject_cast<DownloadsTab*>(widget) != nullptr) {
		Analytics::getInstance().sendScreenView("Downloads");
	} else if (qobject_cast<MonitorsTab*>(widget) != nullptr) {
		Analytics::getInstance().sendScreenView("Monitors");
	} else if (qobject_cast<LogTab*>(widget) != nullptr) {
		Analytics::getInstance().sendScreenView("Log");
	}

	// Handle "normal" search tabs
	auto searchTab = qobject_cast<SearchTab*>(widget);
	if (searchTab != nullptr) {
		// The opening of the window does not always load all tabs, leaving some unloaded
		if (m_tabsWaitingForPreload.contains(searchTab)) {
			searchTab->load();
			m_tabsWaitingForPreload.removeAll(searchTab);
		}

		// Emit the event only if the tab actually changed
		if (m_currentTab != searchTab) {
			emit tabChanged(searchTab);
		}
		m_currentTab = searchTab;

		Analytics::getInstance().sendScreenView(searchTab->screenName());
	}
}

void MainWindow::closeCurrentTab()
{
	// Unclosable tabs have a maximum width of 16777214 (default: 16777215)
	auto currentTab = ui->tabWidget->currentWidget();
	if (currentTab->maximumWidth() != 16777214) {
		currentTab->deleteLater();
	}
}

void MainWindow::tabNext()
{
	int index = ui->tabWidget->currentIndex();
	int count = ui->tabWidget->count();
	ui->tabWidget->setCurrentIndex((index + 1) % count);
}
void MainWindow::tabPrev()
{
	int index = ui->tabWidget->currentIndex();
	int count = ui->tabWidget->count();
	ui->tabWidget->setCurrentIndex((index - 1 + count) % count);
}

void MainWindow::saveFolder()
{
	QString path = m_settings->value("Save/path").toString().replace("\\", "/");
	if (path.right(1) == "/") {
		path = path.left(path.length() - 1);
	}
	QDir dir(path);
	if (dir.exists()) {
		showInGraphicalShell(path);
	}
}
void MainWindow::openSettingsFolder()
{
	QDir dir(savePath(""));
	if (dir.exists()) {
		showInGraphicalShell(dir.absolutePath());
	}
}


Site *MainWindow::getSelectedSiteOrDefault()
{
	if (m_selectedSites.isEmpty()) {
		return m_profile->getSites().first();
	}

	return m_selectedSites.first();
}


void MainWindow::changeEvent(QEvent *event)
{
	// Automatically re-translate UI on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}
	// Minimize to tray
	else if (event->type() == QEvent::WindowStateChange && (windowState() & Qt::WindowMinimized)) {
		bool tray = m_settings->value("Monitoring/enableTray", false).toBool();
		bool minimizeToTray = m_settings->value("Monitoring/minimizeToTray", false).toBool();
		if (tray && minimizeToTray && m_trayIcon != nullptr && m_trayIcon->isVisible()) {
			QTimer::singleShot(250, this, SLOT(hide()));
		}
	}

	QMainWindow::changeEvent(event);
}

// Save tabs and settings on close
void MainWindow::closeEvent(QCloseEvent *e)
{
	// Close to tray
	bool tray = m_settings->value("Monitoring/enableTray", false).toBool();
	bool closeToTray = m_settings->value("Monitoring/closeToTray", false).toBool();
	if (tray && closeToTray && m_trayIcon != nullptr && m_trayIcon->isVisible() && !m_closeFromTray) {
		hide();
		e->ignore();
		return;
	}

	// Confirm before closing if there is a batch download or multiple tabs
	if (m_settings->value("confirm_close", true).toBool() && (m_tabs.count() > 1 || m_downloadsTab->isDownloading())) {
		QMessageBox msgBox(this);
		msgBox.setText(tr("Are you sure you want to quit?"));
		msgBox.setIcon(QMessageBox::Warning);
		QCheckBox dontShowCheckBox(tr("Don't ask me again"));
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
		if (response != QMessageBox::Yes) {
			e->ignore();
			return;
		}

		// Remember checkbox
		if (dontShowCheckBox.checkState() == Qt::Checked) {
			m_settings->setValue("confirm_close", false);
		}
	}

	// End analytics session
	Analytics::getInstance().endSession();
	Analytics::getInstance().startSending();

	log(QStringLiteral("Saving..."), Logger::Debug);
		m_downloadsTab->saveLinkList(m_profile->getPath() + "/restore.igl");
		saveTabs(m_profile->getPath() + "/tabs.json");
		m_settings->setValue("state", saveState());
		m_settings->setValue("geometry", saveGeometry());
		m_settings->setValue("crashed", false);
		m_settings->sync();
		QFile(m_settings->fileName()).copy(m_profile->getPath() + "/old/settings."+QString(VERSION)+".ini");
		m_profile->sync();
	DONE();
	m_loaded = false;

	// Ensore the tray icon is hidden quickly on close
	if (m_trayIcon != nullptr && m_trayIcon->isVisible()) {
		m_trayIcon->hide();
	}

	e->accept();
}

void MainWindow::options()
{
	log(QStringLiteral("Opening options window..."), Logger::Debug);

	auto *options = new OptionsWindow(m_profile, m_themeLoader, this);
	connect(options, &OptionsWindow::languageChanged, &m_languageLoader, &LanguageLoader::setLanguage);
	connect(options, &OptionsWindow::settingsChanged, m_settingsDock, &SettingsDock::reset);
	connect(options, &QDialog::accepted, this, &MainWindow::optionsClosed);
	options->show();

	DONE();
}

void MainWindow::optionsClosed()
{
	for (SearchTab *tab : qAsConst(m_tabs)) {
		tab->optionsChanged();
		tab->updateCheckboxes();
	}
}

void MainWindow::setSource(const QString &site)
{
	if (!m_profile->getSites().contains(site)) {
		return;
	}

	m_selectedSites.clear();
	m_selectedSites.append(m_profile->getSites().value(site));

	if (m_tabs.isEmpty()) {
		return;
	}

	m_tabs.first()->saveSources(m_selectedSites);
}

void MainWindow::aboutWebsite()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_WEBSITE_URL) + "/"));
}
void MainWindow::aboutGithub()
{
	QDesktopServices::openUrl(QUrl(PROJECT_GITHUB_URL));
}
void MainWindow::aboutReportBug()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/issues/new"));
}
void MainWindow::aboutDonatePaypal()
{
	QDesktopServices::openUrl(QUrl(PROJECT_DONATE_PAYPAL_URL));
}
void MainWindow::aboutDonatePatreon()
{
	QDesktopServices::openUrl(QUrl(PROJECT_DONATE_PATREON_URL));
}

void MainWindow::aboutAuthor()
{
	AboutWindow *aw = new AboutWindow(QString(VERSION), this);
	aw->show();
}

void MainWindow::blacklistFix()
{
	auto *win = new BlacklistFix1(getSelectedSiteOrDefault(), m_profile, this);
	win->show();
}
void MainWindow::emptyDirsFix()
{
	auto *win = new EmptyDirsFix1(m_profile, this);
	win->show();
}
void MainWindow::md5FixOpen()
{
	auto *win = new Md5Fix(m_profile, this);
	win->show();
}
void MainWindow::renameExisting()
{
	auto *win = new RenameExisting1(getSelectedSiteOrDefault(), m_profile, this);
	win->show();
}
void MainWindow::utilTagLoader()
{
	auto *win = new TagLoader(m_profile);
	win->show();
}
void MainWindow::utilMd5DatabaseConverter()
{
	auto *win = new Md5DatabaseConverter(m_profile);
	win->show();
}

void MainWindow::tabContextMenuRequested(const QPoint &pos)
{
	Q_UNUSED(pos);

	auto *menu = new QMenu(this);
	menu->addAction(ui->actionAddtab);
	menu->addAction(ui->actionRestoreLastClosedTab);
	menu->exec(QCursor::pos());
}



void MainWindow::loadMd5(const QString &path, bool newTab, bool background, bool save, SearchTab *source)
{
	QFile file(path);
	if (file.open(QFile::ReadOnly)) {
		QString md5 = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
		file.close();

		loadTag("md5:" + md5, newTab, background, save, source);
	}
}
void MainWindow::loadTag(const QString &tag, bool newTab, bool background, bool save, SearchTab *source)
{
	if (tag.startsWith("http://") || tag.startsWith("https://")) {
		QDesktopServices::openUrl(tag);
		return;
	}

	if (newTab || m_tabs.isEmpty()) {
		addTab(tag, background, save, source);
	} else if (m_tabs.count() > 0 && ui->tabWidget->currentIndex() < m_tabs.count()) {
		m_tabs[ui->tabWidget->currentIndex()]->setTags(tag);
	}
}
void MainWindow::loadTagTab(const QString &tag)
{ loadTag(tag.isEmpty() ? m_link : QUrl::fromPercentEncoding(tag.toUtf8()), true); }
void MainWindow::loadTagNoTab(const QString &tag)
{ loadTag(tag.isEmpty() ? m_link : QUrl::fromPercentEncoding(tag.toUtf8()), false); }


void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
		showNormal();
	}
}

void MainWindow::trayMessageClicked()
{
	// No op
}

void MainWindow::trayClose()
{
	m_closeFromTray = true;
	close();
	m_closeFromTray = false;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	// Drop a text containing an URL
	if (mimeData->hasText()) {
		QString url = mimeData->text();
		if (isUrl(url)) {
			event->acceptProposedAction();
			return;
		}
	}

	// Drop URLs
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i) {
			QString path = urlList.at(i).toLocalFile();
			QFileInfo fileInfo(path);
			if (fileInfo.exists() && fileInfo.isFile()) {
				event->acceptProposedAction();
				return;
			}
		}
	}
}

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	// Drop a text containing an URL
	if (mimeData->hasText()) {
		QString url = mimeData->text();
		if (isUrl(url)) {
			QEventLoop loopLoad;
			NetworkReply *reply = m_networkManager.get(QNetworkRequest(QUrl(url)));
			connect(reply, &NetworkReply::finished, &loopLoad, &QEventLoop::quit);
			loopLoad.exec();

			if (reply->error() == NetworkReply::NetworkError::NoError) {
				QString md5 = QCryptographicHash::hash(reply->readAll(), QCryptographicHash::Md5).toHex();
				loadTag("md5:" + md5, true, false);
			}
			return;
		}
	}

	// Drop URLs
	if (mimeData->hasUrls()) {
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i) {
			const QString local = urlList.at(i).toLocalFile();
			if (QFileInfo(local).suffix() == "igl") {
				m_downloadsTab->loadLinkList(local);
			} else {
				loadMd5(local, true, false);
			}
		}
	}
}
