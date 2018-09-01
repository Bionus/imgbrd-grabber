#include "main-window.h"
#include <QCloseEvent>
#include <QCompleter>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QNetworkProxy>
#include <QShortcut>
#include <QTimer>
#include <algorithm>
#if defined(Q_OS_WIN)
	#include <cfloat>
	#include "Windows.h"
#endif
#include <qmath.h>
#include <ui_main-window.h>
#include "about-window.h"
#include "danbooru-downloader-importer.h"
#include "downloader/download-query-group.h"
#include "downloader/download-query-image.h"
#include "functions.h"
#include "helpers.h"
#include "models/api/api.h"
#include "models/favorite.h"
#include "models/filename.h"
#include "models/filtering/post-filter.h"
#include "models/profile.h"
#include "monitoring-center.h"
#include "settings/options-window.h"
#include "settings/start-window.h"
#include "tabs/downloads-tab.h"
#include "tabs/favorites-tab.h"
#include "tabs/gallery-tab.h"
#include "tabs/log-tab.h"
#include "tabs/pool-tab.h"
#include "tabs/search-tab.h"
#include "tabs/tabs-loader.h"
#include "tabs/tag-tab.h"
#include "tag-context-menu.h"
#include "tags/tag-stylist.h"
#include "theme-loader.h"
#include "ui/QAffiche.h"
#include "utils/blacklist-fix/blacklist-fix-1.h"
#include "utils/empty-dirs-fix/empty-dirs-fix-1.h"
#include "utils/md5-fix/md5-fix.h"
#include "utils/rename-existing/rename-existing-1.h"
#include "utils/tag-loader/tag-loader.h"


MainWindow::MainWindow(Profile *profile)
	: ui(new Ui::MainWindow), m_profile(profile), m_favorites(m_profile->getFavorites()), m_loaded(false), m_forcedTab(-1), m_languageLoader(savePath("languages/", true)), m_currentTab(nullptr)
{ }
void MainWindow::init(const QStringList &args, const QMap<QString, QString> &params)
{
	m_settings = m_profile->getSettings();
	auto sites = m_profile->getSites();

	ThemeLoader themeLoader(savePath("themes/", true));
	themeLoader.setTheme(m_settings->value("theme", "Default").toString());
	ui->setupUi(this);

	if (m_settings->value("Log/show", true).toBool())
	{ ui->tabWidget->addTab(new LogTab(this), tr("Log")); }

	log(QStringLiteral("New session started."), Logger::Info);
	log(QStringLiteral("Software version: %1.").arg(VERSION), Logger::Info);
	log(QStringLiteral("Path: `%1`").arg(qApp->applicationDirPath()), Logger::Info);
	log(QStringLiteral("Loading preferences from `%1`").arg(m_settings->fileName()), Logger::Info);

	if (!QSslSocket::supportsSsl())
	{ log(QStringLiteral("Missing SSL libraries"), Logger::Error); }
	else
	{ log(QStringLiteral("SSL libraries: %1").arg(QSslSocket::sslLibraryVersionString()), Logger::Info); }

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
	m_languageLoader.install(qApp);
	m_languageLoader.setLanguage(m_settings->value("language", "English").toString());

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
		}
		else
		{ log(QStringLiteral("Enabling system-wide proxy."), Logger::Info); }
	}

	log(QStringLiteral("Loading sources"), Logger::Debug);
	if (sites.empty())
	{
		QMessageBox::critical(this, tr("No source found"), tr("No source found. Do you have a configuration problem? Try to reinstall the program."));
		qApp->quit();
		this->deleteLater();
		return;
	}

	QString srsc;
	QStringList keys = sites.keys();
	for (const QString &key : keys)
	{ srsc += (!srsc.isEmpty() ? ", " : "") + key + " (" + sites.value(key)->type() + ")"; }
	log(QStringLiteral("%1 source%2 found: %3").arg(sites.size()).arg(sites.size() > 1 ? "s" : "", srsc), Logger::Info);

	// System tray icon
	if (m_settings->value("Monitoring/enableTray", false).toBool())
	{
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
	}
	else
	{ m_trayIcon = nullptr; }

	ui->actionClosetab->setShortcut(QKeySequence::Close);
	QShortcut *actionCloseTabW = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this);
	connect(actionCloseTabW, &QShortcut::activated, ui->actionClosetab, &QAction::trigger);

	QShortcut *actionFocusSearch = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
	connect(actionFocusSearch, &QShortcut::activated, this, &MainWindow::focusSearch);

	QShortcut *actionNextTab = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageDown), this);
	connect(actionNextTab, &QShortcut::activated, this, &MainWindow::tabNext);
	QShortcut *actionPrevTab = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_PageUp), this);
	connect(actionPrevTab, &QShortcut::activated, this, &MainWindow::tabPrev);

	ui->actionAddtab->setShortcut(QKeySequence::AddTab);
	ui->actionQuit->setShortcut(QKeySequence::Quit);
	ui->actionFolder->setShortcut(QKeySequence::Open);

	connect(ui->actionQuit, &QAction::triggered, this, &QMainWindow::close);
	connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

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

	// Downloads tab
	m_downloadsTab = new DownloadsTab(m_profile, this);
	ui->tabWidget->insertTab(m_tabs.size(), m_downloadsTab, tr("Downloads"));
	ui->tabWidget->setCurrentIndex(0);

	// Restore download lists
	if (m_restore)
	{ m_downloadsTab->loadLinkList(m_profile->getPath() + "/restore.igl"); }

	// Tab add button
	QPushButton *add = new QPushButton(QIcon(":/images/add.png"), "", this);
		add->setFlat(true);
		add->resize(QSize(12, 12));
		connect(add, SIGNAL(clicked()), this, SLOT(addTab()));
		ui->tabWidget->setCornerWidget(add);

	// Favorites tab
	m_favoritesTab = new FavoritesTab(m_profile, this);
	connect(m_favoritesTab, &SearchTab::batchAddGroup, m_downloadsTab, &DownloadsTab::batchAddGroup);
	connect(m_favoritesTab, SIGNAL(batchAddUnique(DownloadQueryImage)), m_downloadsTab, SLOT(batchAddUnique(DownloadQueryImage)));
	connect(m_favoritesTab, &SearchTab::titleChanged, this, &MainWindow::updateTabTitle);
	connect(m_favoritesTab, &SearchTab::changed, this, &MainWindow::updateTabs);
	ui->tabWidget->insertTab(m_tabs.size(), m_favoritesTab, tr("Favorites"));
	ui->tabWidget->setCurrentIndex(0);

	// Load given files
	parseArgs(args, params);

	// Get list of selected sources
	QStringList sav = m_settings->value("sites", "").toStringList();
	for (const QString &key : sav)
	{
		if (!sites.contains(key))
			continue;

		Site *site = sites.value(key);
		connect(site, &Site::loggedIn, this, &MainWindow::initialLoginsFinished);
		m_selectedSites.append(site);
	}

	// Initial login on selected sources
	m_waitForLogin = 0;
	if (m_selectedSites.isEmpty())
	{
		initialLoginsDone();
	}
	else
	{
		m_waitForLogin += m_selectedSites.count();
		for (Site *site : qAsConst(m_selectedSites))
			site->login();
	}

	on_buttonInitSettings_clicked();

	m_lineFolder_completer = QStringList(m_settings->value("Save/path").toString());
	ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer, ui->lineFolder));
	//m_lineFilename_completer = QStringList(m_settings->value("Save/filename").toString());
	//ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	ui->comboFilename->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);

	connect(m_profile, &Profile::favoritesChanged, this, &MainWindow::updateFavorites);
	connect(m_profile, &Profile::keptForLaterChanged, this, &MainWindow::updateKeepForLater);
	updateFavorites();
	updateKeepForLater();

	log(QStringLiteral("End of initialization"), Logger::Debug);
}

void MainWindow::parseArgs(const QStringList &args, const QMap<QString, QString> &params)
{
	// When we use Grabber to open a file
	QStringList tags;
	if (args.count() == 1 && QFile::exists(args[0]))
	{
		// Load an IGL file
		QFileInfo info(args[0]);
		if (info.suffix() == QLatin1String("igl"))
		{
			m_downloadsTab->loadLinkList(info.absoluteFilePath());
			m_forcedTab = m_tabs.size() + 1;
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
		loadTag(tags.join(' '), true, false, false);
	}
}

void MainWindow::initialLoginsFinished()
{
	auto site = qobject_cast<Site*>(sender());
	disconnect(site, &Site::loggedIn, this, &MainWindow::initialLoginsFinished);

	m_waitForLogin--;
	if (m_waitForLogin != 0)
		return;

	initialLoginsDone();
}

void MainWindow::initialLoginsDone()
{
	if (m_restore)
	{ loadTabs(m_profile->getPath() + "/tabs.txt"); }
	if (m_tabs.isEmpty())
	{ addTab(); }

	m_currentTab = qobject_cast<SearchTab*>(ui->tabWidget->currentWidget());
	m_loaded = true;

	ui->tabWidget->setCurrentIndex(qMax(0, m_forcedTab));
	m_forcedTab = -1;

	m_monitoringCenter = new MonitoringCenter(m_profile, m_trayIcon, this);
	m_monitoringCenter->start();
}

MainWindow::~MainWindow()
{
	delete m_profile;
	delete ui;
}

void MainWindow::focusSearch()
{
	auto *tab = dynamic_cast<SearchTab*>(ui->tabWidget->currentWidget());
	if (tab != nullptr)
	{
		tab->focusSearch();
	}
}

void MainWindow::onFirstLoad()
{
	// Save all default settings
	auto *ow = new OptionsWindow(m_profile, this);
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
	auto *swin = new StartWindow(m_profile, this);
	connect(swin, &StartWindow::languageChanged, &m_languageLoader, &LanguageLoader::setLanguage);
	connect(swin, &StartWindow::settingsChanged, this, &MainWindow::on_buttonInitSettings_clicked);
	connect(swin, &StartWindow::sourceChanged, this, &MainWindow::setSource);
	swin->show();
}

void MainWindow::addTab(const QString &tag, bool background, bool save)
{
	auto *w = new TagTab(m_profile, this);
	this->addSearchTab(w, background, save);

	if (!tag.isEmpty())
	{ w->setTags(tag); }
	else
	{ w->focusSearch(); }
}
void MainWindow::addPoolTab(int pool, const QString &site, bool background, bool save)
{
	auto *w = new PoolTab(m_profile, this);
	this->addSearchTab(w, background, save);

	if (!site.isEmpty())
	{ w->setSite(site); }
	if (pool != 0)
	{ w->setPool(pool, site); }
	else
	{ w->focusSearch(); }
}
void MainWindow::addGalleryTab(Site *site, QString name, QString id, bool background, bool save)
{
	auto *w = new GalleryTab(site, std::move(name), std::move(id), m_profile, this);
	this->addSearchTab(w, background, save);
}
void MainWindow::addSearchTab(SearchTab *w, bool background, bool save)
{
	if (m_tabs.size() > ui->tabWidget->currentIndex())
	{
		w->setSources(m_tabs[ui->tabWidget->currentIndex()]->sources());
		w->setImagesPerPage(m_tabs[ui->tabWidget->currentIndex()]->imagesPerPage());
		w->setColumns(m_tabs[ui->tabWidget->currentIndex()]->columns());
		w->setPostFilter(m_tabs[ui->tabWidget->currentIndex()]->postFilter());
	}
	connect(w, &SearchTab::batchAddGroup, m_downloadsTab, &DownloadsTab::batchAddGroup);
	connect(w, SIGNAL(batchAddUnique(DownloadQueryImage)), m_downloadsTab, SLOT(batchAddUnique(DownloadQueryImage)));
	connect(w, &SearchTab::titleChanged, this, &MainWindow::updateTabTitle);
	connect(w, &SearchTab::changed, this, &MainWindow::updateTabs);
	connect(w, &SearchTab::closed, this, &MainWindow::tabClosed);

	QString title = w->windowTitle();
	if (title.isEmpty())
	{ title = "New tab"; }

	int pos = m_loaded ? ui->tabWidget->currentIndex() + (!m_tabs.isEmpty() ? 1 : 0) : m_tabs.count();
	int index = ui->tabWidget->insertTab(pos, w, title);
	m_tabs.append(w);

	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "", this);
		closeTab->setFlat(true);
		closeTab->resize(QSize(8, 8));
		connect(closeTab, &QPushButton::clicked, w, &SearchTab::deleteLater);
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);

	if (!background)
		ui->tabWidget->setCurrentIndex(index);

	if (save)
		saveTabs(m_profile->getPath() + "/tabs.txt");
}

bool MainWindow::saveTabs(const QString &filename)
{
	auto currentSearchTab = qobject_cast<SearchTab*>(ui->tabWidget->currentWidget());
	return TabsLoader::save(filename, m_tabs, currentSearchTab);
}
bool MainWindow::loadTabs(const QString &filename)
{
	QList<SearchTab*> tabs;
	int currentTab;

	if (!TabsLoader::load(filename, tabs, currentTab, m_profile, this))
		return false;

	bool preload = m_settings->value("preloadAllTabs", false).toBool();
	for (auto tab : qAsConst(tabs))
	{
		addSearchTab(tab, true, false);
		if (!preload)
			m_tabsWaitingForPreload.append(tab);
	}

	m_forcedTab = currentTab;
	return true;
}
void MainWindow::updateTabTitle(SearchTab *tab)
{
	ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), tab->windowTitle());
}
void MainWindow::updateTabs()
{
	if (m_loaded)
	{
		saveTabs(m_profile->getPath() + "/tabs.txt");
	}
}
void MainWindow::tabClosed(SearchTab *tab)
{
	// Store closed tab information
	QJsonObject obj;
	tab->write(obj);
	m_closedTabs.append(obj);
	if (m_closedTabs.count() > CLOSED_TAB_HISTORY_MAX)
	{
		m_closedTabs.removeFirst();
	}
	ui->actionRestoreLastClosedTab->setEnabled(true);

	m_tabs.removeAll(tab);
}
void MainWindow::restoreLastClosedTab()
{
	if (m_closedTabs.isEmpty())
		return;

	QJsonObject infos = m_closedTabs.takeLast();
	SearchTab *tab = TabsLoader::loadTab(infos, m_profile, this, true);
	addSearchTab(tab);

	ui->actionRestoreLastClosedTab->setEnabled(!m_closedTabs.isEmpty());
}
void MainWindow::currentTabChanged(int tab)
{
	if (m_loaded && tab < m_tabs.size())
	{
		auto currentSearchTab = qobject_cast<SearchTab*>(ui->tabWidget->currentWidget());
		if (currentSearchTab != nullptr)
		{
			SearchTab *tb = m_tabs[tab];
			if (m_tabsWaitingForPreload.contains(tb))
			{
				tb->load();
				m_tabsWaitingForPreload.removeAll(tb);
			}
			else if (m_currentTab != currentSearchTab)
			{
				setTags(tb->results());
				setWiki(tb->wiki());
			}
			m_currentTab = currentSearchTab;
		}
	}
}

void MainWindow::setTags(const QList<Tag> &tags, SearchTab *from)
{
	if (from != nullptr && m_tabs.indexOf(from) != ui->tabWidget->currentIndex())
		return;

	clearLayout(ui->dockInternetScrollLayout);
	m_currentTags = tags;

	QAffiche *taglabel = new QAffiche(QVariant(), 0, QColor(), this);
	taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	connect(taglabel, static_cast<void (QAffiche::*)(const QString &)>(&QAffiche::middleClicked), this, &MainWindow::loadTagTab);
	connect(taglabel, &QAffiche::linkHovered, this, &MainWindow::linkHovered);
	connect(taglabel, &QAffiche::linkActivated, this, &MainWindow::loadTagNoTab);
	taglabel->setText(TagStylist(m_profile).stylished(tags, true, true).join("<br/>"));

	// Context menu
	taglabel->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(taglabel, &QWidget::customContextMenuRequested, this, &MainWindow::contextMenu);

	ui->dockInternetScrollLayout->addWidget(taglabel);
}

void MainWindow::closeCurrentTab()
{
	// Unclosable tabs have a maximum width of 16777214 (default: 16777215)
	auto currentTab = ui->tabWidget->currentWidget();
	if (currentTab->maximumWidth() != 16777214)
	{ currentTab->deleteLater(); }
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
	if (path.right(1) == "/")
	{ path = path.left(path.length() - 1); }
	QDir dir(path);
	if (dir.exists())
	{ showInGraphicalShell(path); }
}
void MainWindow::openSettingsFolder()
{
	QDir dir(savePath(""));
	if (dir.exists())
	{ showInGraphicalShell(dir.absolutePath()); }
}


Site *MainWindow::getSelectedSiteOrDefault()
{
	if (m_selectedSites.isEmpty())
		return m_profile->getSites().first();

	return m_selectedSites.first();
}

void MainWindow::updateFavorites()
{
	clearLayout(ui->layoutFavoritesDock);

	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[qMax(ui->comboOrderFav->currentIndex(), 0)];
	bool reverse = (ui->comboAscFav->currentIndex() == 1);

	if (order == "note")
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByNote); }
	else if (order == "lastviewed")
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByLastViewed); }
	else
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByName); }
	if (reverse)
	{ m_favorites = reversed(m_favorites); }
	QString format = tr("MM/dd/yyyy");

	for (const Favorite &fav : qAsConst(m_favorites))
	{
		QLabel *lab = new QLabel(QString(R"(<a href="%1" style="color:black;text-decoration:none;">%2</a>)").arg(fav.getName(), fav.getName()), this);
		connect(lab, SIGNAL(linkActivated(QString)), this, SLOT(loadTag(QString)));
		lab->setToolTip("<img src=\""+fav.getImagePath()+"\" /><br/>"+tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %%<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(format)));
		ui->layoutFavoritesDock->addWidget(lab);
	}
}
void MainWindow::updateKeepForLater()
{
	QStringList kfl = m_profile->getKeptForLater();

	clearLayout(ui->dockKflScrollLayout);

	for (const QString &tag : kfl)
	{
		auto *taglabel = new QAffiche(QString(tag), 0, QColor(), this);
		taglabel->setText(QString(R"(<a href="%1" style="color:black;text-decoration:none;">%1</a>)").arg(tag));
		taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
		connect(taglabel, static_cast<void (QAffiche::*)(const QString &)>(&QAffiche::middleClicked), this, &MainWindow::loadTagTab);
		connect(taglabel, &QAffiche::linkActivated, this, &MainWindow::loadTagNoTab);
		ui->dockKflScrollLayout->addWidget(taglabel);
	}
}


void MainWindow::changeEvent(QEvent *event)
{
	// Automatically re-translate UI on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
	}

	// Minimize to tray
	else if (event->type() == QEvent::WindowStateChange && (windowState() & Qt::WindowMinimized))
	{
		bool tray = m_settings->value("Monitoring/enableTray", false).toBool();
		bool minimizeToTray = m_settings->value("Monitoring/minimizeToTray", false).toBool();
		if (tray && minimizeToTray && m_trayIcon != nullptr && m_trayIcon->isVisible())
		{
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
	if (tray && closeToTray && m_trayIcon != nullptr && m_trayIcon->isVisible() && !m_closeFromTray)
	{
		hide();
		e->ignore();
		return;
	}

	// Confirm before closing if there is a batch download or multiple tabs
	if (m_settings->value("confirm_close", true).toBool() && (m_tabs.count() > 1 || m_downloadsTab->isDownloading()))
	{
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
		if (response != QMessageBox::Yes)
		{
			e->ignore();
			return;
		}

		// Remember checkbox
		if (dontShowCheckBox.checkState() == Qt::Checked)
		{ m_settings->setValue("confirm_close", false); }
	}

	log(QStringLiteral("Saving..."), Logger::Debug);
		m_downloadsTab->saveLinkList(m_profile->getPath() + "/restore.igl");
		saveTabs(m_profile->getPath() + "/tabs.txt");
		m_settings->setValue("state", saveState());
		m_settings->setValue("geometry", saveGeometry());
		m_settings->setValue("crashed", false);
		m_settings->sync();
		QFile(m_settings->fileName()).copy(m_profile->getPath() + "/old/settings."+QString(VERSION)+".ini");
		m_profile->sync();
	DONE();
	m_loaded = false;

	// Ensore the tray icon is hidden quickly on close
	if (m_trayIcon != nullptr && m_trayIcon->isVisible())
		m_trayIcon->hide();

	e->accept();
	qApp->quit();
}

void MainWindow::options()
{
	log(QStringLiteral("Opening options window..."), Logger::Debug);

	auto *options = new OptionsWindow(m_profile, this);
	connect(options, &OptionsWindow::languageChanged, &m_languageLoader, &LanguageLoader::setLanguage);
	connect(options, &OptionsWindow::settingsChanged, this, &MainWindow::on_buttonInitSettings_clicked);
	connect(options, &QDialog::accepted, this, &MainWindow::optionsClosed);
	options->show();

	DONE();
}

void MainWindow::optionsClosed()
{
	for (SearchTab *tab : qAsConst(m_tabs))
	{
		tab->optionsChanged();
		tab->updateCheckboxes();
	}
}

void MainWindow::setSource(const QString &site)
{
	if (!m_profile->getSites().contains(site))
		return;

	m_selectedSites.clear();
	m_selectedSites.append(m_profile->getSites().value(site));

	if (m_tabs.isEmpty())
		return;

	m_tabs.first()->saveSources(m_selectedSites);
}

void MainWindow::aboutWebsite()
{
	QDesktopServices::openUrl(QUrl(PROJECT_WEBSITE_URL));
}
void MainWindow::aboutGithub()
{
	QDesktopServices::openUrl(QUrl(PROJECT_GITHUB_URL));
}
void MainWindow::aboutReportBug()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/issues/new"));
}

void MainWindow::aboutAuthor()
{
	AboutWindow *aw = new AboutWindow(QString(VERSION), this);
	aw->show();
}

void MainWindow::blacklistFix()
{
	auto *win = new BlacklistFix1(m_profile, this);
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

void MainWindow::setWiki(const QString &wiki, SearchTab *from)
{
	if (from != nullptr && from != m_currentTab)
		return;

	ui->labelWiki->setText("<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>" + wiki);
}

void MainWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choose a save folder"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{
		ui->lineFolder->setText(folder);
		updateCompleters();
		saveSettings();
	}
}
void MainWindow::on_buttonSaveSettings_clicked()
{
	QString folder = fixFilename("", ui->lineFolder->text());
	if (!QDir(folder).exists())
		QDir::root().mkpath(folder);

	m_settings->setValue("Save/path_real", folder);
	m_settings->setValue("Save/filename_real", ui->comboFilename->currentText());
	saveSettings();
}
void MainWindow::on_buttonInitSettings_clicked()
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
void MainWindow::updateCompleters()
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
void MainWindow::saveSettings()
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



void MainWindow::loadMd5(const QString &path, bool newTab, bool background, bool save)
{
	QFile file(path);
	if (file.open(QFile::ReadOnly))
	{
		QString md5 = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
		file.close();

		loadTag("md5:" + md5, newTab, background, save);
	}
}
void MainWindow::loadTag(const QString &tag, bool newTab, bool background, bool save)
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
void MainWindow::loadTagTab(const QString &tag)
{ loadTag(tag.isEmpty() ? m_link : tag, true); }
void MainWindow::loadTagNoTab(const QString &tag)
{ loadTag(tag.isEmpty() ? m_link : tag, false); }
void MainWindow::linkHovered(const QString &tag)
{
	m_link = tag;
}
void MainWindow::contextMenu()
{
	if (m_link.isEmpty())
		return;

	TagContextMenu *menu = new TagContextMenu(m_link, m_currentTags, QUrl(), m_profile, false, this);
	connect(menu, &TagContextMenu::openNewTab, this, &MainWindow::openInNewTab);
	menu->exec(QCursor::pos());
}
void MainWindow::openInNewTab()
{
	addTab(m_link);
}


void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
	{
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

void MainWindow::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

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
		}
	}
}
