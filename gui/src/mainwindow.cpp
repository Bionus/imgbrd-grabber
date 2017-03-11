#include <QtXml>
#include <QtScript>
#include <QMessageBox>
#include <QFileDialog>
#include <QSound>
#include <QShortcut>
#include <QtNetwork>
#include <QDesktopServices>
#include <QCloseEvent>
#include <QScrollBar>
#include <QtSql/QSqlDatabase>
#if defined(Q_OS_WIN)
	#include "windows.h"
	#include <float.h>
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_tag-tab.h"
#include "ui_pool-tab.h"
#include "settings/optionswindow.h"
#include "settings/startwindow.h"
#include "favoritewindow.h"
#include "batch/addgroupwindow.h"
#include "batch/adduniquewindow.h"
#include "batch/batchwindow.h"
#include "aboutwindow.h"
#include "functions.h"
#include "commands/commands.h"
#include "utils/blacklist-fix/blacklist-fix-1.h"
#include "utils/rename-existing/rename-existing-1.h"
#include "utils/empty-dirs-fix/empty-dirs-fix-1.h"
#include "utils/md5-fix/md5-fix.h"
#include "models/filename.h"
#include "helpers.h"
#include "downloader/download-query-loader.h"
#include "tabs/tabs-loader.h"



mainWindow::mainWindow(Profile *profile)
	: ui(new Ui::mainWindow), m_profile(profile), m_favorites(m_profile->getFavorites()), m_downloads(0), m_loaded(false), m_getAll(false), m_forcedTab(false), m_batchAutomaticRetries(0), m_showLog(true)
{ }
void mainWindow::init(QStringList args, QMap<QString,QString> params)
{
	m_settings = m_profile->getSettings();
	bool crashed = m_settings->value("crashed", false).toBool();

	m_settings->setValue("crashed", true);
	m_settings->sync();

	ui->setupUi(this);

	m_showLog = m_settings->value("Log/show", true).toBool();
	if (!m_showLog)
	{ ui->tabWidget->removeTab(ui->tabWidget->indexOf(ui->tabLog)); }

	log("New session started.");
	log(QString("Software version: %1.").arg(VERSION));
	log(QString("Path: %1").arg(qApp->applicationDirPath()));
	log(QString("Loading preferences from <a href=\"file:///%1\">%1</a>").arg(m_settings->fileName()));

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
			log(QString("Enabling application proxy on host \"%1\" and port %2.").arg(m_settings->value("Proxy/hostName").toString()).arg(m_settings->value("Proxy/port").toInt()));
		}
		else
		{ log(QString("Enabling system-wide proxy.")); }
	}

	m_progressdialog = nullptr;

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	log("Loading sources");
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
		log(QString("%n source(s) found: %1").arg(srsc));
	}

	ui->actionClosetab->setShortcut(QKeySequence::Close);
	QShortcut *actionCloseTabW = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_W), this);
	connect(actionCloseTabW, &QShortcut::activated, ui->actionClosetab, &QAction::trigger);

	QShortcut *actionFocusSearch = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
	connect(actionFocusSearch, &QShortcut::activated, this, &mainWindow::focusSearch);

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
		log("It seems that Imgbrd-Grabber hasn't shut down properly last time.");

		QString msg = tr("It seems that the application was not properly closed for its last use. Do you want to restore your last session?");
		QMessageBox dlg(QMessageBox::Question, "Grabber", msg, QMessageBox::Yes | QMessageBox::No);
		dlg.setWindowIcon(windowIcon());
		dlg.setDefaultButton(QMessageBox::Yes);

		int response = dlg.exec();
		m_restore = response == QMessageBox::Yes;
	}

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

	connect(m_profile, &Profile::favoritesChanged, this, &mainWindow::updateFavorites);
	connect(m_profile, &Profile::keptForLaterChanged, this, &mainWindow::updateKeepForLater);
	connect(m_profile, &Profile::sitesChanged, this, &mainWindow::loadSites);
	updateFavorites();
	updateKeepForLater();

	if (m_showLog)
		connect(&Logger::getInstance(), &Logger::newLog, this, &mainWindow::logShow);

	// Check for updates
	int cfuInterval = m_settings->value("check_for_updates", 24*60*60).toInt();
	QDateTime lastCfu = m_settings->value("last_check_for_updates", QDateTime()).toDateTime();
	if (cfuInterval > 0 && (!lastCfu.isValid() || lastCfu.addSecs(cfuInterval) <= QDateTime::currentDateTime()))
	{
		m_settings->setValue("last_check_for_updates", QDateTime::currentDateTime());

		m_updateDialog = new UpdateDialog(this);
		m_updateDialog->checkForUpdates();
	}

	m_currentTab = nullptr;
	log("End of initialization");
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
	{
		loadLinkList(m_profile->getPath() + "/restore.igl");
		loadTabs(m_profile->getPath() + "/tabs.txt");
	}
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

	// Detect danbooru downloader
	QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "Mozilla", "Firefox");
	QString path = QFileInfo(cfg.fileName()).absolutePath()+"/Firefox";
	QSettings profiles(path+"/profiles.ini", QSettings::IniFormat);

	if (QFile::exists(path+"/"+profiles.value("Profile0/Path").toString()+"/extensions/danbooru_downloader@cuberocks.net.xpi"))
	{
		int reponse = QMessageBox::question(this, "", tr("The Mozilla Firefox addon \"Danbooru Downloader\" has been detected on your system. Do you want to load its preferences?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			QFile prefs(path+"/"+profiles.value("Profile0/Path").toString()+"/prefs.js");
			if (prefs.exists() && prefs.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				QString source = prefs.readAll();
				QRegExp rx("user_pref\\(\"danbooru.downloader.([^\"]+)\", ([^\\)]+)\\);");
				QMap<QString,QString> firefox, assoc;
				assoc["blacklist"] = "blacklistedtags";
				assoc["generalTagsSeparator"] = "separator";
				assoc["multipleArtistsAll"] = "artist_useall";
				assoc["multipleArtistsDefault"] = "artist_value";
				assoc["multipleArtistsSeparator"] = "artist_sep";
				assoc["multipleCharactersAll"] = "character_useall";
				assoc["multipleCharactersDefault"] = "character_value";
				assoc["multipleCharactersSeparator"] = "character_sep";
				assoc["multipleCopyrightsAll"] = "copyright_useall";
				assoc["multipleCopyrightsDefault"] = "copyright_value";
				assoc["multipleCopyrightsSeparator"] = "copyright_sep";
				assoc["noArtist"] = "artist_empty";
				assoc["noCharacter"] = "character_empty";
				assoc["noCopyright"] = "copyright_empty";
				assoc["targetFolder"] = "path";
				assoc["targetName"] = "filename";

				int pos = 0;
				while ((pos = rx.indexIn(source, pos)) != -1)
				{
					pos += rx.matchedLength();
					QString value = rx.cap(2);
					if (value.startsWith('"'))	{ value = value.right(value.length() - 1);	}
					if (value.endsWith('"'))	{ value = value.left(value.length() - 1);	}
					firefox[rx.cap(1)] = value;
				}

				m_settings->beginGroup("Save");
				if (firefox.keys().contains("useBlacklist"))
				{
					if (firefox["useBlacklist"] == "true")
					{ m_settings->setValue("downloadblacklist", false); }
					else
					{ m_settings->setValue("downloadblacklist", true); }
				}
				for (int i = 0; i < firefox.size(); i++)
				{
					if (assoc.keys().contains(firefox.keys().at(i)))
					{
						QString v =  firefox.values().at(i);
						v.replace("\\\\", "\\");
						m_settings->setValue(assoc[firefox.keys().at(i)], v);
					}
				}
				m_settings->endGroup();
				prefs.close();
			}
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

int mainWindow::addTab(QString tag, bool background, bool save)
{
	tagTab *w = new tagTab(&m_sites, m_profile, this);
	this->addSearchTab(w, background, save);

	if (!tag.isEmpty())
	{ w->setTags(tag); }

	m_tagTabs.append(w);
	return m_tabs.size() - 1;
}
int mainWindow::addPoolTab(int pool, QString site, bool background, bool save)
{
	poolTab *w = new poolTab(&m_sites, m_profile, this);
	this->addSearchTab(w, background, save);

	if (!site.isEmpty())
	{ w->setSite(site); }
	if (pool != 0)
	{ w->setPool(pool, site); }

	m_poolTabs.append(w);
	return m_tabs.size() - 1;
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
	QList<searchTab*> allTabs;
	QList<tagTab*> tagTabs;
	QList<poolTab*> poolTabs;
	int currentTab;

	if (!TabsLoader::load(filename, tagTabs, poolTabs, allTabs, currentTab, m_profile, m_sites, this))
		return false;

	for (auto tab : allTabs)
		addSearchTab(tab, true, false);

	m_tagTabs.append(tagTabs);
	m_poolTabs.append(poolTabs);

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
	m_tabs.removeAll(tab);
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
			m_currentTab = ui->tabWidget->currentWidget();

			ui->labelWiki->setText("<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+tb->wiki());
		}
	}
}

void mainWindow::setTags(QList<Tag> tags, searchTab *from)
{
	if (from != nullptr && m_tabs.indexOf(from) != ui->tabWidget->currentIndex())
		return;

	clearLayout(ui->dockInternetScrollLayout);

	QString text = "";
	for (Tag tag : tags)
	{
		if (!text.isEmpty())
			text += "<br/>";
		text += tag.stylished(m_profile, QStringList(), QStringList(), true, true);
	}

	QAffiche *taglabel = new QAffiche(QVariant(), 0, QColor(), this);
	taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	connect(taglabel, static_cast<void (QAffiche::*)(QString)>(&QAffiche::middleClicked), this, &mainWindow::loadTagTab);
	connect(taglabel, &QAffiche::linkHovered, this, &mainWindow::linkHovered);
	connect(taglabel, &QAffiche::linkActivated, this, &mainWindow::loadTagNoTab);
	taglabel->setText(text);

	// Context menu
	taglabel->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(taglabel, &QWidget::customContextMenuRequested, this, &mainWindow::contextMenu);

	ui->dockInternetScrollLayout->addWidget(taglabel);
}

void mainWindow::closeCurrentTab()
{
	// Unclosable tabs have a maximum width of 16777214 (default: 16777215)
	if (ui->tabWidget->widget(ui->tabWidget->currentIndex())->maximumWidth() != 16777214)
	{ ui->tabWidget->widget(ui->tabWidget->currentIndex())->deleteLater(); }
}

void mainWindow::addTableItem(QTableWidget *table, int row, int col, QString text)
{
	QTableWidgetItem *item = new QTableWidgetItem(text);
	item->setToolTip(text);

	table->setItem(row, col, item);

}

void mainWindow::batchAddGroup(const DownloadQueryGroup &values)
{
	m_groupBatchs.append(values);
	int pos = m_groupBatchs.count();

	ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount() + 1);
	int row = ui->tableBatchGroups->rowCount() - 1;
	m_allow = false;

	QTableWidgetItem *item = new QTableWidgetItem(getIcon(":/images/colors/black.png"), QString::number(pos));
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
	log(QString("Adding single image: %1").arg(query.values["file_url"]));

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

	// Delete single image downloads
	selected = ui->tableBatchUniques->selectedItems();
	count = selected.size();
	todelete.clear();
	for (int i = 0; i < count; i++)
	{ todelete.append(selected.at(i)->row()); }
	qSort(todelete);
	rem = 0;
	for (int i : todelete)
	{
		int pos = i - rem;
		ui->tableBatchUniques->removeRow(pos);
		m_batchs.removeAt(pos);
		rem++;
	}
	updateGroupCount();
}

QList<int> mainWindow::getSelectedRows(QList<QTableWidgetItem*> selected)
{
	QList<int> rows;
	for (QTableWidgetItem *item : selected)
	{
		int row = item->row();
		if (rows.contains(row))
			continue;
		else
			rows.append(row);
	}
	return rows;
}

void mainWindow::batchMove(int diff)
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	if (selected.isEmpty())
		return;

	QList<int> rows = getSelectedRows(selected);
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

void mainWindow::batchChange(int)
{
	/*if (!m_tabs[0]->ui->checkMergeResults->isChecked())
	{
		int n = 0;
		for (int i = 0; i < m_webPics.count(); i++)
		{
			if (m_webPics.at(i)->id() == id)
			{ n = i; break; }
		}
		batchAddUnique(m_details.at(n));
	}*/
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
			case 8:	m_groupBatchs[y].getBlacklisted = (val != "false");	break;

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
				{ m_groupBatchs[y].page = toInt; }
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


void mainWindow::logShow(QDateTime date, QString msg)
{
	if (!m_showLog)
		return;

	QString line = "[" + date.toString("hh:mm:ss.zzz") + "] " + msg;

	ui->labelLog->appendHtml(line);
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
			log(QString("Translating texts in %1...").arg(m_currLang));
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

	log("Saving...");
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
	log("Opening options window...");

	optionsWindow *options = new optionsWindow(m_profile, this);
	connect(options, SIGNAL(languageChanged(QString)), this, SLOT(loadLanguage(QString)));
	connect(options, &optionsWindow::settingsChanged, this, &mainWindow::on_buttonInitSettings_clicked);
	connect(options, &QDialog::accepted, this, &mainWindow::optionsClosed);
	options->show();

	DONE();
}

void mainWindow::optionsClosed()
{
	m_tabs[0]->optionsChanged();
	m_tabs[0]->updateCheckboxes();
}

void mainWindow::setSource(QString source)
{
	if (m_tabs.size() < 1)
		return;

	QList<bool> sel;
	QStringList keys = m_sites.keys();
	for (QString key : keys)
	{ sel.append(key == source); }

	m_tabs[0]->saveSources(sel);
}

void mainWindow::aboutWebsite()
{
	QDesktopServices::openUrl(QUrl("https://github.com/Bionus/imgbrd-grabber"));
}
void mainWindow::aboutReportBug()
{
	QDesktopServices::openUrl(QUrl("https://github.com/Bionus/imgbrd-grabber/issues/new"));
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
		log("Batch download start cancelled because another one is already running.");
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
	log("Batch download started.");

	if (m_progressdialog == nullptr)
	{
		m_progressdialog = new batchWindow(this);
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

	QList<QTableWidgetItem *> selected = ui->tableBatchUniques->selectedItems();
	int count = selected.size();
	if (!all)
	{
		QList<int> tdl;
		for (int r = 0; r < count; r++)
		{
			int row = selected.at(r)->row();

			if (tdl.contains(row))
				continue;
			else
			{
				tdl.append(row);
				int i = row;
				Site *site = m_batchs[i].site;
				m_getAllRemaining.append(QSharedPointer<Image>(new Image(site, m_batchs[i].values, m_profile, new Page(m_profile, site, m_sites.values(), m_batchs[i].values.value("tags").split(" "), 1, 1, QStringList(), false, this))));
			}
		}
	}
	else
	{
		for (int i = 0; i < m_batchs.size(); i++)
		{
			if (m_batchs[i].values.value("file_url").isEmpty())
			{
				// If we cannot get the image's url, we try looking for it
				/*Page *page = new Page(m_sites[site], &m_sites, m_groupBatchs.at(i).at(0).split(' '), m_groupBatchs.at(i).at(1).toInt()+r, pp, QStringList(), false, this);
				connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
				page->load();
				log(QString("Loading page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
				m_groupBatchs[i][8] += (m_groupBatchs[i][8] == "" ? "" : "¤") + QString::number((int)page);
				m_progressdialog->setImagesCount(m_progressdialog->count() + 1);*/
			}
			else
			{
				Site *site = m_batchs[i].site;
				m_getAllRemaining.append(QSharedPointer<Image>(new Image(site, m_batchs[i].values, m_profile, new Page(m_profile, site, m_sites.values(), m_batchs[i].values["tags"].split(" "), 1, 1, QStringList(), false, this))));
			}
		}
	}
	m_getAllLimit = m_batchs.size();

	m_allow = false;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
	{ ui->tableBatchGroups->item(i, 0)->setIcon(getIcon(":/images/colors/black.png")); }
	m_allow = true;
	m_profile->getCommands().before();
	selected = ui->tableBatchGroups->selectedItems();
	count = selected.size();
	m_batchDownloading.clear();

	QSet<int> todownload = QSet<int>();
	for (int i = 0; i < count; i++)
		if (!todownload.contains(selected.at(i)->row()))
			todownload.insert(selected.at(i)->row());

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

	for (int j : m_batchPending.keys())
	{
		DownloadQueryGroup b = m_batchPending[j];

		int constImagesPerPack = usePacking ? realConstImagesPerPack : b.total;
		int pagesPerPack = qCeil((float)constImagesPerPack / b.perpage);
		int imagesPerPack = pagesPerPack * b.perpage;
		int packs = qCeil((float)b.total / imagesPerPack);

		int lastPageImages = b.total % imagesPerPack;
		if (lastPageImages == 0)
			lastPageImages = imagesPerPack;

		for (int i = 0; i < packs; ++i)
		{
			Downloader *downloader = new Downloader(m_profile,
													b.tags.split(' '),
													QStringList(),
													QList<Site*>() << b.site,
													b.page + i * pagesPerPack,
													(i == packs - 1 ? lastPageImages : imagesPerPack),
													b.perpage,
													b.path,
													b.filename,
													nullptr,
													nullptr,
													b.getBlacklisted,
													m_settings->value("blacklistedtags").toString().split(' '),
													false,
													0,
													"");
			downloader->setData(j);
			downloader->setQuit(false);

			connect(downloader, &Downloader::finishedImages, this, &mainWindow::getAllFinishedImages);
			connect(downloader, &Downloader::finishedImagesPage, this, &mainWindow::getAllFinishedPage);

			m_waitingDownloaders.enqueue(downloader);
		}
	}

	getNextPack();
}

void mainWindow::getNextPack()
{
	m_downloaders.clear();
	m_downloaders.append(m_waitingDownloaders.dequeue());

	getAllGetPages();
}

void mainWindow::getAllGetPages()
{
	m_progressdialog->clear();
	m_progressdialog->setText(tr("Downloading pages, please wait..."));

	// Only images to download
	if (m_downloaders.isEmpty())
	{
		m_batchAutomaticRetries = m_settings->value("Save/automaticretries", 0).toInt();
		getAllImages();
	}
	else
	{
		for (Downloader *downloader : m_downloaders)
		{
			m_progressdialog->setImagesCount(m_progressdialog->count() + downloader->pagesCount());
			downloader->getImages();
		}
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

	log(tr("All images' urls have been received (%n image(s)).", "", m_getAllRemaining.count()));

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
	m_progressdialog->setImagesCount(m_getAllLimit);
	//m_progressdialog->setMaximum(count);
	m_progressdialog->setText(tr("Downloading images..."));
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
		log("Downloading images details.");
	else
		log("Downloading images directly.");

	// We start the simultaneous downloads
	for (int i = 0; i < qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10)); i++)
		_getAll();
}

bool mainWindow::needExactTags(QSettings *settings)
{
	if (settings->value("Textfile/activate", false).toBool())
	{
		Filename fn(settings->value("Textfile/content", "").toString());
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
		if (m_mustGetTags)
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
				ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/colors/blue.png"));
				path = m_groupBatchs[site_id - 1].filename;
				pth = m_groupBatchs[site_id - 1].page;
			}

			QString p = img->folder().isEmpty() ? pth : img->folder();
			QStringList paths = img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1, true, false, true, true, true);

			bool notexists = true;
			for (QString path : paths)
			{
				QFile f(path);
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
				log(QString("File already exists: <a href=\"file:///%1\">%1</a>").arg(paths.at(0)));
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
	bool detected = false;
	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
		detected = !img->blacklisted(blacklistedtags).isEmpty();
	}

	if (detected && site_id >= 0 && !m_groupBatchs[site_id - 1].getBlacklisted)
	{
		m_getAllIgnored++;
		log("Image ignored.");

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
		{ ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/colors/green.png")); }
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

	log("Tags received");

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
		log(QString("File already exists: <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
		m_progressdialog->loadedImage(img->url());
		if (site_id >= 0)
		{
			m_progressBars[site_id - 1]->setValue(m_progressBars[site_id - 1]->value()+1);
			if (m_progressBars[site_id - 1]->value() >= m_progressBars[site_id - 1]->maximum())
			{ ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/colors/green.png")); }
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
	// Row
	int site_id = m_progressdialog->batch(img->url());
	int row = getRowForSite(site_id);

	// Path
	QString path = m_settings->value("Save/filename").toString();
	QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/colors/blue.png"));
		path = m_groupBatchs[site_id - 1].filename;
		p = m_groupBatchs[site_id - 1].path;
	}
	QStringList paths = img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1, true, false, true, true, true);

	// Action
	QString whatToDo = m_settings->value("Save/md5Duplicates", "save").toString();
	QString md5Duplicate = m_profile->md5Exists(img->md5());
	bool next = true;
	if (md5Duplicate.isEmpty() || whatToDo == "save")
	{
		log(QString("Loading image from <a href=\"%1\">%1</a> %2").arg(img->fileUrl().toString()).arg(m_getAllDownloading.size()));
		m_progressdialog->loadingImage(img->url());
		m_downloadTime.insert(img->url(), new QTime);
		m_downloadTime[img->url()]->start();
		m_downloadTimeLast.insert(img->url(), new QTime);
		m_downloadTimeLast[img->url()]->start();
		connect(img.data(), &Image::finishedImage, this, &mainWindow::getAllPerformImage, Qt::UniqueConnection);
		connect(img.data(), &Image::downloadProgressImage, this, &mainWindow::getAllProgress, Qt::UniqueConnection);
		img->loadImage();
		next = false;
	}
	else
	{
		for (QString fp : paths)
		{
			if (whatToDo == "copy")
			{
				m_getAllDownloaded++;
				log(QString("Copy from <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(fp));
				QFile::copy(md5Duplicate, fp);

				if (m_settings->value("Save/keepDate", true).toBool())
					setFileCreationDate(fp, img->createdAt());
			}
			else if (whatToDo == "move")
			{
				m_getAllDownloaded++;
				log(QString("Move from <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(fp));
				QFile::rename(md5Duplicate, fp);
				m_profile->setMd5(img->md5(), fp);

				if (m_settings->value("Save/keepDate", true).toBool())
					setFileCreationDate(fp, img->createdAt());
			}
			else
			{
				m_getAllExists++;
				log(QString("MD5 \"%1\" of the image <a href=\"%2\">%2</a> already found in file <a href=\"file:///%3\">%3</a>").arg(img->md5(), img->url(), md5Duplicate));
			}
		}
	}

	// Continue to next image
	if (next)
	{
		//m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_progressdialog->images()+1);
		m_downloadTimeLast.remove(img->url());
		m_getAllDownloading.removeAll(img);
		_getAll();
	}
}
void mainWindow::getAllPerformImage(QNetworkReply::NetworkError error, QString errorString)
{
	if (m_progressdialog->cancelled())
		return;

	QSharedPointer<Image> img;
	for (QSharedPointer<Image> i : m_getAllDownloading)
		if (i.data() == sender())
			img = i;
	if (img.isNull())
		return;

	bool del = true;

	log(QString("Image received from <a href=\"%1\">%1</a> %2").arg(img->url()).arg(m_getAllDownloading.size()));

	// Row
	int site_id = m_progressdialog->batch(img->url());
	int row = getRowForSite(site_id);

	int errors = m_getAllErrors, e404s = m_getAll404s;
	if (error == QNetworkReply::NoError)
	{
		if (site_id >= 0)
		{
			ui->tableBatchGroups->item(row, 0)->setIcon(getIcon(":/images/colors/blue.png"));
			saveImage(img, m_groupBatchs[site_id - 1].filename, m_groupBatchs[site_id - 1].path);
		}
		else
		{ saveImage(img); }
	}
	else if (error == QNetworkReply::ContentNotFoundError)
	{ m_getAll404s++; }
	else
	{
		log(QString("Unknown error for the image: <a href=\"%1\">%1</a>. \"%2\"").arg(img->url().toHtmlEscaped(), errorString), Logger::Error);
		m_getAllErrors++;
	}

	if (m_getAllErrors == errors && m_getAll404s == e404s)
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

	getAllImageOk(img, site_id, del);
}
void mainWindow::saveImage(QSharedPointer<Image> img, QString path, QString p, bool getAll)
{
	// Get image's content
	if (!img->data().isEmpty())
	{
		// Path
		if (path == "")
		{ path = m_settings->value("Save/filename").toString(); }
		if (p == "")
		{ p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder(); }
		int count = m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1;
		QStringList paths = img->path(path, p, count, true, false, true, true, true);

		for (QString path : paths)
		{
			if (getAll)
			{ path.replace("%n%", QString::number(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors)); }
			QString fp = QDir::toNativeSeparators(path);

			QString whatToDo = m_settings->value("Save/md5Duplicates", "save").toString();
			QString md5Duplicate = m_profile->md5Exists(img->md5());
			if (md5Duplicate.isEmpty() || whatToDo == "save")
			{
				// Create the reception's directory
				QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2)), dir(p);
				if (!path_to_file.exists() && !dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
				{
					log(QString("Impossible to create the destination folder: %1.").arg(p+"/"+path.section('/', 0, -2)), Logger::Error);
					if (getAll)
					{ m_getAllErrors++; }
				}

				// Save the file
				else
				{
					QFile f(fp);
					f.open(QIODevice::WriteOnly);
					if (f.write(img->data()) < 0)
					{
						f.close();
						f.remove();
						m_getAllErrors++;
						m_progressdialog->pause();
						QString err = tr("An error occured saving the image.\n%1\n%2\nPlease solve the issue before resuming the download.").arg(fp, f.errorString());
						log(err);
						QMessageBox::critical(m_progressdialog, tr("Error"), err);
						return;
					}
					f.close();

					m_profile->addMd5(img->md5(), fp);

					// Save info to a text file
					if (m_settings->value("Textfile/activate", false).toBool())
					{
						QStringList cont = img->path(m_settings->value("Textfile/content", "%all%").toString(), "", count, true, true, false, false);
						if (!cont.isEmpty())
						{
							QString contents = cont.at(0);
							QFile file_tags(fp + ".txt");
							if (file_tags.open(QFile::WriteOnly | QFile::Text))
							{
								file_tags.write(contents.toLatin1());
								file_tags.close();
							}
						}
					}

					// Log info to a text file
					if (m_settings->value("SaveLog/activate", false).toBool() && !m_settings->value("SaveLog/file", "").toString().isEmpty())
					{
						QStringList cont = img->path(m_settings->value("SaveLog/format", "%website% - %md5% - %all%").toString(), "", count, true, true, false, false);
						if (!cont.isEmpty())
						{
							QString contents = cont.at(0);
							QFile file_tags(m_settings->value("SaveLog/file", "").toString());
							if (file_tags.open(QFile::WriteOnly | QFile::Append | QFile::Text))
							{
								file_tags.write(contents.toUtf8() + "\n");
								file_tags.close();
							}
						}
					}
				}

				// Execute commands
				Commands &commands = m_profile->getCommands();
				for (Tag tag : img->tags())
				{ commands.tag(*img, tag, false); }
				commands.image(*img, fp);
				for (Tag tag : img->tags())
				{ commands.tag(*img, tag, true); }

				if (m_settings->value("Save/keepDate", true).toBool())
					setFileCreationDate(fp, img->createdAt());
			}
		}
	}
	else
	{
		log(QString("Nothing has been received for the image: <a href=\"%1\">%1</a>.").arg(img->url().toHtmlEscaped()), Logger::Error);
		if (getAll)
		{ m_getAllErrors++; }
	}
}

void mainWindow::getAllCancel()
{
	log("Cancelling downloads...");
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
	log("Skipping downloads...");

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

	log("Images download finished.");
	m_progressdialog->setValue(m_progressdialog->maximum());

	// Delete objects
	for (Downloader *d : m_downloadersDone)
	{
		d->clear();
	}
	qDeleteAll(m_downloadersDone);
	m_downloadersDone.clear();

	// Final action
	switch (m_progressdialog->endAction())
	{
		case 1:	m_progressdialog->close();				break;
		case 2:	openTray();								break;
		case 3:	saveFolder();							break;
		case 4:	QSound::play(":/sounds/finished.wav");	break;
		case 5: shutDown();								break;
	}
	if (m_progressdialog->endRemove())
	{
		int rem = 0;
		for (int i : m_batchDownloading)
		{
			int pos = i - rem;
			m_progressBars[pos]->deleteLater();
			m_progressBars.removeAt(pos);
			ui->tableBatchGroups->removeRow(pos);
			rem++;
		}
	}
	activateWindow();
	m_getAll = false;

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

	// End of batch download
	m_profile->getCommands().after();
	ui->widgetDownloadButtons->setEnabled(true);
	log("Batch download finished");
}

void mainWindow::getAllPause()
{
	if (m_progressdialog->isPaused())
	{
		log("Pausing downloads...");
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			m_getAllDownloading[i]->abortTags();
			m_getAllDownloading[i]->abortImage();
		}
		m_getAll = false;
	}
	else
	{
		log("Recovery of downloads...");
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			if (m_getAllDownloading[i]->tagsReply() != nullptr)
			{ m_getAllDownloading[i]->loadDetails(); }
			if (m_getAllDownloading[i]->imageReply() != nullptr)
			{ m_getAllDownloading[i]->loadImage(); }
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

	log(tr("Loading %n download(s)", "", newBatchs.count() + newGroupBatchs.count()));

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
		addTableItem(ui->tableBatchGroups, row, 8, queryGroup.getBlacklisted ? "true" : "false");

		queryGroup.unk = "true";
		m_groupBatchs.append(queryGroup);
		QTableWidgetItem *it = new QTableWidgetItem(getIcon(":/images/colors/"+QString(val == max ? "green" : (val > 0 ? "blue" : "black"))+".png"), "");
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
	ui->labelWiki->setText(wiki);
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
	fn.isValid(&message);
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

void mainWindow::increaseDownloads()
{
	m_downloads++;
	updateDownloads();
}
void mainWindow::decreaseDownloads()
{
	m_downloads--;
	updateDownloads();
}
void mainWindow::updateDownloads()
{
	if (m_downloads == 0)
		setWindowTitle("");
	else
		setWindowTitle(tr("%n download(s) in progress", "", m_downloads));
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
	if (tag.startsWith("http://"))
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
	QMenu *menu = new QMenu(this);
	if (!this->m_link.isEmpty())
	{
		bool favorited = false;
		for (Favorite fav : m_favorites)
			if (fav.getName() == m_link)
				favorited = true;
		if (favorited)
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Remove from favorites"), this, SLOT(unfavorite())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Add to favorites"), this, SLOT(favorite())); }

		QStringList &vil = m_profile->getKeptForLater();
		if (vil.contains(m_link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Don't keep for later"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Keep for later"), this, SLOT(viewitlater())); }

		menu->addSeparator();
		menu->addAction(QIcon(":/images/icons/tab-plus.png"), tr("Open in a new tab"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Open in a new window"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}

void mainWindow::openInNewTab()
{
	addTab(m_link);
}
void mainWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(m_link));
}

void mainWindow::favorite()
{
	m_profile->addFavorite(m_link);
}
void mainWindow::unfavorite()
{
	m_profile->removeFavorite(m_link);
}

void mainWindow::viewitlater()
{
	m_profile->addKeptForLater(m_link);
}
void mainWindow::unviewitlater()
{
	m_profile->removeKeptForLater(m_link);
}


void mainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData* mimeData = event->mimeData();
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
			}
		}
	}
}

void mainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			loadMd5(urlList.at(i).toLocalFile(), true, false);
		}
	}
}
