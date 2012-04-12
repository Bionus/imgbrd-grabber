#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include "functions.h"
#include "json.h"
#include "windows.h"
#include <QtXml>
#include <QtScript>
#include <float.h>



#define VERSION	"3.1.0a3"
#define DONE()	logUpdate(QObject::tr(" Fait"))

extern QMap<QDateTime,QString> _log;

/*
  deviantart.org
  g.e-hentai.org
  minitokyo.net
  pixiv.net
*/

/*
  QString inits = "var artist = \"ishikei\";";
  QString prgm = "artist + \" lolollo\"";
  QString filename = QScriptEngine().evaluate(QScriptProgram(inits + prgm)).toString();
*/



mainWindow::mainWindow(QString program, QStringList tags, QStringMap params) : ui(new Ui::mainWindow), m_currentFav(-1), m_loaded(false), m_getAll(false), m_program(program), m_tags(tags), m_params(params)
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
		QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_settings->value("Proxy/hostName").toString(), m_settings->value("Proxy/port").toInt());
		QNetworkProxy::setApplicationProxy(proxy);
		log(tr("Activation du proxy général sur l'hôte \"%1\" et le port %2.").arg(m_settings->value("Proxy/hostName").toString()).arg(m_settings->value("Proxy/port").toInt()));
	}

	m_serverDate = QDateTime::currentDateTime().toUTC().addSecs(-60*60*4);
	m_timezonedecay = QDateTime::currentDateTime().time().hour()-m_serverDate.time().hour();
	m_gotMd5 = QStringList();
	m_mergeButtons = QList<QBouton*>();
	m_progressBars = QList<QProgressBar*>();
	m_progressdialog = new batchWindow(this);

	ui->tableBatchGroups->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	ui->tableBatchUniques->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

	// Searching for availables sites
	QStringMapMap stes;
	QStringList dir = QDir(savePath("sites")).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (int i = 0; i < dir.count(); i++)
	{
		QFile file(savePath("sites/"+dir.at(i)+"/model.xml"));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString source = file.readAll();
			QDomDocument doc;
			QString errorMsg;
			int errorLine, errorColumn;
			if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
			{ log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Error); }
			else
			{
				QDomElement docElem = doc.documentElement();
				QStringMap details = domToMap(docElem);
				QStringList defaults = QStringList() << "xml" << "json" << "rss" << "regex";
				QString curr;
				QStringList source;
				for (int s = 0; s < 4; s++)
				{
					QString t = m_settings->value("source_"+QString::number(s+1), defaults.at(s)).toString();
					t[0] = t[0].toUpper();
					if (details.contains("Urls/"+(t == "Regex" ? "Html" : t)+"/Tags"))
					{ source.append(t); }
				}
				if (!source.isEmpty())
				{
					QFile f(savePath("sites/"+dir.at(i)+"/sites.txt"));
					if (f.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						while (!f.atEnd())
						{
							QString line = f.readLine();
							line.remove("\n").remove("\r");
							QStringList srcs;
							if (line.contains(':'))
							{
								curr = line.section(':', 1).toLower();
								curr[0] = curr[0].toUpper();
								srcs.append(curr);
								line = line.section(':', 0, 0);
							}
							else
							{ srcs = source; }
							stes[line] = details;
							for (int i = 0; i < srcs.size(); i++)
							{
								stes[line]["Urls/"+QString::number(i+1)+"/Tags"] = "http://"+line+stes[line]["Urls/"+(srcs[i] == "Regex" ? "Html" : srcs[i])+"/Tags"];
								if (stes[line].contains("Urls/"+(srcs[i] == "Regex" ? "Html" : srcs[i])+"/Limit"))
								{ stes[line]["Urls/"+QString::number(i+1)+"/Limit"] = stes[line]["Urls/"+(srcs[i] == "Regex" ? "Html" : srcs[i])+"/Limit"]; }
								if (stes[line].contains("Urls/"+(srcs[i] == "Regex" ? "Html" : srcs[i])+"/Home"))
								{ stes[line]["Urls/"+QString::number(i+1)+"/Home"] = "http://"+line+stes[line]["Urls/"+(srcs[i] == "Regex" ? "Html" : srcs[i])+"/Home"]; }
							}
							stes[line]["Url"] = line;
							stes[line]["Urls/Html/Post"] = "http://"+line+stes[line]["Urls/Html/Post"];
							if (stes[line].contains("Urls/Html/Tags"))
							{ stes[line]["Urls/Html/Tags"] = "http://"+line+stes[line]["Urls/Html/Tags"]; }
							if (stes[line].contains("Urls/Html/Home"))
							{ stes[line]["Urls/Html/Home"] = "http://"+line+stes[line]["Urls/Html/Home"]; }
							stes[line]["Selected"] = srcs.join("/").toLower();
						}
					}
					else
					{ log(tr("Fichier sites.txt du modèle %1 introuvable.").arg(dir.at(i)), Error); }
					f.close();
				}
				else
				{ log(tr("Aucune source valide trouvée dans le fichier model.xml de %1.").arg(dir.at(i))); }
			}
		}
		file.close();
	}
	m_sites = stes;

	ui->actionClosetab->setShortcut(QKeySequence("Ctrl+W"));
	ui->actionAddtab->setShortcut(QKeySequence::AddTab);
	ui->actionQuit->setShortcut(QKeySequence::Quit);
	ui->actionFolder->setShortcut(QKeySequence::Open);

	log(tr("Chargement des préférences depuis <a href=\"file:///%1\">%1</a>").arg(savePath("settings.ini")));
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		ui->comboOrderfavorites->setCurrentIndex(assoc.indexOf(m_settings->value("Favorites/order", "name").toString()));
		ui->comboOrderasc->setCurrentIndex(int(m_settings->value("Favorites/reverse", false).toBool()));
		m_settings->setValue("reverse", bool(ui->comboOrderasc->currentIndex() == 1));
	loadLanguage(m_settings->value("language", "English").toString());

	ui->widgetFavoritesResults->hide();

	connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	updateFavorites();
	updateFavoritesDock();
	updateKeepForLater();

	if (m_settings->value("firstload", true).toBool())
	{
		QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "Mozilla", "Firefox");
		QString path = QFileInfo(cfg.fileName()).absolutePath()+"/Firefox";
		QSettings profiles(path+"/profiles.ini", QSettings::IniFormat);
		if (QFile::exists(path+"/"+profiles.value("Profile0/Path").toString()+"/extensions/danbooru_downloader@cuberocks.net.xpi"))
		{
			int reponse = QMessageBox::question(this, tr("Grabber"), tr("L'extension pour Mozilla Firefox \"Danbooru Downloader\" a été détéctée sur votre système. Souhaitez-vous en importer les préférences ?"), QMessageBox::Yes | QMessageBox::No);
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
						QStringMap firefox, assoc;
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
	QString sel = '1'+QString().fill('0',stes.count()-1);
	QString sav = m_settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selectedSources.append(sav.at(i) == '1' ? true : false);
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
		int reponse = QMessageBox::question(this, tr("Grabber"), tr("Il semblerait que l'application n'ait pas été arrêtée correctement lors de sa dernière utilisation. Voulez-vous restaurer votre dernière seesion ?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{ restore = true; }
	}
	if (restore)
	{
		loadLinkList(savePath("restore.igl"));
		loadTabs(savePath("tabs.txt"));
	}
	if (m_tabs.isEmpty())
	{ addTab(); }

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
			ui->tabWidget->setCurrentIndex(m_tabs.size()+1);
			m_tags.clear();
		}
		else
		{ m_tabs[0]->setTags(this->m_tags.join(" ")); }
	}

	QHeaderView *headerView = ui->tableBatchGroups->horizontalHeader();
	headerView->setResizeMode(QHeaderView::Interactive);
	headerView = ui->tableBatchUniques->horizontalHeader();
	headerView->setResizeMode(QHeaderView::Interactive);
	ui->lineFilename->setText(m_settings->value("Save/filename").toString());
	ui->lineFolder->setText(m_settings->value("Save/path").toString());

	QStringList sizes = m_settings->value("batch", "100,100,100,100,100,100,100,100,100").toString().split(',');
	int m = sizes.size() > ui->tableBatchGroups->columnCount() ? ui->tableBatchGroups->columnCount() : sizes.size();
	for (int i = 0; i < m; i++)
	{ ui->tableBatchGroups->horizontalHeader()->resizeSection(i, sizes.at(i).toInt()); }

	m_loaded = true;
	logShow();
}

mainWindow::~mainWindow()
{
	delete ui;
}

int mainWindow::addTab(QString tag)
{
	searchTab *w = new searchTab(m_tabs.size(), &m_sites, &m_favorites, &m_serverDate, this);
	connect(w, SIGNAL(batchAddGroup(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	connect(w, SIGNAL(titleChanged(searchTab*)), this, SLOT(updateTabTitle(searchTab*)));
	connect(w, SIGNAL(changed(searchTab*)), this, SLOT(updateTabs()));
	connect(w, SIGNAL(closed(searchTab*)), this, SLOT(tabClosed(searchTab*)));
	int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+(!m_tabs.isEmpty()), w, tr("Nouvel onglet"));
	m_tabs.append(w);
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
bool mainWindow::saveTabs(QString filename)
{
	QStringList tabs = QStringList();
	foreach (searchTab *tab, m_tabs)
	{ tabs.append(tab->tags()+"¤"+QString::number(tab->ui->spinPage->value())+"¤"+QString::number(tab->ui->spinImagesPerPage->value())+"¤"+QString::number(tab->ui->spinColumns->value())); }

	QFile *f = new QFile(filename, this);
	if (f->open(QFile::WriteOnly))
	{
		f->write(tabs.join("\r\n").toUtf8());
		f->close();
		return true;
	}
	return false;
}
bool mainWindow::loadTabs(QString filename)
{
	QFile *f = new QFile(filename, this);
	if (f->open(QFile::ReadOnly))
	{
		QString links = f->readAll().trimmed();
		f->close();

		QStringList tabs = links.split("\r\n");
		for (int j = 0; j < tabs.size(); j++)
		{
			QStringList infos = tabs[j].split("¤");
			if (infos.size() > 3)
			{
				int i = addTab(infos[0]);
				m_tabs[i]->ui->spinPage->setValue(infos[1].toInt());
				m_tabs[i]->ui->spinImagesPerPage->setValue(infos[2].toInt());
				m_tabs[i]->ui->spinColumns->setValue(infos[3].toInt());
			}
		}
		return true;
	}
	return false;
}
void mainWindow::addTabFavorite(int id)
{
	QString tag = m_favorites.keys().at(id);
	addTab(tag);
}
void mainWindow::updateTabTitle(searchTab *tab)
{ ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), tab->windowTitle()); }
void mainWindow::updateTabs()
{ saveTabs(savePath("tabs.txt")); }
void mainWindow::tabClosed(searchTab *tab)
{ m_tabs.removeAll(tab); }
void mainWindow::currentTabChanged(int tab)
{
	if (m_loaded)
	{
		if (ui->tabWidget->widget(tab)->maximumWidth() != 16777214)
		{
			ui->labelTags->setText(m_tabs[tab]->results());
			ui->labelWiki->setText("<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+m_tabs[tab]->wiki());
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
}
void mainWindow::batchAddUnique(QStringMap values)
{
	if (!m_batchs.removeOne(values))
	{
		log(tr("Ajout d'une image en téléchargement unique : %1").arg(values.value("file_url")));
		m_batchs.append(values);
		QStringList types = QStringList() << "id" << "md5" <<  "rating" << "tags" << "file_url" << "site" << "filename" << "folder";
		QTableWidgetItem *item;
		ui->tableBatchUniques->setRowCount(ui->tableBatchUniques->rowCount()+1);
		for (int t = 0; t < types.count(); t++)
		{
			QString v;
			if (types.at(t) == "rating")
			{
				QStringMap assoc;
					assoc["s"] = tr("Safe");
					assoc["q"] = tr("Questionable");
					assoc["e"] = tr("Explicit");
				v = assoc[values.value(types.at(t))];
			}
			else
			{ v = values.value(types.at(t)); }
			item = new QTableWidgetItem;
				item->setFlags(Qt::NoItemFlags);
				item->setText(v);
			ui->tableBatchUniques->setItem(ui->tableBatchUniques->rowCount()-1, t, item);
		}
	}
	else
	{
		//ui->tableBatchUniques->removeRow(0);
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
}
void mainWindow::batchClearSel()
{
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	int count = selected.size();
	QSet<int> todelete = QSet<int>();
	for (int i = 0; i < count; i++)
	{ todelete.insert(selected.at(i)->row()); }
	int rem = 0;
	foreach (int i, todelete)
	{
		m_groupBatchs.removeAt(i-rem);
		m_progressBars.removeAt(i-rem);
		ui->tableBatchGroups->removeRow(i-rem);
		rem++;
	}

	selected = ui->tableBatchUniques->selectedItems();
	count = selected.size();
	todelete.clear();
	for (int i = 0; i < count; i++)
	{ todelete.insert(selected.at(i)->row()); }
	rem = 0;
	foreach (int i, todelete)
	{
		m_batchs.removeAt(i-rem);
		ui->tableBatchUniques->removeRow(i-rem);
		rem++;
	}
}
void mainWindow::batchChange(int id)
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
		m_groupBatchs[y][r-1] = ui->tableBatchGroups->item(y, x)->text();
		saveLinkList(savePath("restore.igl"));
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
	wAddUnique->show();
}

#include <algorithm>
template <typename T>
QList<T> reversed(const QList<T> & in)
{
	QList<T> result;
	std::reverse_copy(in.begin(), in.end(), std::back_inserter(result));
	return result;
}
bool sortByNote(const QStringMap &s1, const QStringMap &s2)
{ return s1["note"].toInt() < s2["note"].toInt(); }
bool sortByName(const QStringMap &s1, const QStringMap &s2)
{ return s1["name"].toLower() < s2["name"].toLower(); }
bool sortByLastviewed(const QStringMap &s1, const QStringMap &s2)
{ return QDateTime::fromString(s1["lastviewed"], Qt::ISODate) < QDateTime::fromString(s2["lastviewed"], Qt::ISODate); }
void mainWindow::updateFavorites(bool dock)
{
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[ui->comboOrderfavorites->currentIndex()];
	bool reverse = (ui->comboOrderasc->currentIndex() == 1);
	m_favorites = loadFavorites();
	QStringList keys = m_favorites.keys();
	QList<QStringMap > favorites;
	for (int i = 0; i < keys.size(); i++)
	{
		QStringMap d;
		QString tag = keys.at(i);
		d["id"] = QString::number(i);
		d["name"] = tag;
		QStringList xp = m_favorites.value(tag).split("|");
		d["note"] = xp.isEmpty() ? "50" : xp.takeFirst();
		d["lastviewed"] = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)).toString(Qt::ISODate) : xp.takeFirst();
		tag.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
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

	for (int i = 0; i < favorites.count(); i++)
	{
		QString tag = favorites[i]["tag"];
		tag.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
		QPixmap img(favorites[i]["imagepath"]);
		if ((img.width() > 150 || img.height() > 150) && QFile::exists(favorites[i]["imagepath"]))
		{
			img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			img.save(savePath("thumbs/"+tag+".png"), "PNG");
		}
		QString xt = tr("<b>Nom :</b> %1<br/><b>Note :</b> %2 %%<br/><b>Dernière vue :</b> %3").arg(favorites[i]["name"], favorites[i]["note"], QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format));
		if (m_favoritesImages.count() > i)
		{
			m_favoritesImages.at(i)->setIcon(img);
			m_favoritesImages.at(i)->setIconSize(img.size());
			m_favoritesImages.at(i)->setId(favorites[i]["id"].toInt());
			m_favoritesImages.at(i)->setToolTip(xt);
			m_favoritesCaptions.at(i)->setText(favorites[i]["name"]+"<br/>("+favorites[i]["note"]+" % - "+QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format)+")");
		}
		else
		{
			QBouton *image = new QBouton(favorites[i]["id"].toInt(), this, m_settings->value("resizeInsteadOfCropping", true).toBool(), QColor(), this);
				image->setIcon(img);
				image->setIconSize(img.size());
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
				connect(image, SIGNAL(middleClick(int)), this, SLOT(addTabFavorite(int)));
			QAffiche *caption = new QAffiche(favorites[i]["id"].toInt(), 0 ,QColor(), this);
				caption->setText(favorites[i]["name"]+"<br/>("+favorites[i]["note"]+" % - "+QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format)+")");
				caption->setTextFormat(Qt::RichText);
				caption->setAlignment(Qt::AlignCenter);
				caption->setToolTip(xt);
			connect(image, SIGNAL(appui(int)), this, SLOT(loadFavorite(int)));
			connect(caption, SIGNAL(clicked(int)), this, SLOT(loadFavorite(int)));
			ui->layoutFavorites->addWidget(image, (i/10)*2, i%10);
			ui->layoutFavorites->addWidget(caption, (i/10)*2+1, i%10);
			m_favoritesImages.append(image);
			m_favoritesCaptions.append(caption);
		}
	}
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
	QString order = assoc[ui->comboOrderFav->currentIndex()];
	bool reverse = (ui->comboAscFav->currentIndex() == 1);
	m_favorites = loadFavorites();
	QStringList keys = m_favorites.keys();
	QList<QStringMap > favorites;

	for (int i = 0; i < keys.size(); i++)
	{
		QStringMap d;
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
void mainWindow::loadFavorite(int id)
{
	ui->widgetFavoritesResults->show();
	QString tag = m_favorites.keys().at(id);
	m_currentFavorite = tag;
	m_loadFavorite = QDateTime::fromString(m_favorites.value(tag).section('|', 1, 1), Qt::ISODate);
	web(tag);
}
void mainWindow::checkFavorites()
{
	ui->widgetFavoritesResults->show();
	m_currentFav = -1;
	m_currentFavCount = 0;
	m_currentFavorite = QString();
	loadNextFavorite();
}
void mainWindow::loadNextFavorite()
{
	if (m_currentFav+1 == m_favorites.count())
	{ return; }
	else
	{
		m_currentFav++;
		QString tag = m_favorites.keys().at(m_currentFav);
		m_loadFavorite = QDateTime::fromString(m_favorites.value(tag).section('|', 1, 1), Qt::ISODate);
		web(tag);
	}
}
void mainWindow::web(QString tags)
{
	while (ui->layoutFavoritesResults->count() > 0)
	{ ui->layoutFavoritesResults->takeAt(0)->widget()->hide(); }
	qDeleteAll(m_pages);
	m_pages.clear();
	qDeleteAll(m_images);
	m_images.clear();
	for (int i = 0; i < m_selectedSources.count(); i++)
	{
		if (m_selectedSources.at(i))
		{
			Page *page = new Page(&m_sites, m_sites.keys().at(i), tags.split(" "), 1, m_settings->value("limit", 20).toInt(), QStringList(), this);
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(Qt::escape(page->url().toString())));
			connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
			page->load();
			m_pages.append(page);
		}
	}
}
void mainWindow::finishedLoading(Page* page)
{
	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(Qt::escape(page->url().toString())));

	if (page->imagesCount() < m_pagemax || m_pagemax == -1 )
	{ m_pagemax = page->imagesCount(); }
	QList<Image*> imgs = page->images();
	int pos = m_pages.indexOf(page);
	m_images.append(imgs);

	// Loading images
	int results = 0;
	for (int i = 0; i < imgs.count(); i++)
	{
		Image *img = imgs.at(i);
		if (img->createdAt() < m_loadFavorite)
		{ log(tr("Image #%1 ignorée. Raison : %2.").arg(i).arg("déjà vue"));; }
		else
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
			QStringList detected = img->blacklisted(blacklistedtags);
			if (!detected.isEmpty() && m_settings->value("hideblacklisted", false).toBool())
			{ log(tr("Image #%1 ignorée. Raison : %2.").arg(i).arg("\""+detected.join(", ")+"\""));; }
			else
			{
				connect(img, SIGNAL(finishedLoadingPreview(Image*)), this, SLOT(finishedLoadingPreview(Image*)));
				img->loadPreview();
				results++;
			}
		}
	}
	if (results >= m_settings->value("hidefavorites", 20).toInt())
	{ ui->widgetFavorites->hide(); }

	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	QLabel *txt = new QLabel(this);
		if (results == 0)
		{ txt->setText(tr("Aucun résultat depuis le %1.").arg(m_loadFavorite.toString(tr("dd/MM/yyyy")))); }
		else
		{ txt->setText(m_sites.key(page->site())+" - <a href=\""+page->url().toString()+"\">"+page->url().toString()+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(1).arg(page->imagesCount() != 0 ? ceil(page->imagesCount()/((float)m_settings->value("limit", 20).toInt())) : 0).arg(imgs.count()).arg(page->imagesCount() != 0 ? page->imagesCount() : 0)); }
		txt->setOpenExternalLinks(true);
	ui->layoutFavoritesResults->addWidget(txt, floor(pos/m_settings->value("columns", 1).toInt())*(fl+1), pl*(pos%m_settings->value("columns", 1).toInt()), 1, pl);
	ui->layoutFavoritesResults->setRowMinimumHeight((floor(pos/m_settings->value("columns", 1).toInt())*(fl+1)), 50);
}
void mainWindow::finishedLoadingPreview(Image *img)
{
	int position = m_images.indexOf(img);
	int page = m_pages.indexOf(img->page());
	if (img->previewImage().isNull())
	{
		log("<b>Warning:</b> one of the preview pictures (<a href='"+img->previewUrl().toString()+"'>"+img->previewUrl().toString()+"</a>) is empty.");
		return;
	}

	QString unit;
	int size = img->fileSize();
	if (size >= 2048)
	{
		size /= 1024;
		if (size >= 2048)
		{
			size /= 1024;
			unit = "Mio";
		}
		else
		{ unit = "Kio"; }
	}
	else
	{ unit = "o"; }

	QColor color;
	if (img->status() == "pending")
	{ color = QColor("#0000ff"); }
	if (img->parentId() != 0)
	{ color = QColor("#cccc00"); }
	if (img->hasChildren())
	{ color = QColor("##00ff00"); }
	QBouton *l = new QBouton(position, this, m_settings->value("resizeInsteadOfCropping", true).toBool(), color, this);
		l->setIcon(img->previewImage());
		QString t;
		QList<Tag> tgs = img->tags();
		for (int i = 0; i < tgs.count(); i++)
		{ t += " "+tgs[i].text(); }
		l->setToolTip(QString("%1%2%3%4%5%6%7%8")
			.arg(img->tags().isEmpty() ? " " : tr("<b>Tags :</b> %1<br/><br/>").arg(t.trimmed()))
			.arg(img->id() == 0 ? " " : tr("<b>ID :</b> %1<br/>").arg(img->id()))
			.arg(img->rating().isEmpty() ? " " : tr("<b>Classe :</b> %1<br/>").arg(img->rating()))
			.arg(tr("<b>Score :</b> %1<br/>").arg(img->score()))
			.arg(img->author().isEmpty() ? " " : tr("<b>Posteur :</b> %1<br/><br/>").arg(img->author()))
			.arg(img->width() == 0 || img->height() == 0 ? " " : tr("<b>Dimensions :</b> %1 x %2<br/>").arg(QString::number(img->width()), QString::number(img->height())))
			.arg(img->fileSize() == 0 ? " " : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(round(size)), unit))
			.arg(!img->createdAt().isValid() ? " " : tr("<b>Date :</b> %1").arg(img->createdAt().toString(tr("le dd/MM/yyyy à hh:mm"))))
		);
		l->setIconSize(img->previewImage().size());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), this, SLOT(batchChange(int)));
	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	ui->layoutFavoritesResults->addWidget(l, floor(float(position%m_settings->value("limit", 20).toInt())/fl)+(floor(page/m_settings->value("columns", 1).toInt())*(fl+1))+1, (page%m_settings->value("columns", 1).toInt())*pl+position%pl, 1, 1);
}
void mainWindow::webZoom(int id)
{
	Image *image = m_images.at(id);
	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
		QStringList detected = image->blacklisted(blacklistedtags);
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(this, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}
	zoomWindow *zoom = new zoomWindow(image, image->page()->site(), &m_sites);
	zoom->show();
	connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
}

void mainWindow::viewed()
{
	if (m_currentFavorite.isEmpty())
	{
		int reponse = QMessageBox::question(this, tr("Grabber - Marquer comme vu"), tr("Êtes-vous sûr de vouloir marquer tous vos favoris comme vus ?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			QStringList keys = m_favorites.keys();
			for (int i = 0; i < keys.count(); i++)
			{ setFavoriteViewed(keys.at(i)); }
		}
	}
	else
	{ setFavoriteViewed(m_currentFavorite); }
	updateFavorites();
	updateFavoritesDock();
}
void mainWindow::setFavoriteViewed(QString tag)
{
	log(tr("Marquage comme vu de %1...").arg(tag));
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(tag+"\\|([^|]+)\\|([^|]+)\r\n");
	reg.setMinimal(true);
	favs.replace(reg, tag+"|\\1|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"\r\n");
	f.open(QIODevice::WriteOnly);
		f.write(favs.toUtf8());
	f.close();
	DONE();
}
void mainWindow::favoritesBack()
{
	ui->widgetFavoritesResults->hide();
	ui->widgetFavorites->show();
	if (!m_currentFavorite.isEmpty() || m_currentFav != -1)
	{
		m_currentFavorite = "";
		m_currentFav = -1;
		ui->widgetFavorites->show();
		if (!m_replies.isEmpty() && m_currentFav < 1)
		{
			for (int i = 0; i < m_replies.count(); i++)
			{ m_replies.at(i)->abort(); }
			qDeleteAll(m_replies);
			m_replies.clear();
		}
		if (!m_webPics.isEmpty() && m_currentFav < 1)
		{
			qDeleteAll(m_webPics);
			m_webPics.clear();
			m_details.clear();
		}
		if (!m_webSites.isEmpty() && m_currentFav < 1)
		{
			for (int i = 0; i < m_webSites.count()*11; i++)
			{ ui->layoutFavoritesResults->setRowMinimumHeight(i, 0); }
			qDeleteAll(m_webSites);
			m_webSites.clear();
		}
	}
}
void mainWindow::favoriteProperties(int id)
{
	QString tag = m_favorites.keys().at(id);
	QStringList xp = m_favorites.value(tag).split("|");
	int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
	QDateTime lastviewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);
	favoriteWindow *fwin = new favoriteWindow(tag, note, lastviewed, this);
	fwin->show();
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
		switchTranslator(m_translator, "languages/"+m_currLang);
		if (!shutup)
		{
			log(tr("Traduction des textes en %1...").arg(m_currLang));
				int _comboOrderfavorites = ui->comboOrderfavorites->currentIndex();
				int _comboOrderasc = ui->comboOrderasc->currentIndex();
				/*#if defined(Q_OS_WIN)
					ui->retranslateUi(this);
				#endif*/
				ui->comboOrderfavorites->setCurrentIndex(_comboOrderfavorites);
				ui->comboOrderasc->setCurrentIndex(_comboOrderasc);
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
		m_settings->beginGroup("Favorites");
			QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
			m_settings->setValue("order", assoc[ui->comboOrderfavorites->currentIndex()]);
			m_settings->setValue("reverse", bool(ui->comboOrderasc->currentIndex() == 1));
		m_settings->endGroup();
		for (int i = 0; i < m_tabs.size(); i++)
		{ m_tabs.at(i)->deleteLater(); }
		m_settings->setValue("crashed", false);
		m_settings->sync();
	DONE();
	e->accept();
	qApp->quit();
}


void mainWindow::options()
{
	log(tr("Ouverture de la fenêtre des options..."));
	optionsWindow *options = new optionsWindow(this);
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

	for (int i = 0; i < m_batchs.size(); i++)
	{ m_getAllRemaining.append(new Image(m_batchs.at(i), m_timezonedecay, new Page(&m_sites, m_batchs.at(i).value("site"), m_batchs.at(i).value("tags").split(" "), 1, 1, QStringList(), this))); }

	for (int i = 0; i < m_progressBars.size(); i++)
	{
		m_progressBars.at(i)->setMaximum(100);
		m_progressBars.at(i)->setValue(0);
	}
	m_allow = false;
	for (int i = 0; i < ui->tableBatchGroups->rowCount(); i++)
	{ ui->tableBatchGroups->item(i, 0)->setIcon(QIcon(":/images/colors/black.png")); }
	m_allow = true;
	if (!m_settings->value("Exec/Group/init").toString().isEmpty())
	{
		log(tr("Execution de la commande d'initialisation' \"%1\"").arg(m_settings->value("Exec/Group/init").toString()));
		m_process = new QProcess(this);
		m_process->start(m_settings->value("Exec/Group/init").toString());
		if (!m_process->waitForStarted(10000))
		{ log(tr("Erreur lors de la commande d'initialisation : %1.").arg("timed out"), Error); }
	}
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	int count = selected.size();
	m_batchDownloading.clear();
	QSet<int> todownload = QSet<int>();
	for (int i = 0; i < count; i++)
	{ todownload.insert(selected.at(i)->row()); }
	if (all || !todownload.isEmpty())
	{
		for (int i = 0; i < m_groupBatchs.count(); i++)
		{
			if (all || todownload.contains(i))
			{
				m_batchDownloading.insert(i);
				QString site = m_groupBatchs.at(i).at(5);
				int pp = m_groupBatchs.at(i).at(2).toInt();
				QString text = " "+m_groupBatchs.at(i).at(0)+" ";
				text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
					.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
					.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
					.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
					.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
					.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
				QStringList tags = text.split(" ", QString::SkipEmptyParts);
				tags.removeDuplicates();
				for (int r = 0; r < ceil(m_groupBatchs.at(i).at(3).toDouble()/pp); r++)
				{
					if (!m_sites.keys().contains(site))
					{ log(tr("<b>Attention :</b> %1").arg(tr("site \"%1\" not found.").arg(site))); }
					else
					{
						Page *page = new Page(&m_sites, site, tags, m_groupBatchs.at(i).at(1).toInt()+r, pp, QStringList(), this);
						log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(Qt::escape(page->url().toString())));
						connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
						page->load();
						m_groupBatchs[i][8] = page->url().toString();
						m_getAllPages.append(page);
					}
				}
			}
		}
	}
	if (m_getAllPages.isEmpty())
	{ getAllImages(); }
	m_progressdialog->setText(tr("Téléchargement des pages, veuillez patienter..."));
		connect(m_progressdialog, SIGNAL(rejected()), this, SLOT(getAllCancel()));
		m_progressdialog->setValue(0);
		m_progressdialog->setImagesCount(0);
		m_progressdialog->setImages(0);
		m_progressdialog->show();
		logShow();
}
void mainWindow::getAllFinishedLoading(Page* p)
{
	log(tr("Page reçue <a href=\"%1\">%1</a>").arg(Qt::escape(p->url().toString())));
	int n = 0;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(8) == p->url().toString())
		{ n = i; break; }
	}

	QList<Image*> imgs = QList<Image*>(), ims = p->images();
	QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
	for (int i = 0; i < ims.size(); i++)
	{
		if (ims[i]->blacklisted(blacklistedtags).isEmpty())
		{ imgs.append(ims[i]); }
	}

	m_progressBars[n]->setMaximum(imgs.size());
	while (imgs.size() > m_groupBatchs.at(n).at(2).toInt())
	{ imgs.removeAt(m_groupBatchs.at(n).at(2).toInt()); }
	m_getAllRemaining.append(imgs);
	m_getAllCount++;

	if (m_getAllCount == m_getAllPages.count())
	{
		if (m_getAllRemaining.isEmpty())
		{
			if (ims.isEmpty())
			{ error(this, tr("<b>Attention :</b> %1").arg(tr("rien n'a été reçu depuis %1. Raisons possibles : tag incorrect, page trop éloignée.").arg(p->site().value("Url")))); }
			else
			{ error(this, tr("<b>Attention :</b> %1").arg(tr("toutes les images provenant de %1 ont été ignorées.").arg(p->site().value("Url")))); }
			return;
		}
		getAllImages();
	}
}
void mainWindow::getAllImages()
{
	log(tr("Toutes les urls des images ont été reçues."));

	int count = 0;
	for (int i = 0; i < m_getAllRemaining.count(); i++)
	{
		count += m_getAllRemaining.at(i)->value();
		int n = 0;
		for (int r = 0; r < m_groupBatchs.count(); r++)
		{
			if (m_groupBatchs.at(r).at(8) == m_getAllRemaining.at(i)->page()->url().toString())
			{ n = r + 1; break; }
		}
		m_progressdialog->addImage(m_getAllRemaining.at(i)->url(), n, m_getAllRemaining.at(i)->fileSize());
	}
	m_progressdialog->setMaximum(count);
	m_progressdialog->setImagesCount(m_getAllRemaining.count());
	m_progressdialog->setText(tr("Téléchargement des images en cours..."));

	m_must_get_tags = false;
	QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "general";
	for (int f = 0; f < m_groupBatchs.size(); f++)
	{
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (m_groupBatchs[f][6].contains("%"+forbidden.at(i)+"%"))
			{ m_must_get_tags = true; }
		}
	}
	if (!m_batchs.isEmpty())
	{
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (m_settings->value("Save/filename").toString().contains("%"+forbidden.at(i)+"%"))
			{ m_must_get_tags = true; }
		}
	}
	if (m_must_get_tags)
	{ log("Downloading detailed tags first."); }
	else
	{ log("Downloading pictures directly."); }

	for (int i = 0; i < qMax(1, qMin(m_settings->value("Save/simultaneous").toInt(), 10)); i++)
	{ _getAll(); }
}
void mainWindow::_getAll()
{
	if (m_progressdialog->cancelled())
	{ return; }

	if (m_getAllRemaining.count() > 0)
	{
		m_getAllDownloading.prepend(m_getAllRemaining.takeFirst());

		if (m_must_get_tags)
		{
			m_getAllDownloading.at(0)->loadTags();
			connect(m_getAllDownloading.at(0), SIGNAL(finishedLoadingTags(Image*)), this, SLOT(getAllPerformTags(Image*)));
		}
		else
		{
			Image *img = m_getAllDownloading.at(0);

			// Row
			int site_id = -1;
			for (int i = 0; i < m_groupBatchs.count(); i++)
			{
				if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
				{ site_id = i; break; }
			}

			// Path
			QString path = m_settings->value("Save/filename").toString();
			QString pth = m_settings->value("Save/path").toString();
			if (site_id >= 0)
			{
				ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/blue.png"));
				path = m_groupBatchs[site_id][6];
				pth = m_groupBatchs[site_id][7];
			}

			QFile f((img->folder().isEmpty() ? pth : img->folder())+"/"+img->path(path));
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
					m_getAllDetails.clear();
					_getAll();
				}
				else
				{
					log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a>").arg(img->fileUrl().toString()));
					m_progressdialog->loadingImage(img->url());
					m_downloadTime.insert(img->url(), new QTime);
					m_downloadTime[img->url()]->start();
					connect(img, SIGNAL(finishedImage(Image*)), this, SLOT(getAllPerformImage(Image*)));
					connect(img, SIGNAL(downloadProgressImage(Image*,qint64,qint64)), this, SLOT(getAllProgress(Image*,qint64,qint64)));
					m_getAllDownloadingSpeeds.insert(img->url(), 0);
					img->loadImage();
				}
			}
			else
			{
				m_progressdialog->setValue(m_progressdialog->value()+img->value());
				m_progressdialog->setImages(m_progressdialog->images()+1);
				m_getAllExists++;
				log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
				m_progressdialog->errorImage(img->url());
				if (site_id >= 0)
				{
					m_progressBars[site_id]->setValue(m_progressBars[site_id]->value()+1);
					if (m_progressBars[site_id]->value() >= m_progressBars[site_id]->maximum())
					{ ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/green.png")); }
				}
				m_getAllDetails.clear();
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
			case 1:	m_progressdialog->close();	break;
			case 2:	openTray();					break;
			case 3:	shutDown();					break;
		}
		if (m_progressdialog->endRemove())
		{
			int rem = 0;
			foreach (int i, m_batchDownloading)
			{
				m_groupBatchs.removeAt(i-rem);
				m_progressBars.removeAt(i-rem);
				ui->tableBatchGroups->removeRow(i-rem);
				rem++;
			}
		}
		activateWindow();
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
			reponse = QMessageBox::question(this, tr("Récupération des images"), tr("Des erreurs sont survenues pendant le téléchargement des images. Voulez vous relancer le téléchargement de celles-ci ? (%1/%2)").arg(m_getAllErrors).arg(m_progressdialog->maximum()), QMessageBox::Yes | QMessageBox::No);
			if (reponse == QMessageBox::Yes)
			{
				m_progressdialog->clear();
				qDeleteAll(m_getAllRemaining);
				m_getAllRemaining.clear();
				m_getAllRemaining = m_getAllFailed;
				m_getAllDownloaded = 0;
				m_getAllExists = 0;
				m_getAllIgnored = 0;
				m_getAll404s = 0;
				m_getAllErrors = 0;
				m_getAllCount = 0;
				getAllImages();
			}
		}
		if (reponse != QMessageBox::Yes)
		{
			if (!m_settings->value("Exec/Group/init").toString().isEmpty())
			{
				m_process->closeWriteChannel();
				m_process->waitForFinished(1000);
				m_process->close();
			}
			m_getAll = false;
			ui->widgetDownloadButtons->setDisabled(false);
			log(tr("Téléchargement groupé terminé"));
		}
	}
}
void mainWindow::getAllProgress(Image *img, qint64 bytesReceived, qint64 bytesTotal)
{
	float speed = (bytesReceived * 1000) / m_downloadTime[img->url()]->elapsed();
	m_progressdialog->speedImage(img->url(), speed);
	m_progressdialog->statusImage(img->url(), (bytesReceived * 100) / bytesTotal);
}
void mainWindow::getAllPerformTags(Image* img)
{
	if (m_progressdialog->cancelled())
	{ return; }

	log(tr("Tags reçus"));

	bool under = m_settings->value("Save/remplaceblanksbyunderscores", false).toBool();
	for (int i = 0; i < img->tags().count(); i++)
	{
		Tag tag = img->tags().at(i);
		QString normalized = tag.text().replace(" ", "_"), original = normalized;
		m_getAllDetails["alls_original"].append(original);
		normalized = normalized.replace("\\", " ").replace("/", " ").replace(":", " ").replace("|", " ").replace("*", " ").replace("?", " ").replace("\"", " ").replace("<", " ").replace(">", " ").trimmed();
		if (!under)
		{ normalized.replace('_', ' '); }
		if		(tag.type() == "character")	{ m_getAllDetails["characters"].append(normalized); }
		else if (tag.type() == "copyright")	{ m_getAllDetails["copyrights"].append(normalized); }
		else if (tag.type() == "artist")		{ m_getAllDetails["artists"].append(normalized);	}
		else if (tag.type() == "model")		{ m_getAllDetails["models"].append(normalized);		}
		else									{ m_getAllDetails["generals"].append(normalized);	}
		m_getAllDetails["alls"].append(normalized);
	}

	// Row
	int site_id = -1;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
		{ site_id = i; break; }
	}

	// Getting path
	QString path = m_settings->value("Save/filename").toString();
	QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
	if (site_id >= 0)
	{
		path = m_groupBatchs[site_id][6];
		p = m_groupBatchs[site_id][7];
	}
	path = img->path(path);

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
			m_getAllDetails.clear();
			m_getAllDownloadingSpeeds.remove(img->url());
			m_getAllDownloading.removeAt(m_getAllId);
			_getAll();
		}
		else
		{
			log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a>").arg(img->fileUrl().toString()));
			m_progressdialog->loadingImage(img->url());
			m_downloadTime.insert(img->url(), new QTime);
			m_downloadTime[img->url()]->start();
			connect(img, SIGNAL(finishedImage(Image*)), this, SLOT(getAllPerformImage(Image*)));
			connect(img, SIGNAL(downloadProgressImage(Image*,qint64,qint64)), this, SLOT(getAllProgress(Image*,qint64,qint64)));
			m_getAllDownloadingSpeeds.insert(img->url(), 0);
			img->loadImage();
		}
	}
	else
	{
		m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_progressdialog->images()+1);
		m_getAllExists++;
		log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
		m_getAllDetails.clear();
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
void mainWindow::getAllPerformImage(Image* img)
{
	if (m_progressdialog->cancelled())
	{ return; }

	QNetworkReply* reply = img->imageReply();

	if (reply->error() == QNetworkReply::OperationCanceledError)
	{ return; }
	log(tr("Image reçue depuis <a href=\"%1\">%1</a>").arg(reply->url().toString()));

	// Row
	int site_id = -1;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
		{ site_id = i; break; }
	}

	int m_getAllId = -1;
	for (int i = 0; i < m_getAllDownloading.count(); i++)
	{
		if (m_getAllDownloading[i]->url() == img->url())
		{ m_getAllId = i; }
	}

	int errors = m_getAllErrors, e404s = m_getAll404s;
	if (reply->error() == QNetworkReply::NoError)
	{
		// Path
		QString path = m_settings->value("Save/filename").toString();
		QString p = img->folder().isEmpty() ? m_settings->value("Save/path").toString() : img->folder();
		if (site_id >= 0)
		{
			ui->tableBatchGroups->item(site_id, 0)->setIcon(QIcon(":/images/colors/blue.png"));
			path = m_groupBatchs[site_id][6];
			p = m_groupBatchs[site_id][7];
		}
		path = img->path(path);
		path.replace("%n%", QString::number(m_getAllDownloaded + m_getAllExists + m_getAllIgnored + m_getAllErrors));

		if (path.left(1) == QDir::toNativeSeparators("/"))	{ path = path.right(path.length()-1);	}
		if (p.right(1) == QDir::toNativeSeparators("/"))	{ p = p.left(p.length()-1);				}
		QString fp = QDir::toNativeSeparators(p+"/"+path);

		QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2));
		if (!path_to_file.exists())
		{
			QDir dir(p);
			if (!dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
			{
				log(tr("Impossible de créer le dossier de destination: %1.").arg(p+"/"+path.section('/', 0, -2)), Error);
				m_getAllErrors++;
			}
		}

		QByteArray data = reply->readAll();
		if (!data.isEmpty())
		{
			QFile f(fp);
			if (f.open(QIODevice::WriteOnly))
			{ f.write(data); }
			else
			{
				log(tr("Impossible d'ouvrir le fichier de destination: %1.").arg(fp), Error);
				m_getAllErrors++;
			}
			f.close();
		}
		else
		{
			log(tr("Rien n'a été reçu pour l'image: <a href=\"%1\">%1</a>.").arg(Qt::escape(img->url())), Error);
			m_getAllErrors++;
		}

		QMap<QString,int> types;
		types["general"] = 0;
		types["artist"] = 1;
		types["general"] = 2;
		types["copyright"] = 3;
		types["character"] = 4;
		types["model"] = 5;
		types["photo_set"] = 6;
		for (int i = 0; i < img->tags().count(); i++)
		{
			Tag tag = img->tags().at(i);
			QString original = tag.text().replace(" ", "_");
			if (!m_settings->value("Exec/tag").toString().isEmpty())
			{
				QString exec = m_settings->value("Exec/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag.type())
				.replace("%number%", QString::number(types[tag.type()]));
				log(tr("Execution seule de \"%1\"").arg(exec));
				QProcess::execute(exec);
			}
			if (!m_settings->value("Exec/Group/tag").toString().isEmpty())
			{
				QString exec = m_settings->value("Exec/Group/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag.type())
				.replace("%number%", QString::number(types[tag.type()]));
				log(tr("Execution groupée de \"%1\"").arg(exec));
				m_process->write(exec.toUtf8());
			}
		}
		if (!m_settings->value("Exec/image").toString().isEmpty())
		{
			QString exec = img->path(m_settings->value("Exec/image").toString());
			exec.replace("%path%", fp);
			exec.replace(" \\C ", " /C ");
			log(tr("Execution seule de \"%1\"").arg(exec));
			QProcess::execute(exec);
		}
		if (!m_settings->value("Exec/Group/image").toString().isEmpty())
		{
			QString exec = img->path(m_settings->value("Exec/Group/image").toString());
			exec.replace("%path%", fp);
			log(tr("Execution groupée de \"%1\"").arg(exec));
			m_process->write(exec.toUtf8());
		}
	}
	else if (reply->error() == QNetworkReply::ContentNotFoundError)
	{ m_getAll404s++; }
	else
	{ m_getAllErrors++; }

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
	m_getAllDetails.clear();
	m_getAllDownloadingSpeeds.remove(img->url());
	m_getAllDownloading.removeAt(m_getAllId);

	_getAll();
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
	QString links = "[IGL 1]\r\n";
	for (int i = 0; i < m_groupBatchs.size(); i++)
	{ links += m_groupBatchs[i].join("¤")+"¤"+QString::number(m_progressBars[i]->value())+"/"+QString::number(m_progressBars[i]->maximum())+"\r\n"; }

	QFile *f = new QFile(filename, this);
	if (f->open(QFile::WriteOnly))
	{
		f->write(links.trimmed().toUtf8());
		f->close();
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
	QFile *f = new QFile(filename, this);
	if (f->open(QFile::ReadOnly))
	{
		QString links = f->readAll();
		f->close();

		QStringList det = links.split("\r\n");
		for (int i = 1; i < det.size(); i++)
		{
			QString link = det[i];
			m_allow = false;
			QStringList infos = link.split("¤");
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
		saveSettings();
	}
}
void mainWindow::saveSettings()
{
	m_settings->setValue("Save/path", ui->lineFolder->text());
	m_settings->setValue("Save/filename", ui->lineFilename->text());
	ui->labelFilename->setText(validateFilename(ui->lineFilename->text()));
	m_settings->sync();
}
