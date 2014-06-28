#include <QtXml>
#include <QtScript>
#include <QMessageBox>
#include <QFileDialog>
#include <QSound>
#include <QtSql/QSqlDatabase>
#if defined(Q_OS_WIN)
	#include "windows.h"
	#include <float.h>
#endif
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_tagtab.h"
#include "ui_pooltab.h"
#include "optionswindow.h"
#include "startwindow.h"
#include "favoritewindow.h"
#include "addgroupwindow.h"
#include "adduniquewindow.h"
#include "zoomwindow.h"
#include "batchwindow.h"
#include "aboutwindow.h"
#include "blacklistfix.h"
#include "emptydirsfix.h"
#include "md5fix.h"
#include "functions.h"
#include "json.h"
#include "commands.h"

#define DONE()			logUpdate(QObject::tr(" Fait"))
#define DIR_SEPARATOR	QDir::toNativeSeparators("/")

extern QMap<QDateTime,QString> _log;
extern QMap<QString,QString> _md5;

/*
  deviantart.org
  g.e-hentai.org
  minitokyo.net
  pixiv.net
*/



mainWindow::mainWindow(QString program, QStringList tags, QMap<QString,QString> params) : ui(new Ui::mainWindow), m_currentFav(-1), m_downloads(0), m_loaded(false), m_getAll(false), m_program(program), m_tags(tags), m_params(params)
{ }
void mainWindow::init()
{
	m_settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	bool crashed = m_settings->value("crashed", false).toBool();

	m_settings->setValue("crashed", true);
	m_settings->sync();

	loadLanguage(m_settings->value("language", "English").toString(), true);
	ui->setupUi(this);
	log(tr("Nouvelle session démarée."));
	log(tr("Version du logiciel : %1.").arg(VERSION));
	log(tr("Chemin : %1").arg(qApp->applicationDirPath()));
	log(tr("Chargement des préférences depuis <a href=\"file:///%1\">%1</a>").arg(savePath("settings.ini")));

	loadMd5s();

	tabifyDockWidget(ui->dock_internet, ui->dock_wiki);
	tabifyDockWidget(ui->dock_wiki, ui->dock_kfl);
	tabifyDockWidget(ui->dock_kfl, ui->dock_favorites);
	ui->dock_internet->raise();

	ui->menuView->addAction(ui->dock_internet->toggleViewAction());
	ui->menuView->addAction(ui->dock_wiki->toggleViewAction());
	ui->menuView->addAction(ui->dock_kfl->toggleViewAction());
	ui->menuView->addAction(ui->dock_favorites->toggleViewAction());
	ui->menuView->addAction(ui->dockOptions->toggleViewAction());

	m_favorites = loadFavorites();

	if (m_settings->value("Proxy/use", false).toBool())
	{
		QNetworkProxy::ProxyType type = m_settings->value("Proxy/type", "http").toString() == "http" ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy;
		QNetworkProxy proxy(type, m_settings->value("Proxy/hostName").toString(), m_settings->value("Proxy/port").toInt());
		QNetworkProxy::setApplicationProxy(proxy);
		log(tr("Activation du proxy général sur l'hôte \"%1\" et le port %2.").arg(m_settings->value("Proxy/hostName").toString()).arg(m_settings->value("Proxy/port").toInt()));
	}

	m_serverDate = QDateTime::currentDateTime().toUTC().addSecs(-60*60*4);
	m_timezonedecay = QDateTime::currentDateTime().time().hour()-m_serverDate.time().hour();
	m_gotMd5 = QStringList();
	m_mergeButtons = QList<QBouton*>();
	m_progressBars = QList<QProgressBar*>();

	m_progressdialog = new batchWindow(this);
	connect(m_progressdialog, SIGNAL(paused()), this, SLOT(getAllPause()));

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	loadSites();

	if (m_sites.size() == 0)
	{
		QMessageBox::critical(this, tr("Aucune source trouvée"), tr("Aucune source n'a été trouvée. Auriez-vous un problème de configuration ? Essayez de réinstaller."));
		qApp->quit();
		this->deleteLater();
		return;
	}

	ui->actionClosetab->setShortcut(QKeySequence::Close);
	ui->actionAddtab->setShortcut(QKeySequence::AddTab);
	ui->actionQuit->setShortcut(QKeySequence::Quit);
	ui->actionFolder->setShortcut(QKeySequence::Open);

	loadLanguage(m_settings->value("language", "English").toString());

	connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	if (m_settings->value("firstload", true).toBool())
	{
		QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "Mozilla", "Firefox");
		QString path = QFileInfo(cfg.fileName()).absolutePath()+"/Firefox";
		QSettings profiles(path+"/profiles.ini", QSettings::IniFormat);
		if (QFile::exists(path+"/"+profiles.value("Profile0/Path").toString()+"/extensions/danbooru_downloader@cuberocks.net.xpi"))
		{
			int reponse = QMessageBox::question(this, "", tr("L'extension pour Mozilla Firefox \"Danbooru Downloader\" a été détéctée sur votre système. Souhaitez-vous en importer les préférences ?"), QMessageBox::Yes | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{
				QFile prefs(path+"/"+profiles.value("Profile0/Path").toString()+"/prefs.js");
				if (prefs.exists())
				{
					if (prefs.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						QString source;
						while (!prefs.atEnd())
						{ source += QString(prefs.readLine()); }
						QRegExp rx("user_pref\\(\"danbooru.downloader.([^\"]+)\", ([^\\)]+)\\);");
						int pos = 0;
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
						while ((pos = rx.indexIn(source, pos)) != -1)
						{
							pos += rx.matchedLength();
							QString value = rx.cap(2);
							if (value.left(1) == "\"")	{ value = value.right(value.length()-1);	}
							if (value.right(1) == "\"")	{ value = value.left(value.length()-1);		}
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
				}
			}
		}
		else
		{
			startWindow *swin = new startWindow(this);
			swin->show();
		}
		m_settings->setValue("firstload", false);
	}

	// Loading last window state, size and position from the settings file
	restoreGeometry(m_settings->value("geometry").toByteArray());
	restoreState(m_settings->value("state").toByteArray());

	// Selected ones
	QString sav = m_settings->value("sites", "1").toString();
	for (int i = 0; i < m_sites.count(); i++)
	{
		if (sav.count() <= i)
		{ m_selectedSources.append(false); }
		else
		{ m_selectedSources.append(sav.at(i) == '1' ? true : false); }
	}

	QPushButton *add = new QPushButton(QIcon(":/images/add.png"), "", this);
		add->setFlat(true);
		add->resize(QSize(12,12));
		connect(add, SIGNAL(clicked()), this, SLOT(addTab()));
		ui->tabWidget->setCornerWidget(add);

	bool restore = m_settings->value("start", "none").toString() == "restore";
	if (crashed && !restore)
	{
		log(tr("Il semblerait que Imgbrd-Grabber n'ait pas été fermé correctement la dernière fois."));
		int reponse = QMessageBox::question(this, "", tr("Il semblerait que l'application n'ait pas été arrêtée correctement lors de sa dernière utilisation. Voulez-vous restaurer votre dernière seesion ?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{ restore = true; }
	}
	ui->tabWidget->setCurrentIndex(0);
	if (restore)
	{
		loadLinkList(savePath("restore.igl"));
		loadTabs(savePath("tabs.txt"));
	}
	if (m_tabs.isEmpty())
	{ addTab(); }

	// Favorites tab
	m_favoritesTab = new favoritesTab(m_tabs.size(), &m_sites, &m_favorites, &m_serverDate, this);
	connect(m_favoritesTab, SIGNAL(batchAddGroup(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	connect(m_favoritesTab, SIGNAL(batchAddUnique(QMap<QString,QString>)), this, SLOT(batchAddUnique(QMap<QString,QString>)));
	connect(m_favoritesTab, SIGNAL(changed(searchTab*)), this, SLOT(updateTabs()));
	ui->tabWidget->insertTab(m_tabs.size(), m_favoritesTab, tr("Favoris"));

	ui->tableBatchGroups->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	ui->tableBatchUniques->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	on_buttonInitSettings_clicked();

	// Console usage
	if (this->m_params.keys().contains("batch"))
	{
		batchAddGroup(QStringList() << m_tags.join(" ") << m_params.value("page", "1") << m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("ignore", m_settings->value("downloadblacklist", "false").toString()) << this->m_params.value("booru", m_sites.keys().at(0)) << this->m_params.value("filename", m_settings->value("Save/filename").toString()) << this->m_params.value("path", m_settings->value("Save/path").toString()) << "");
		ui->tabWidget->setCurrentIndex(2);
		if (!m_params.keys().contains("dontstart"))
		{ getAll(); }
	}
	else if (!m_tags.isEmpty() || m_settings->value("start", "none").toString() == "firstpage")
	{
		if (!m_tags.isEmpty() && m_tags.first().endsWith(".igl"))
		{
			loadLinkList(m_tags.first());
			ui->tabWidget->setCurrentIndex(m_tagTabs.size()+1);
			m_tags.clear();
		}
		else
		{ m_tagTabs[0]->setTags(this->m_tags.join(" ")); }
	}

	QStringList sizes = m_settings->value("batch", "100,100,100,100,100,100,100,100,100").toString().split(',');
	int m = sizes.size() > ui->tableBatchGroups->columnCount() ? ui->tableBatchGroups->columnCount() : sizes.size();
	for (int i = 0; i < m; i++)
	{ ui->tableBatchGroups->horizontalHeader()->resizeSection(i, sizes.at(i).toInt()); }

	Commands::get()->init(m_settings);

	updateFavorites(true);
	updateKeepForLater();

	m_lineFilename_completer = QStringList(m_settings->value("Save/filename").toString());
	m_lineFolder_completer = QStringList(m_settings->value("Save/path").toString());
	ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer));
	ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));

	m_loaded = true;
	logShow();
}
void mainWindow::loadSites()
{
	// TODO: fix crash on qDeleteAll here
	// qDeleteAll(m_sites);

	m_sites.clear();
	m_sites = *Site::getAllSites();
}

mainWindow::~mainWindow()
{
	delete ui;
}

int mainWindow::addTab(QString tag)
{
	tagTab *w = new tagTab(m_tabs.size(), &m_sites, &m_favorites, &m_serverDate, this);
	if (m_tabs.size() > ui->tabWidget->currentIndex())
	{
		w->setSources(m_tabs[ui->tabWidget->currentIndex()]->sources());
		w->setImagesPerPage(m_tabs[ui->tabWidget->currentIndex()]->imagesPerPage());
		w->setColumns(m_tabs[ui->tabWidget->currentIndex()]->columns());
		w->setPostFilter(m_tabs[ui->tabWidget->currentIndex()]->postFilter());
	}
	connect(w, SIGNAL(batchAddGroup(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	connect(w, SIGNAL(batchAddUnique(QMap<QString,QString>)), this, SLOT(batchAddUnique(QMap<QString,QString>)));
	connect(w, SIGNAL(titleChanged(searchTab*)), this, SLOT(updateTabTitle(searchTab*)));
	connect(w, SIGNAL(changed(searchTab*)), this, SLOT(updateTabs()));
	connect(w, SIGNAL(closed(tagTab*)), this, SLOT(tabClosed(tagTab*)));
	int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+(!m_tabs.isEmpty()), w, tr("Nouvel onglet"));
	m_tabs.append(w);
	m_tagTabs.append(w);
	ui->tabWidget->setCurrentIndex(index);
	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "", this);
		closeTab->setFlat(true);
		closeTab->resize(QSize(8,8));
		connect(closeTab, SIGNAL(clicked()), w, SLOT(deleteLater()));
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);
	if (!tag.isEmpty())
	{ w->setTags(tag); }
	saveTabs(savePath("tabs.txt"));
	return m_tabs.size() - 1;
}
int mainWindow::addPoolTab(int pool, QString site)
{
	poolTab *w = new poolTab(m_tabs.size(), &m_sites, &m_favorites, &m_serverDate, this);
	if (m_tabs.size() > ui->tabWidget->currentIndex())
	{ w->setSources(m_tabs[ui->tabWidget->currentIndex()]->sources()); }
	connect(w, SIGNAL(batchAddGroup(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	connect(w, SIGNAL(batchAddUnique(QMap<QString,QString>)), this, SLOT(batchAddUnique(QMap<QString,QString>)));
	connect(w, SIGNAL(titleChanged(searchTab*)), this, SLOT(updateTabTitle(searchTab*)));
	connect(w, SIGNAL(changed(searchTab*)), this, SLOT(updateTabs()));
	connect(w, SIGNAL(closed(poolTab*)), this, SLOT(tabClosed(poolTab*)));
	int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+(!m_tabs.isEmpty()), w, tr("Nouvel onglet"));
	m_tabs.append(w);
	m_poolTabs.append(w);
	ui->tabWidget->setCurrentIndex(index);
	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "", this);
		closeTab->setFlat(true);
		closeTab->resize(QSize(8,8));
		connect(closeTab, SIGNAL(clicked()), w, SLOT(deleteLater()));
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);
	if (!site.isEmpty())
	{ w->setSite(site); }
	if (pool != 0)
	{ w->setPool(pool, site); }
	saveTabs(savePath("tabs.txt"));
	return m_tabs.size() - 1;
}
bool mainWindow::saveTabs(QString filename)
{
	QStringList tabs = QStringList();
	foreach (tagTab *tab, m_tagTabs)
	{
		if (tab != NULL)
		{ tabs.append(tab->tags()+"¤"+QString::number(tab->ui->spinPage->value())+"¤"+QString::number(tab->ui->spinImagesPerPage->value())+"¤"+QString::number(tab->ui->spinColumns->value())); }
	}
	foreach (poolTab *tab, m_poolTabs)
	{
		if (tab != NULL)
		{ tabs.append(QString::number(tab->ui->spinPool->value())+"¤"+QString::number(tab->ui->comboSites->currentIndex())+"¤"+tab->tags()+"¤"+QString::number(tab->ui->spinPage->value())+"¤"+QString::number(tab->ui->spinImagesPerPage->value())+"¤"+QString::number(tab->ui->spinColumns->value())+"¤pool"); }
	}

	QFile f(filename);
	if (f.open(QFile::WriteOnly))
	{
		f.write(tabs.join("\r\n").toUtf8());
		f.close();
		return true;
	}
	return false;
}
bool mainWindow::loadTabs(QString filename)
{
	QFile f(filename);
	if (f.open(QFile::ReadOnly))
	{
		QString links = f.readAll().trimmed();
		f.close();

		QStringList tabs = links.split("\r\n");
		for (int j = 0; j < tabs.size(); j++)
		{
			QStringList infos = tabs[j].split("¤");
			if (infos.size() > 3)
			{
				if (infos[infos.size() - 1] == "pool")
				{
					addPoolTab();
					int i = m_poolTabs.size() - 1;
					m_poolTabs[i]->ui->spinPool->setValue(infos[0].toInt());
					m_poolTabs[i]->ui->comboSites->setCurrentIndex(infos[1].toInt());
					m_poolTabs[i]->ui->spinPage->setValue(infos[2].toInt());
					m_poolTabs[i]->ui->spinImagesPerPage->setValue(infos[4].toInt());
					m_poolTabs[i]->ui->spinColumns->setValue(infos[5].toInt());
					m_poolTabs[i]->setTags(infos[2]);
				}
				else
				{
					addTab();
					int i = m_tagTabs.size() - 1;
					m_tagTabs[i]->ui->spinPage->setValue(infos[1].toInt());
					m_tagTabs[i]->ui->spinImagesPerPage->setValue(infos[2].toInt());
					m_tagTabs[i]->ui->spinColumns->setValue(infos[3].toInt());
					m_tagTabs[i]->setTags(infos[0]);
				}
			}
		}
		return true;
	}
	return false;
}
void mainWindow::updateTabTitle(searchTab *tab)
{ ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), tab->windowTitle()); }
void mainWindow::updateTabs()
{ saveTabs(savePath("tabs.txt")); }
void mainWindow::tabClosed(tagTab *tab)
{
	m_tagTabs.removeAll(tab);
	m_tabs.removeAll((searchTab*)tab);
}
void mainWindow::tabClosed(poolTab *tab)
{
	m_poolTabs.removeAll(tab);
	m_tabs.removeAll((searchTab*)tab);
}
void mainWindow::tabClosed(searchTab *tab)
{ m_tabs.removeAll(tab); }
void mainWindow::currentTabChanged(int tab)
{
	if (m_loaded)
	{
		if (ui->tabWidget->widget(tab)->maximumWidth() != 16777214)
		{
			searchTab *tb = m_favoritesTab;
			if (tab < m_tabs.size())
			{ tb = m_tabs[tab]; }
			ui->labelTags->setText(tb->results());
			ui->labelWiki->setText("<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+tb->wiki());
		}
	}
}
void mainWindow::closeCurrentTab()
{
	// Unclosable tabs have a maximum width of 16777214 (default: 16777215)
	if (ui->tabWidget->widget(ui->tabWidget->currentIndex())->maximumWidth() != 16777214)
	{ ui->tabWidget->widget(ui->tabWidget->currentIndex())->deleteLater(); }
}

void mainWindow::batchAddGroup(const QStringList& values)
{
	m_groupBatchs.append(values);
	QTableWidgetItem *item;
	ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount()+1);
	m_allow = false;
	QTableWidgetItem *it = new QTableWidgetItem(QIcon(":/images/colors/black.png"), QString::number(ui->tableBatchGroups->rowCount()));
	ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, 0, it);
	for (int t = 0; t < values.count(); t++)
	{
		item = new QTableWidgetItem;
			item->setText(values.at(t));
			item->setToolTip(values.at(t));
		int r = t+1;
		if (r == 1) { r = 0; }
		else if (r == 6) { r = 1; }
		else if (r == 7) { r = 5; }
		else if (r == 8) { r = 6; }
		else if (r == 5) { r = 7; }
		ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, r+1, item);
	}
	QProgressBar *prog = new QProgressBar(this);
	prog->setTextVisible(false);
	m_progressBars.append(prog);
	ui->tableBatchGroups->setCellWidget(ui->tableBatchGroups->rowCount()-1, 9, prog);
	m_allow = true;
	saveLinkList(savePath("restore.igl"));
	updateGroupCount();
}
void mainWindow::updateGroupCount()
{
	int groups = 0;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
	{ groups += ui->tableBatchGroups->item(i, 5)->text().toInt(); }
	ui->labelGroups->setText(tr("Groupes (%1/%2)").arg(ui->tableBatchGroups->rowCount()).arg(groups));
}
void mainWindow::batchAddUnique(QMap<QString,QString> values)
{
	log(tr("Ajout d'une image en téléchargement unique : %1").arg(values.value("file_url")));
	m_batchs.append(values);
	QStringList types = QStringList() << "id" << "md5" <<  "rating" << "tags" << "file_url" << "site" << "filename" << "folder";
	QTableWidgetItem *item;
	ui->tableBatchUniques->setRowCount(ui->tableBatchUniques->rowCount()+1);
	for (int t = 0; t < types.count(); t++)
	{
		QString v = values.value(types.at(t));
		item = new QTableWidgetItem(v);
		ui->tableBatchUniques->setItem(ui->tableBatchUniques->rowCount()-1, t, item);
	}
	saveLinkList(savePath("restore.igl"));
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
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	QList<int> todelete = QList<int>();
	int count = selected.size();
	for (int i = 0; i < count; i++)
	{ todelete.append(selected.at(i)->row()); }
	qSort(todelete);
	int rem = 0;
	foreach (int i, todelete)
	{
		m_groupBatchs.removeAt(i - rem);
		m_progressBars.removeAt(i - rem);
		ui->tableBatchGroups->removeRow(i - rem);
		rem++;
	}

	selected = ui->tableBatchUniques->selectedItems();
	count = selected.size();
	todelete.clear();
	for (int i = 0; i < count; i++)
	{ todelete.append(selected.at(i)->row()); }
	qSort(todelete);
	rem = 0;
	foreach (int i, todelete)
	{
		m_batchs.removeAt(i - rem);
		ui->tableBatchUniques->removeRow(i - rem);
		rem++;
	}
	updateGroupCount();
}

void mainWindow::batchMoveUp()
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	if (selected.count() <= 0)
		return;

	QList<int> rows;
	int count = selected.size();
	for (int i = 0; i < count; i++)
	{
		int sourceRow = selected.at(i)->row();
		if (rows.contains(sourceRow))
			continue;
		else
			rows.append(sourceRow);
	}
	foreach (int sourceRow, rows)
	{
		int destRow = sourceRow - 1;
		if (destRow < 0 || destRow >= ui->tableBatchGroups->rowCount())
			return;

		QList<QTableWidgetItem*> sourceItems;
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			sourceItems << ui->tableBatchGroups->takeItem(sourceRow, col);
		QList<QTableWidgetItem*> destItems;
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			destItems << ui->tableBatchGroups->takeItem(destRow, col);

		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			ui->tableBatchGroups->setItem(sourceRow, col, destItems.at(col));
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			ui->tableBatchGroups->setItem(destRow, col, sourceItems.at(col));
	}

	if (!selected.empty())
	{
		QItemSelectionModel* selectionModel = new QItemSelectionModel(ui->tableBatchGroups->model());
		QItemSelection selection;
		for (int i = 0; i < count; i++)
		{
			QModelIndex index = ui->tableBatchGroups->model()->index(selected.at(i)->row(), selected.at(i)->column());
			selection.select(index, index);
		}
		selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
		ui->tableBatchGroups->setSelectionModel(selectionModel);
	}
}
void mainWindow::batchMoveDown()
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	if (selected.count() <= 0)
		return;

	QList<int> rows;
	int count = selected.size();
	for (int i = 0; i < count; i++)
	{
		int sourceRow = selected.at(i)->row();
		if (rows.contains(sourceRow))
			continue;
		else
			rows.append(sourceRow);
	}
	foreach (int sourceRow, rows)
	{
		int destRow = sourceRow + 1;
		if (destRow < 0 || destRow >= ui->tableBatchGroups->rowCount())
			return;

		QList<QTableWidgetItem*> sourceItems;
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			sourceItems << ui->tableBatchGroups->takeItem(sourceRow, col);
		QList<QTableWidgetItem*> destItems;
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			destItems << ui->tableBatchGroups->takeItem(destRow, col);

		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			ui->tableBatchGroups->setItem(sourceRow, col, destItems.at(col));
		for (int col = 0; col < ui->tableBatchGroups->columnCount(); ++col)
			ui->tableBatchGroups->setItem(destRow, col, sourceItems.at(col));
	}

	if (!selected.empty())
	{
		QItemSelectionModel* selectionModel = new QItemSelectionModel(ui->tableBatchGroups->model());
		QItemSelection selection;
		for (int i = 0; i < count; i++)
		{
			QModelIndex index = ui->tableBatchGroups->model()->index(selected.at(i)->row(), selected.at(i)->column());
			selection.select(index, index);
		}
		selectionModel->select(selection, QItemSelectionModel::ClearAndSelect);
		ui->tableBatchGroups->setSelectionModel(selectionModel);
	}
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
		int r = x - 1;
		if (r == 0) { r = 1; }
		else if (r == 1) { r = 6; }
		else if (r == 5) { r = 7; }
		else if (r == 6) { r = 8; }
		else if (r == 7) { r = 5; }

		if (r == 3 && ui->tableBatchGroups->item(y, x)->text().toInt() < 1)
		{
			error(this, tr("La limite d'images par page doit être supérieure ou égale à 1."));
			ui->tableBatchGroups->item(y, x)->setText(m_groupBatchs[y][r-1]);
		}
		else if (r == 4 && ui->tableBatchGroups->item(y, x)->text().toInt() < 0)
		{
			error(this, tr("La limite d'imagessupérieure ou égale à 0."));
			ui->tableBatchGroups->item(y, x)->setText(m_groupBatchs[y][r-1]);
		}
		else
		{
			m_groupBatchs[y][r-1] = ui->tableBatchGroups->item(y, x)->text();
			saveLinkList(savePath("restore.igl"));
		}
	}
}
void mainWindow::addGroup()
{
	if (m_tabs.count() > 0)
	{ m_selectedSources = m_tabs[0]->sources(); }
	QString selected;
	for (int i = 0; i < m_selectedSources.count(); i++)
	{
		if (m_selectedSources[i])
		{
			selected = m_sites.keys().at(i);
			break;
		}
	}
	if (selected.isEmpty() && m_sites.size() > 0)
	{ selected = m_sites.keys().at(0); }
	AddGroupWindow *wAddGroup = new AddGroupWindow(selected, m_sites.keys(), m_favorites.keys(), this);
	connect(wAddGroup, SIGNAL(sendData(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	wAddGroup->show();
}
void mainWindow::addUnique()
{
	if (m_tabs.count() > 0)
	{ m_selectedSources = m_tabs[0]->sources(); }
	QString selected;
	for (int i = 0; i < m_selectedSources.count(); i++)
	{
		if (m_selectedSources[i])
		{
			selected = m_sites.keys().at(i);
			break;
		}
	}
	if (selected.isEmpty() && m_sites.size() > 0)
	{ selected = m_sites.keys().at(0); }
	AddUniqueWindow *wAddUnique = new AddUniqueWindow(selected, m_sites, this);
	connect(wAddUnique, SIGNAL(sendData(QMap<QString,QString>)), this, SLOT(batchAddUnique(QMap<QString,QString>)));
	wAddUnique->show();
}

void mainWindow::updateFavorites(bool dock)
{
	m_favoritesTab->updateFavorites();
	if (dock)
	{ updateFavoritesDock(); }
}
void mainWindow::updateFavoritesDock()
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
	m_favorites = loadFavorites();
	QStringList keys = m_favorites.keys();
	QList<QMap<QString,QString> > favorites;

	for (int i = 0; i < keys.size(); i++)
	{
		QMap<QString,QString> d;
		QString tag = keys.at(i);
		d["id"] = QString::number(i);
		d["name"] = tag;
		QStringList xp = m_favorites.value(tag).split("|");
		d["note"] = xp.isEmpty() ? "50" : xp.takeFirst();
		d["lastviewed"] = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)).toString(Qt::ISODate) : xp.takeFirst();
		tag.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
		d["tag"] = tag;
		d["imagepath"] = savePath("thumbs/"+tag+".png");
		if (!QFile::exists(d["imagepath"]))
		{ d["imagepath"] = ":/images/noimage.png"; }
		favorites.append(d);
	}

	qSort(favorites.begin(), favorites.end(), sortByName);
	if (order == "note")
	{ qSort(favorites.begin(), favorites.end(), sortByNote); }
	else if (order == "lastviewed")
	{ qSort(favorites.begin(), favorites.end(), sortByLastviewed); }
	if (reverse)
	{ favorites = reversed(favorites); }
	QString format = tr("dd/MM/yyyy");

	for (int i = 0; i < favorites.size(); i++)
	{
		QLabel *lab = new QLabel(QString("<a href=\"%1\" style=\"color:black;text-decoration:none;\">%2</a>").arg(favorites[i]["name"], favorites[i]["tag"]), this);
		connect(lab, SIGNAL(linkActivated(QString)), this, SLOT(loadTag(QString)));
		lab->setToolTip("<img src=\""+favorites[i]["imagepath"]+"\" /><br/>"+tr("<b>Nom :</b> %1<br/><b>Note :</b> %2 %%<br/><b>Dernière vue :</b> %3").arg(favorites[i]["name"], favorites[i]["note"], QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format)));
		ui->layoutFavoritesDock->addWidget(lab);
	}
}
void mainWindow::updateKeepForLater()
{
	QStringList kpl = loadViewItLater();
	QStringList text = QStringList();

	for (int i = 0; i < kpl.size(); i++)
	{ text.append(QString("<a href=\"%1\" style=\"color:black;text-decoration:none;\">%1</a>").arg(kpl[i])); }

	ui->labelKpl->setText(text.join("<br/>"));
}


void mainWindow::logShow()
{
	if (m_loaded)
	{
		QString txt("");
		int k;
		for (int i = 0; i < _log.size(); i++)
		{
			k = m_settings->value("Log/invert", false).toBool() ? _log.size()-i-1 : i;
			txt += QString(i > 0 ? "<br/>" : "")+"["+_log.keys().at(k).toString("hh:mm:ss.zzz")+"] "+_log.values().at(k);
		}
		ui->labelLog->setText(txt);
	}
}
void mainWindow::logClear()
{
	_log.clear();
	logShow();
}
void mainWindow::logOpen()
{ QDesktopServices::openUrl("file:///"+savePath("main.log")); }

void mainWindow::switchTranslator(QTranslator& translator, const QString& filename)
{
	qApp->removeTranslator(&translator);
	if (translator.load(filename))
	{ qApp->installTranslator(&translator); }
}
void mainWindow::loadLanguage(const QString& rLanguage, bool shutup)
{
	if (m_currLang != rLanguage)
	{
		m_currLang = rLanguage;
		QLocale locale = QLocale(m_currLang);
		QLocale::setDefault(locale);
		switchTranslator(m_translator, qApp->applicationDirPath()+"/languages/"+m_currLang);
		if (!shutup)
		{
			log(tr("Traduction des textes en %1...").arg(m_currLang));
			ui->retranslateUi(this);
			logUpdate(tr(" Fait"));
		}
	}
}
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

void mainWindow::closeEvent(QCloseEvent *e)
{
	log(tr("Sauvegarde..."));
		saveLinkList(savePath("restore.igl"));
		saveTabs(savePath("tabs.txt"));
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
		QFile::copy(m_settings->fileName(), savePath("old/settings."+QString(VERSION)+".ini"));
	DONE();
	m_loaded = false;
	e->accept();
	qApp->quit();
}


void mainWindow::options()
{
	log(tr("Ouverture de la fenêtre des options..."));
	optionsWindow *options = new optionsWindow(this);
	connect(options, SIGNAL(languageChanged(QString)), this, SLOT(loadLanguage(QString)));
	connect(options, SIGNAL(settingsChanged()), this, SLOT(on_buttonInitSettings_clicked()));
	connect(options, SIGNAL(accepted()), this, SLOT(optionsClosed()));
	options->show();
	DONE();
}
void mainWindow::optionsClosed()
{
	m_tabs[0]->optionsChanged();
	m_tabs[0]->updateCheckboxes();
}

void mainWindow::advanced()
{
	log(tr("Ouverture de la fenêtre des sources..."));
	sourcesWindow *adv = new sourcesWindow(m_selectedSources, &m_sites, this);
	adv->show();
	connect(adv, SIGNAL(valid(sourcesWindow*)), this, SLOT(saveAdvanced(sourcesWindow*)));
	DONE();
}
void mainWindow::saveAdvanced(sourcesWindow *w)
{
	log(tr("Sauvegarde des nouvelles sources..."));
	m_selectedSources = w->getSelected();
	QString sav;
	for (int i = 0; i < m_selectedSources.count(); i++)
	{ sav += (m_selectedSources.at(i) ? "1" : "0"); }
	m_settings->setValue("sites", sav);
	for (int i = 0; i < m_tabs.count(); i++)
	{ m_tabs[i]->updateCheckboxes(); }
	DONE();
}

void mainWindow::aboutAuthor()
{
	aboutWindow *aw = new aboutWindow(QString(VERSION), this);
	aw->show();
}

/* Batch download */
void mainWindow::batchSel()
{
	getAll(false);
}
void mainWindow::getAll(bool all)
{
	if (m_getAll)
	{
		log(tr("Lancement d'un téléchargement groupé annulé car un autre est déjà en cours d'éxecution."));
		return;
	}
	if (m_settings->value("Save/path").toString().isEmpty())
	{
		error(this, tr("Vous n'avez pas précisé de dossier de sauvegarde !"));
		return;
	}
	else if (m_settings->value("Save/filename").toString().isEmpty())
	{
		error(this, tr("Vous n'avez pas précisé de nom de fichier !"));
		return;
	}
	log(tr("Téléchargement groupé commencé."));

	m_getAll = true;
	ui->widgetDownloadButtons->setDisabled(m_getAll);

	m_getAllDownloaded = 0;
	m_getAllExists = 0;
	m_getAllIgnored = 0;
	m_getAll404s = 0;
	m_getAllErrors = 0;
	m_getAllCount = 0;
	m_getAllPageCount = 0;
	m_getAllBeforeId = -1;
	m_getAllRequestExists = false;

	m_getAllDownloadingSpeeds.clear();
	qDeleteAll(m_getAllRemaining);
	m_getAllRemaining.clear();
	qDeleteAll(m_getAllFailed);
	m_getAllFailed.clear();
	qDeleteAll(m_getAllDownloading);
	m_getAllDownloading.clear();
	qDeleteAll(m_getAllPages);
	m_getAllPages.clear();

	QList<QTableWidgetItem *> selected = ui->tableBatchUniques->selectedItems();
	int count = selected.size();
	if (!all)
	{
		for (int r = 0; r < count; r++)
		{
			int i = selected.at(r)->row();
			m_getAllRemaining.append(new Image(m_batchs.at(i), m_timezonedecay, new Page(m_sites[m_batchs.at(i).value("site")], &m_sites, m_batchs.at(i).value("tags").split(" "), 1, 1, QStringList(), false, this)));
		}
	}
	else
	{
		for (int i = 0; i < m_batchs.size(); i++)
		{
			if (m_batchs.at(i).value("file_url").isEmpty())
			{
				/*Page *page = new Page(m_sites[site], &m_sites, m_groupBatchs.at(i).at(0).split(' '), m_groupBatchs.at(i).at(1).toInt()+r, pp, QStringList(), false, this);
				connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
				page->load();
				log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
				m_groupBatchs[i][8] += (m_groupBatchs[i][8] == "" ? "" : "¤") + QString::number((int)page);
				m_getAllPages.append(page);
				m_progressdialog->setImagesCount(m_progressdialog->count() + 1);*/
			}
			else
			{ m_getAllRemaining.append(new Image(m_batchs.at(i), m_timezonedecay, new Page(m_sites[m_batchs.at(i).value("site")], &m_sites, m_batchs.at(i).value("tags").split(" "), 1, 1, QStringList(), false, this))); }
		}
	}
	m_getAllLimit = m_batchs.size();

	for (int i = 0; i < m_progressBars.size(); i++)
	{
		m_progressBars.at(i)->setMaximum(100);
		m_progressBars.at(i)->setValue(0);
	}

	m_allow = false;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
	{ ui->tableBatchGroups->item(i, 0)->setIcon(QIcon(":/images/colors/black.png")); }
	m_allow = true;
	Commands::get()->before();
	selected = ui->tableBatchGroups->selectedItems();
	count = selected.size();
	m_batchDownloading.clear();

	m_progressdialog->setText(tr("Téléchargement des pages, veuillez patienter..."));
	connect(m_progressdialog, SIGNAL(rejected()), this, SLOT(getAllCancel()));
	m_progressdialog->clear();

	QSet<int> todownload = QSet<int>();
	for (int i = 0; i < count; i++)
	{ todownload.insert(selected.at(i)->row()); }

	if (all || !todownload.isEmpty())
	{
		m_progressdialog->setImagesCount(0);
		for (int i = 0; i < m_groupBatchs.count(); i++)
		{
			if (all || todownload.contains(i))
			{
				QString site = m_groupBatchs.at(i).at(5);
				int pp = m_groupBatchs.at(i).at(2).toInt();

				m_getAllLimit += m_groupBatchs.at(i).at(3).toDouble();
				m_batchDownloading.insert(i);

				for (int r = 0; r < ceil(m_groupBatchs.at(i).at(3).toDouble()/pp); r++)
				{
					if (!m_sites.keys().contains(site))
					{ log(tr("<b>Attention :</b> %1").arg(tr("site \"%1\" not found.").arg(site))); }
					else
					{
						Page *page = new Page(m_sites[site], &m_sites, m_groupBatchs.at(i).at(0).split(' '), m_groupBatchs.at(i).at(1).toInt()+r, pp, QStringList(), false, this);
						connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
						page->load();

						log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));

						m_groupBatchs[i][8] += (m_groupBatchs[i][8] == "" ? "" : "¤") + QString::number((int)page);
						m_getAllPages.append(page);
						m_progressdialog->setImagesCount(m_progressdialog->count() + 1);
					}
				}
			}
		}
	}

	if (m_getAllPages.isEmpty())
	{
		if (m_getAllRemaining.isEmpty())
		{ return; }
		else
		{ getAllImages(); }
	}

	m_progressdialog->show();
	logShow();
}
void mainWindow::getAllFinishedLoading(Page* p)
{
	log(tr("Page reçue <a href=\"%1\">%1</a> (%n résultat(s))", "", p->images().count()).arg(p->url().toString().toHtmlEscaped()));
	int n = 0;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs[i][8].split("¤", QString::SkipEmptyParts).contains(QString::number((int)p)))
		{
			n = i;
			break;
		}
	}

	QList<Image*> imgs = QList<Image*>(), ims = p->images();
	if (m_groupBatchs[n][4] == "true")
	{ imgs = ims; }
	else
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' ', QString::SkipEmptyParts));
		for (int i = 0; i < ims.size(); i++)
		{
			if (ims[i]->blacklisted(blacklistedtags).isEmpty())
			{ imgs.append(ims[i]); }
			else
			{ log("Blacklisted: "+ims[i]->blacklisted(blacklistedtags).join(", ")); }
		}
	}

	m_progressBars[n]->setMaximum(imgs.size());
	while (imgs.size() > m_groupBatchs[n][2].toInt())
	{ imgs.removeAt(m_groupBatchs[n][2].toInt()); }
	m_getAllRemaining.append(imgs);
	m_getAllCount++;
	m_progressdialog->setImages(m_getAllCount);

	if (m_getAllCount == m_getAllPages.count())
	{
		if (m_getAllRemaining.isEmpty())
		{
			if (ims.isEmpty())
			{ error(this, tr("<b>Attention :</b> %1").arg(tr("rien n'a été reçu depuis %1. Raisons possibles : tag incorrect, page trop éloignée.").arg(p->site()->value("Url")))); }
			else
			{ error(this, tr("<b>Attention :</b> %1").arg(tr("toutes les images provenant de %1 ont été ignorées.").arg(p->site()->value("Url")))); }
			return;
		}
		getAllImages();
	}
}
void mainWindow::getAllImages()
{
	// Si la limite d'images n'est pas un multiple du nombre d'images par page, on retire celles en trop
	while (m_getAllRemaining.count() > m_getAllLimit)
	{ m_getAllRemaining.removeLast(); }

	log(tr("Toutes les urls des images ont été reçues (%n image(s)).", "", m_getAllRemaining.count()));

	m_progressdialog->setText(tr("Préparation des images, veuillez patienter..."));
	int count = 0;
	m_progressdialog->setCount(m_getAllRemaining.count());
	m_progressdialog->setImagesCount(m_getAllRemaining.count());
	for (int i = 0; i < m_getAllRemaining.count(); i++)
	{
		count += m_getAllRemaining[i]->value();
		int n = 0;
		for (int r = 0; r < m_groupBatchs.count(); r++)
		{
			if (m_groupBatchs[r][8].split("¤", QString::SkipEmptyParts).contains(QString::number((int)m_getAllRemaining[i]->page())))
			{
				n = r + 1;
				break;
			}
		}
		m_progressdialog->addImage(m_getAllRemaining[i]->url(), n, m_getAllRemaining[i]->fileSize());
		connect(m_getAllRemaining[i], SIGNAL(urlChanged(QString,QString)), m_progressdialog, SLOT(imageUrlChanged(QString,QString)));
		connect(m_getAllRemaining[i], SIGNAL(urlChanged(QString,QString)), this, SLOT(imageUrlChanged(QString,QString)));
		m_progressdialog->setImages(i+1);
	}

	m_progressdialog->updateColumns();
	m_progressdialog->setImagesCount(m_getAllRemaining.count());
	m_progressdialog->setMaximum(count);
	m_progressdialog->setText(tr("Téléchargement des images en cours..."));
	m_progressdialog->setImages(0);

	m_must_get_tags = false;
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "general";
	for (int f = 0; f < m_groupBatchs.size(); f++)
	{
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (m_groupBatchs[f][6].startsWith("javascript:") || m_groupBatchs[f][6].contains("%"+forbidden.at(i)+"%") || (m_groupBatchs[f][6].contains("%filename%") && m_sites[m_groupBatchs[f][5]]->contains("Regex/ForceImageUrl")))
			{ m_must_get_tags = true; }
		}
	}
	if (!m_batchs.isEmpty())
	{
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (m_settings->value("Save/filename").toString().startsWith("javascript:") || m_settings->value("Save/filename").toString().contains("%"+forbidden.at(i)+"%"))
			{ m_must_get_tags = true; }
		}
	}

	if (m_must_get_tags)
	{ log(tr("Téléchargement des détails des images.")); }
	else
	{ log(tr("Téléchargement des images directement.")); }

	for (int i = 0; i < qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10)); i++)
	{ _getAll(); }
}
void mainWindow::_getAll()
{
	if (m_progressdialog->cancelled())
	{ return; }

	if (m_getAllRemaining.size() > 0)
	{
		m_getAllDownloading.prepend(m_getAllRemaining.takeFirst());

		if (m_must_get_tags)
		{
			m_getAllDownloading.at(0)->loadDetails();
			connect(m_getAllDownloading.at(0), SIGNAL(finishedLoadingTags(Image*)), this, SLOT(getAllPerformTags(Image*)));
		}
		else
		{
			Image *img = m_getAllDownloading.at(0);

			// Row
			int site_id = m_progressdialog->batch(img->url());

			// Path
			QString path = m_settings->value("Save/filename").toString();
			QString pth = m_settings->value("Save/path").toString();
			if (site_id >= 0)
			{
				ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/blue.png"));
				path = m_groupBatchs[site_id][6];
				pth = m_groupBatchs[site_id][7];
			}

			QString p = img->folder().isEmpty() ? pth : img->folder();
			QFile f(p+"/"+img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1));
			if (!f.exists())
			{
				bool detected = false;
				QStringList tags = site_id >= 0 ? m_groupBatchs[site_id][0].split(' ') : QStringList();
				QList<QChar> modifiers = QList<QChar>() << '~';
				for (int r = 0; r < tags.size(); r++)
				{
					if (modifiers.contains(tags[r][0]))
					{ tags[r] = tags[r].right(tags[r].size()-1); }
				}
				if (!m_settings->value("blacklistedtags").toString().isEmpty())
				{
					QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
					detected = !img->blacklisted(blacklistedtags).isEmpty();
				}
				if (detected && site_id >= 0 && m_groupBatchs[site_id][4] == "false")
				{
					m_getAllDownloading.removeAt(0);
					m_progressdialog->setValue(m_progressdialog->value()+img->value());
					m_progressdialog->setImages(m_progressdialog->images()+1);
					m_getAllIgnored++;
					log(tr("Image ignorée."));
					m_progressdialog->loadedImage(img->url());
					m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
					if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
					{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
					_getAll();
				}
				else
				{ getAllGetImage(img); }
			}
			else
			{
				m_progressdialog->setValue(m_progressdialog->value()+img->value());
				m_progressdialog->setImages(m_progressdialog->images()+1);
				m_getAllExists++;
				log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
				m_progressdialog->loadedImage(img->url());
				if (site_id >= 0)
				{
					m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
					if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
					{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
				}
				m_getAllDownloading.removeAt(0);
				_getAll();
			}
		}
	}
	else if (m_getAllDownloading.isEmpty() && m_getAll)
	{
		log("Images download finished.");
		m_progressdialog->setValue(m_progressdialog->maximum());
		switch (m_progressdialog->endAction())
		{
			case 1:	m_progressdialog->close();				break;
			case 2:	openTray();								break;
			case 3:	QSound::play(":/sounds/finished.wav");	break;

			case 4:
				qDebug() << m_progressdialog->count();
				if (false)
				{ shutDown(); }
				break;
		}
		if (m_progressdialog->endRemove())
		{
			int rem = 0;
			foreach (int i, m_batchDownloading)
			{
				m_groupBatchs.removeAt(i - rem);
				m_progressBars.removeAt(i - rem);
				ui->tableBatchGroups->removeRow(i - rem);
				rem++;
			}
		}
		activateWindow();
		m_getAll = false;
		QMessageBox::information(
			this,
			tr("Récupération des images"),
			QString(
				tr("%n fichier(s) récupéré(s) avec succès.", "", m_getAllDownloaded)+"\r\n"+
				tr("%n fichier(s) ignoré(s).", "", m_getAllIgnored)+"\r\n"+
				tr("%n fichier(s) déjà existant(s).", "", m_getAllExists)+"\r\n"+
				tr("%n fichier(s) non trouvé(s) sur le serveur.", "", m_getAll404s)+"\r\n"+
				tr("%n erreur(s).", "", m_getAllErrors)
			)
		);
		int reponse = QMessageBox::No;
		if (m_getAllErrors > 0)
		{
			reponse = QMessageBox::question(this, tr("Récupération des images"), tr("Des erreurs sont survenues pendant le téléchargement des images. Voulez vous relancer le téléchargement de celles-ci ? (%1/%2)").arg(m_getAllErrors).arg(m_getAllDownloaded + m_getAllIgnored + m_getAllExists + m_getAll404s + m_getAllErrors), QMessageBox::Yes | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{
				m_getAll = true;
				m_progressdialog->clear();
				qDeleteAll(m_getAllRemaining);
				m_getAllRemaining.clear();
				m_getAllRemaining = m_getAllFailed;
				m_getAllFailed.clear();
				m_getAllDownloaded = 0;
				m_getAllExists = 0;
				m_getAllIgnored = 0;
				m_getAll404s = 0;
				m_getAllCount = 0;
				m_progressdialog->show();
				getAllImages();
			}
			m_getAllErrors = 0;
		}
		if (reponse != QMessageBox::Yes)
		{
			Commands::get()->after();
			ui->widgetDownloadButtons->setDisabled(false);
			log(tr("Téléchargement groupé terminé"));
		}
	}
}
void mainWindow::imageUrlChanged(QString before, QString after)
{
	m_downloadTimeLast.insert(after, m_downloadTimeLast[before]);
	m_downloadTimeLast.remove(before);
	m_downloadTime.insert(after, m_downloadTime[before]);
	m_downloadTime.remove(before);
}
void mainWindow::getAllProgress(Image *img, qint64 bytesReceived, qint64 bytesTotal)
{
	if (!m_downloadTimeLast.contains(img->url()) || m_downloadTimeLast[img->url()] == NULL)
	{ return; }
	if (m_downloadTimeLast[img->url()]->elapsed() >= 1000)
	{
		m_downloadTimeLast[img->url()]->restart();
		float speed = (bytesReceived * 1000) / m_downloadTime[img->url()]->elapsed();
		m_progressdialog->speedImage(img->url(), speed);
	}
	if (img->fileSize() == 0)
	{
		img->setFileSize(bytesTotal);
		m_progressdialog->sizeImage(img->url(), bytesTotal);
	}
	m_progressdialog->statusImage(img->url(), (bytesReceived * 100) / bytesTotal);
}
void mainWindow::getAllPerformTags(Image* img)
{
	if (m_progressdialog->cancelled())
	{ return; }

	log(tr("Tags reçus"));

	// Row
	int site_id = m_progressdialog->batch(img->url());

	// Getting path
	QString path = m_settings->value("Save/filename").toString();
	QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		path = m_groupBatchs[site_id][6];
		p = m_groupBatchs[site_id][7];
	}
	path = img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1);

	// Save path
	p.replace("\\", "/");
	if (p.right(1) == "/")
	{ p = p.left(p.length()-1); }
	QString pth = p+"/"+path;

	int m_getAllId = -1;
	for (int i = 0; i < m_getAllDownloading.count(); i++)
	{
		if (m_getAllDownloading[i]->fileUrl() == img->fileUrl())
		{ m_getAllId = i; }
	}

	QFile f(pth);
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".png");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".gif");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".jpeg");	}
	if (!f.exists())
	{
		bool detected = false;
		QStringList tags = site_id >= 0 ? m_groupBatchs[site_id][0].split(' ') : QStringList();
		QList<QChar> modifiers = QList<QChar>() << '~';
		for (int r = 0; r < tags.size(); r++)
		{
			if (modifiers.contains(tags[r][0]))
			{ tags[r] = tags[r].right(tags[r].size()-1); }
		}
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
			detected = !img->blacklisted(blacklistedtags).isEmpty();
		}
		if (detected && site_id >= 0 && m_groupBatchs[site_id][4] == "false")
		{
			m_progressdialog->setValue(m_progressdialog->value()+img->value());
			m_progressdialog->setImages(m_progressdialog->images()+1);
			m_getAllIgnored++;
			log(tr("Image ignorée."));
			m_progressdialog->loadedImage(img->url());
			m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
			if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
			{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
			m_getAllDownloadingSpeeds.remove(img->url());
			m_getAllDownloading.removeAt(m_getAllId);
			_getAll();
		}
		else
		{ getAllGetImage(img); }
	}
	else
	{
		m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_progressdialog->images()+1);
		m_getAllExists++;
		log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
		m_progressdialog->loadedImage(img->url());
		if (site_id >= 0)
		{
			m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
			if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
			{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
		}
		m_getAllDownloadingSpeeds.remove(img->url());
		m_getAllDownloading.removeAt(m_getAllId);
		_getAll();
	}
}
void mainWindow::getAllGetImage(Image* img)
{
	// Row
	int site_id = m_progressdialog->batch(img->url());
	int m_getAllId = -1;
	for (int i = 0; i < m_getAllDownloading.count(); i++)
	{
		if (m_getAllDownloading[i]->url() == img->url())
		{ m_getAllId = i; }
	}

	// Path
	QString path = m_settings->value("Save/filename").toString();
	QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/blue.png"));
		path = m_groupBatchs[site_id][6];
		p = m_groupBatchs[site_id][7];
	}
	path = img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1);
	path.replace("%n%", QString::number(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors));
	if (path.left(1) == QDir::toNativeSeparators("/"))	{ path = path.right(path.length()-1);	}
	if (p.right(1) == QDir::toNativeSeparators("/"))	{ p = p.left(p.length()-1);				}
	QString fp = QDir::toNativeSeparators(p+"/"+path);

	// Action
	QString whatToDo = m_settings->value("Save/md5Duplicates", "save").toString();
	QString md5Duplicate = md5Exists(img->md5());
	bool next = true;
	if (md5Duplicate.isEmpty() || whatToDo == "save")
	{
		log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a> %2").arg(img->fileUrl().toString()).arg(m_getAllDownloading.size()));
		m_progressdialog->loadingImage(img->url());
		m_downloadTime.insert(img->url(), new QTime);
		m_downloadTime[img->url()]->start();
		m_downloadTimeLast.insert(img->url(), new QTime);
		m_downloadTimeLast[img->url()]->start();
		connect(img, SIGNAL(finishedImage(Image*)), this, SLOT(getAllPerformImage(Image*)), Qt::UniqueConnection);
		connect(img, SIGNAL(downloadProgressImage(Image*,qint64,qint64)), this, SLOT(getAllProgress(Image*,qint64,qint64)), Qt::UniqueConnection);
		m_getAllDownloadingSpeeds.insert(img->url(), 0);
		img->loadImage();
		next = false;
	}
	else if (whatToDo == "copy")
	{
		m_getAllIgnored++;
		log(tr("Copie depuis <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(fp));
		QFile::copy(md5Duplicate, fp);
	}
	else if (whatToDo == "move")
	{
		m_getAllDownloaded++;
		log(tr("Déplacement depuis <a href=\"file:///%1\">%1</a> vers <a href=\"file:///%2\">%2</a>").arg(md5Duplicate).arg(fp));
		QFile::rename(md5Duplicate, fp);
		setMd5(img->md5(), fp);
	}
	else
	{
		m_getAllIgnored++;
		log(tr("MD5 \"%1\" de l'image <a href=\"%2\">%2</a> déjà existant dans le fichier <a href=\"file:///%3\">%3</a>").arg(img->md5(), md5Duplicate));
	}

	// Continue to next image
	if (next)
	{
		m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_progressdialog->images()+1);
		m_getAllDownloadingSpeeds.remove(img->url());
		m_getAllDownloading.removeAt(m_getAllId);
		_getAll();
	}
}
void mainWindow::getAllPerformImage(Image* img)
{
	if (m_progressdialog->cancelled())
	{ return; }

	QNetworkReply* reply = img->imageReply();

	if (reply->error() == QNetworkReply::OperationCanceledError)
	{ return; }
	log(tr("Image reçue depuis <a href=\"%1\">%1</a> %2").arg(reply->url().toString()).arg(m_getAllDownloading.size()));

	// Row
	int site_id = m_progressdialog->batch(img->url());

	int m_getAllId = -1;
	for (int i = 0; i < m_getAllDownloading.count(); i++)
	{
		if (m_getAllDownloading[i]->url() == img->url())
		{ m_getAllId = i; }
	}

	int errors = m_getAllErrors, e404s = m_getAll404s;
	if (reply->error() == QNetworkReply::NoError)
	{
		if (site_id >= 0)
		{
			ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/blue.png"));
			saveImage(img, reply, m_groupBatchs[site_id][6], m_groupBatchs[site_id][7]);
		}
		else
		{ saveImage(img, reply); }
	}
	else if (reply->error() == QNetworkReply::ContentNotFoundError)
	{ m_getAll404s++; }
	else
	{
		log(tr("Erreur inconnue pour l'image: <a href=\"%1\">%1</a>. \"%2\"").arg(img->url().toHtmlEscaped(), reply->errorString()), Error);
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
		m_getAllFailed.append(m_getAllDownloading[m_getAllId]);
	}

	if (site_id >= 0)
	{
		m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
		if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
		{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
	}

	m_progressdialog->setValue(m_progressdialog->value()+img->value());
	m_progressdialog->setImages(m_progressdialog->images()+1);
	m_getAllDownloadingSpeeds.remove(img->url());
	m_getAllDownloading.removeAt(m_getAllId);

	_getAll();
}
void mainWindow::saveImage(Image *img, QNetworkReply *reply, QString path, QString p, bool getAll)
{
	if (reply == NULL || !reply->isReadable())
	{
		reply = img->imageReply();
		if (reply == NULL || !reply->isReadable())
		{ return; }
	}

	// Path
	if (path == "")
	{ path = m_settings->value("Save/filename").toString(); }
	if (p == "")
	{ p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder(); }
	path = img->path(path, p, m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors + 1);
	if (getAll)
	{ path.replace("%n%", QString::number(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors)); }

	if (path.left(1) == DIR_SEPARATOR)	{ path = path.right(path.length()-1);	}
	if (p.right(1) == DIR_SEPARATOR)	{ p = p.left(p.length()-1);				}
	QString fp = QDir::toNativeSeparators(p+"/"+path);

	QString whatToDo = m_settings->value("Save/md5Duplicates", "save").toString();
	QString md5Duplicate = md5Exists(img->md5());
	if (md5Duplicate.isEmpty() || whatToDo == "save")
	{
		QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2)), dir(p);
		if (!path_to_file.exists() && !dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
		{
			log(tr("Impossible de créer le dossier de destination: %1.").arg(p+"/"+path.section('/', 0, -2)), Error);
			if (getAll)
			{ m_getAllErrors++; }
		}
		else
		{
			QByteArray data = reply->readAll();
			if (!data.isEmpty())
			{
				addMd5(img->md5(), fp);
				QFile f(fp);
				f.open(QIODevice::WriteOnly);
				f.write(data);
				f.close();

				if (m_settings->value("Textfile/activate", false).toBool())
				{
					QString contents = img->path(m_settings->value("Textfile/content", "%all%").toString(), "");
					QFile file_tags(fp + ".txt");
					file_tags.open(QFile::WriteOnly);
					file_tags.write(contents.toLatin1());
					file_tags.close();
				}
			}
			else
			{
				log(tr("Rien n'a été reçu pour l'image: <a href=\"%1\">%1</a>.").arg(img->url().toHtmlEscaped()), Error);
				if (getAll)
				{ m_getAllErrors++; }
			}

			for (int i = 0; i < img->tags().count(); i++)
			{ Commands::get()->tag(img->tags().at(i)); }
			Commands::get()->image(img, fp);
		}
	}
}

void mainWindow::getAllCancel()
{
	log(tr("Annulation des téléchargements..."));
	m_progressdialog->cancel();
	for (int i = 0; i < m_getAllDownloading.size(); i++)
	{
		m_getAllDownloading[i]->abortTags();
		m_getAllDownloading[i]->abortImage();
	}
	m_progressdialog->clear();
	m_getAll = false;
	ui->widgetDownloadButtons->setDisabled(m_getAll);
	DONE();
}
void mainWindow::getAllPause()
{
	if (m_progressdialog->isPaused())
	{
		log(tr("Mise en pause des téléchargements..."));
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			m_getAllDownloading[i]->abortTags();
			m_getAllDownloading[i]->abortImage();
		}
		m_getAll = false;
	}
	else
	{
		log(tr("Reprise des téléchargements..."));
		for (int i = 0; i < m_getAllDownloading.size(); i++)
		{
			if (m_getAllDownloading[i]->tagsReply() != NULL)
			{ m_getAllDownloading[i]->loadDetails(); }
			if (m_getAllDownloading[i]->imageReply() != NULL)
			{ m_getAllDownloading[i]->loadImage(); }
		}
		m_getAll = true;
	}
	DONE();
}

void mainWindow::blacklistFix()
{
	BlacklistFix *bf = new BlacklistFix(m_sites, this);
	bf->show();
}
void mainWindow::emptyDirsFix()
{
	EmptyDirsFix *edf = new EmptyDirsFix(this);
	edf->show();
}
void mainWindow::md5FixOpen()
{
	md5Fix *md5f = new md5Fix(this);
	md5f->show();
}

void mainWindow::on_buttonSaveLinkList_clicked()
{
	QString save = QFileDialog::getSaveFileName(this, tr("Enregistrer la liste de liens"), QString(), tr("Liens Imageboard-Grabber (*.igl)"));
	if (save.isEmpty())
	{ return; }

	if (saveLinkList(save))
	{ QMessageBox::information(this, tr("Enregistrer la liste de liens"), tr("Liste de liens enregistrée avec succès !")); }
	else
	{ QMessageBox::critical(this, tr("Enregistrer la liste de liens"), tr("Erreur lors de l'ouverture du fichier.")); }
}
bool mainWindow::saveLinkList(QString filename)
{
	QByteArray links = "[IGL 2]\r\n";
	for (int i = 0; i < m_groupBatchs.size(); i++)
	{
		links.append(m_groupBatchs[i].join(QString((char)29)));
		links.append(QString((char)29)+QString::number(m_progressBars[i]->value())+"/"+QString::number(m_progressBars[i]->maximum()));
		links.append((char)28);
	}
	QStringList vals = QStringList() << "id" << "md5" << "rating" << "tags" << "file_url" << "site" << "filename" << "folder";
	for (int i = 0; i < m_batchs.size(); i++)
	{
		for (int j = 0; j < vals.size(); j++)
		{
			if (j != 0)
			{ links.append((char)29); }
			links.append(m_batchs[i][vals[j]]);
		}
		links.append((char)28);
	}

	QFile f(filename);
	if (f.open(QFile::WriteOnly))
	{
		f.write(links);
		f.close();
		return true;
	}
	return false;
}

void mainWindow::on_buttonLoadLinkList_clicked()
{
	QString load = QFileDialog::getOpenFileName(this, tr("Charger une liste de liens"), QString(), tr("Liens Imageboard-Grabber (*.igl)"));
	if (load.isEmpty())
	{ return; }

	if (loadLinkList(load))
	{ QMessageBox::information(this, tr("Charger une liste de liens"), tr("Liste de liens chargée avec succès !")); }
	else
	{ QMessageBox::critical(this, tr("Charger une liste de liens"), tr("Erreur lors de l'ouverture du fichier.")); }
}
bool mainWindow::loadLinkList(QString filename)
{
	QFile f(filename);
	if (f.open(QFile::ReadOnly))
	{
		QString header = f.readLine().trimmed();
		int version = header.mid(5, header.size() - 6).toInt();

		QString links = f.readAll();
		f.close();

		QStringList det;
		if (version == 1)
		{ det = links.split("\r\n", QString::SkipEmptyParts); }
		else
		{ det = links.split(QString((char)28), QString::SkipEmptyParts); }
		if (det.size() < 1)
		{ return false; }

		for (int i = 0; i < det.size(); i++)
		{
			QString link = det[i];
			m_allow = false;
			QStringList infos;
			if (version == 1)
			{ infos = link.split("¤"); }
			else
			{ infos = link.split((char)29); }
			if (infos.size() == 8)
			{
				QStringList vals = QStringList() << "id" << "md5" << "rating" << "tags" << "file_url" << "site" << "filename" << "folder";
				QMap<QString,QString> values;
				for (int i = 0; i < infos.size(); i++)
				{ values.insert(vals[i], infos[i]); }
				batchAddUnique(values);
			}
			else
			{
				ui->tableBatchGroups->setRowCount(ui->tableBatchGroups->rowCount()+1);
				QString last = infos.takeLast();
				int max = last.right(last.indexOf("/")+1).toInt(), val = last.left(last.indexOf("/")).toInt();
				QTableWidgetItem *item, *it = new QTableWidgetItem(QIcon(":/images/colors/"+QString(val == max ? "green" : (val > 0 ? "blue" : "black"))+".png"), QString::number(ui->tableBatchGroups->rowCount()));
				ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, 0, it);
				for (int t = 0; t < infos.count(); t++)
				{
					item = new QTableWidgetItem;
						item->setText(infos.at(t));
					int r = t+1;
					if (r == 1) { r = 0; }
					else if (r == 6) { r = 1; }
					else if (r == 7) { r = 5; }
					else if (r == 8) { r = 6; }
					else if (r == 5) { r = 7; }
					ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, r+1, item);
				}
				m_groupBatchs.append(infos);
				QProgressBar *prog = new QProgressBar(this);
				prog->setMaximum(max);
				prog->setValue(val);
				prog->setTextVisible(false);
				m_progressBars.append(prog);
				ui->tableBatchGroups->setCellWidget(ui->tableBatchGroups->rowCount()-1, 9, prog);
				m_allow = true;
			}
		}
		updateGroupCount();
		return true;
	}
	return false;
}

void mainWindow::loadTag(QString tag)
{
	if (tag.startsWith("http://"))
	{
		QDesktopServices::openUrl(tag);
		return;
	}

	if (m_tabs.count() > 0 && ui->tabWidget->currentIndex() < m_tabs.count())
	{ m_tabs[ui->tabWidget->currentIndex()]->setTags(tag); }
}

void mainWindow::on_buttonFolder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory(this, tr("Choisir un dossier de sauvegarde"), ui->lineFolder->text());
	if (!folder.isEmpty())
	{
		ui->lineFolder->setText(folder);
		updateCompleters();
		saveSettings();
	}
}
void mainWindow::on_buttonSaveSettings_clicked()
{
	if (!QDir(ui->lineFolder->text()).exists())
	{ QDir::root().mkpath(ui->lineFolder->text()); }
	m_settings->setValue("Save/path_real", ui->lineFolder->text());
	m_settings->setValue("Save/filename_real", ui->lineFilename->text());
	saveSettings();
}
void mainWindow::on_buttonInitSettings_clicked()
{
	ui->lineFolder->setText(m_settings->value("Save/path_real").toString());
	ui->lineFilename->setText(m_settings->value("Save/filename_real").toString());
	Commands::get()->init(m_settings);
	saveSettings();
}
void mainWindow::updateCompleters()
{
	if (ui->lineFolder->text() != m_settings->value("Save/path").toString())
	{
		m_lineFolder_completer.append(ui->lineFolder->text());
		ui->lineFolder->setCompleter(new QCompleter(m_lineFolder_completer));
	}
	if (ui->labelFilename->text() != m_settings->value("Save/filename").toString())
	{
		m_lineFilename_completer.append(ui->lineFilename->text());
		ui->lineFilename->setCompleter(new QCompleter(m_lineFilename_completer));
	}
}
void mainWindow::saveSettings()
{
	m_settings->setValue("Save/path", ui->lineFolder->text());
	m_settings->setValue("Save/filename", ui->lineFilename->text());
	ui->labelFilename->setText(validateFilename(ui->lineFilename->text()));
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
	{ setWindowTitle(""); }
	else
	{ setWindowTitle(tr("%n téléchargement(s) en cours", "", m_downloads)); }
}
