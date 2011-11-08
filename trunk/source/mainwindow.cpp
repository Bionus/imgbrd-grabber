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
#include "functions.h"
#include "json.h"
#include <QtXml>

#define VERSION	"2.4.0"
#define DONE()	logUpdate(tr(" Fait"))

extern QMap<QDateTime,QString> _log;



mainWindow::mainWindow(QString program, QStringList tags, QStringMap params) : ui(new Ui::mainWindow), m_currentFav(-1), m_params(params), m_program(program), m_tags(tags), m_loaded(false)
{
}
void mainWindow::init()
{
	m_settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	loadLanguage(m_settings->value("language", "English").toString(), true);
	ui->setupUi(this);
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
			{ log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)))); }
			else
			{
				QDomElement docElem = doc.documentElement();
				QStringMap details = domToMap(docElem);
				QStringList defaults = QStringList() << "xml" << "json" << "regex";
				QString source, curr;
				for (int s = 0; s < 3; s++)
				{
					QString t = m_settings->value("source_"+QString::number(s+1), defaults.at(s)).toString();
					t[0] = t[0].toUpper();
					if (details.contains("Urls/"+(t == "Regex" ? "Html" : t)+"/Tags"))
					{
						source = t;
						break;
					}
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
							if (line.contains(':'))
							{
								curr = line.section(':', 1).toLower();
								curr[0] = curr[0].toUpper();
								line = line.section(':', 0, 0);
							}
							else
							{ curr = source; }
							stes[line] = details;
							stes[line]["Urls/Selected/Tags"] = "http://"+line+stes[line]["Urls/"+(curr == "Regex" ? "Html" : curr)+"/Tags"];
							stes[line]["Url"] = line;
							stes[line]["Urls/Html/Post"] = "http://"+line+stes[line]["Urls/Html/Post"];
							if (stes[line].contains("Urls/Html/Tags"))
							{ stes[line]["Urls/Html/Tags"] = "http://"+line+stes[line]["Urls/Html/Tags"]; }
							stes[line]["Selected"] = curr.toLower();
						}
					}
					else
					{ log(tr("<b>Erreur :</b> %1").arg(tr("fichier sites.txt du modèle %1 introuvable.").arg(dir.at(i)))); }
					f.close();
				}
				else
				{ log(tr("<b>Erreur :</b> %1").arg(tr("aucune source valide trouvée dans le fichier model.xml de %1.").arg(dir.at(i)))); }
			}
		}
		file.close();
	}
	m_sites = stes;

	QPushButton *add = new QPushButton(QIcon(":/images/add.png"), "");
		add->setFlat(true);
		add->resize(QSize(12,12));
		connect(add, SIGNAL(clicked()), this, SLOT(addTab()));
		ui->tabWidget->setCornerWidget(add);
	addTab();

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

	if (m_settings->value("firstload", true).toBool())
	{
		QSettings cfg(QSettings::IniFormat, QSettings::UserScope, "Mozilla", "Firefox");
		QString path = QFileInfo(cfg.fileName()).absolutePath()+"/Firefox";
		QSettings profiles(path+"/profiles.ini", QSettings::IniFormat);
		if (QFile::exists(path+"/"+profiles.value("Profile0/Path").toString()+"/extensions/danbooru_downloader@cuberocks.net.xpi"))
		{
			int reponse = QMessageBox::question(this, tr("Danbooru Downloader"), tr("L'extension pour Mozilla Firefox \"Danbooru Downloader\" a été détéctée sur votre système. Souhaitez-vous en importer les préférences ?"), QMessageBox::Yes | QMessageBox::No);
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
	setWindowState(Qt::WindowStates(m_settings->value("state", 0).toInt()));
	if (!isMaximized())
	{
		resize(m_settings->value("size", QSize(800, 600)).toSize());
		move(m_settings->value("pos", QPoint(200, 200)).toPoint());
	}

	// Selected ones
	QString sel = '1'+QString().fill('0',stes.count()-1);
	QString sav = m_settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selectedSources.append(sav.at(i) == '1' ? true : false);
	}

	// Console usage
	if (this->m_params.keys().contains("batch"))
	{
		batchAddGroup(QStringList() << m_tags.join(" ") << m_params.value("page", "1") << m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("ignore", m_settings->value("downloadblacklist", "false").toString()) << this->m_params.value("booru", m_sites.keys().at(0)) << this->m_params.value("filename", m_settings->value("Save/filename").toString()) << this->m_params.value("path", m_settings->value("Save/path").toString()) << "");
		ui->tabWidget->setCurrentIndex(2);
		if (!m_params.keys().contains("dontstart"))
		{ getAll(); }
	}
	else if (!m_tags.isEmpty() || m_settings->value("loadatstart", false).toBool())
	{ m_tabs[0]->setTags(this->m_tags.join(" ")); }

	QHeaderView *headerView = ui->tableBatchGroups->horizontalHeader();
	headerView->setResizeMode(QHeaderView::Interactive);
	headerView = ui->tableBatchUniques->horizontalHeader();
	headerView->setResizeMode(QHeaderView::Interactive);

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

void mainWindow::addTab(QString tag)
{
	searchTab *w = new searchTab(&m_sites, &m_favorites, &m_serverDate, this);
	connect(w, SIGNAL(batchAddGroup(QStringList)), this, SLOT(batchAddGroup(QStringList)));
	connect(w, SIGNAL(titleChanged(searchTab*)), this, SLOT(updateTabTitle(searchTab*)));
	int index = ui->tabWidget->insertTab(ui->tabWidget->currentIndex()+(!m_tabs.isEmpty()), w, tr("Nouvel onglet"));
	ui->tabWidget->setCurrentIndex(index);
	m_tabs.append(w);
	QPushButton *closeTab = new QPushButton(QIcon(":/images/close.png"), "");
		closeTab->setFlat(true);
		closeTab->resize(QSize(8,8));
		connect(closeTab, SIGNAL(clicked()), w, SLOT(deleteLater()));
		ui->tabWidget->findChild<QTabBar*>()->setTabButton(index, QTabBar::RightSide, closeTab);
	if (!tag.isEmpty())
	{ w->setTags(tag); }
}
void mainWindow::addTabFavorite(int id)
{
	QString tag = m_favorites.keys().at(id);
	addTab(tag);
}
void mainWindow::updateTabTitle(searchTab *tab)
{
	ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), tab->windowTitle());
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
	for (int t = 0; t < values.count(); t++)
	{
		item = new QTableWidgetItem;
			item->setText(values.at(t));
		int r = t+1;
		if (r == 1) { r = 0; }
		else if (r == 6) { r = 1; }
		else if (r == 7) { r = 5; }
		else if (r == 8) { r = 6; }
		else if (r == 5) { r = 7; }
		ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, r, item);
		/*QProgressBar *prog = new QProgressBar();
		prog->setTextVisible(false);
		m_progressBars.append(prog);
		ui->tableBatchGroups->setCellWidget(ui->tableBatchGroups->rowCount()-1, 8, prog);*/
	}
	m_allow = true;
}
void mainWindow::batchAddUnique(QStringMap values)
{
	if (!m_batchs.removeOne(values))
	{
		m_batchs.append(values);
		QStringList types = QStringList() << "id" << "md5" <<  "rating" << "tags" << "file_url" << "site";
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
	if (m_allow)
	{
		int r = x;
		if (0 == 1) { r = 1; }
		else if (r == 1) { r = 6; }
		else if (r == 5) { r = 7; }
		else if (r == 6) { r = 8; }
		else if (r == 7) { r = 5; }
		r--;
		m_groupBatchs[y][r] = ui->tableBatchGroups->item(y,x)->text();
	}
}
void mainWindow::addGroup()
{
	AddGroupWindow *wAddGroup = new AddGroupWindow(m_sites.keys(), m_favorites.keys(), this);
	wAddGroup->show();
}
void mainWindow::addUnique()
{
	AddUniqueWindow *wAddUnique = new AddUniqueWindow(m_sites, this);
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
void mainWindow::updateFavorites()
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
			QBouton *image = new QBouton(favorites[i]["id"].toInt(), this, m_settings->value("resizeInsteadOfCropping", true).toBool());
				image->setIcon(img);
				image->setIconSize(img.size());
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
				connect(image, SIGNAL(middleClick(int)), this, SLOT(addTabFavorite(int)));
			QAffiche *caption = new QAffiche(favorites[i]["id"].toInt());
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
	m_pages.clear();
	m_images.clear();
	for (int i = 0; i < m_selectedSources.count(); i++)
	{
		if (m_selectedSources.at(i))
		{
			Page *page = new Page(&m_sites, m_sites.keys().at(i), tags.split(" "), 1, m_settings->value("limit", 20).toInt(), QStringList(), this);
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString()));
			connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
			page->load();
			m_pages.append(page);
		}
	}
}
void mainWindow::finishedLoading(Page* page)
{
	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(page->url().toString()));

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
			QStringList detected;
			if (!m_settings->value("blacklistedtags").toString().isEmpty())
			{
				QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
				for (int b = 0; b < blacklistedtags.size(); b++)
				{
					for (int t = 0; t < img->tags().count(); t++)
					{
						if (img->tags().at(t)->text().toLower() == blacklistedtags.at(b).toLower())
						{ detected.append(blacklistedtags.at(b)); }
					}
				}
			}
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
	QLabel *txt = new QLabel();
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
	QPixmap preview = img->previewImage();
	if (preview.isNull())
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
			unit = "mo";
		}
		else
		{ unit = "ko"; }
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
	QBouton *l = new QBouton(position, this, m_settings->value("resizeInsteadOfCropping", true).toBool(), color);
		l->setIcon(preview);
		QString t;
		for (int i = 0; i < img->tags().count(); i++)
		{ t += " "+img->tags().at(i)->text(); }
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
		l->setIconSize(preview.size());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), this, SLOT(batchChange(int)));
	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	ui->layoutFavoritesResults->addWidget(l, floor(float(position%m_settings->value("limit", 20).toInt())/fl)+(floor(page/m_settings->value("columns", 1).toInt())*(fl+1))+1, (page%m_settings->value("columns", 1).toInt())*pl+position%pl, 1, 1);
}
void mainWindow::webZoom(int id)
{
	QStringList detected;
	Image *image = m_images.at(id);
	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
		for (int i = 0; i < blacklistedtags.size(); i++)
		{
			for (int t = 0; t < image->tags().count(); t++)
			{
				if (image->tags().at(t)->text().toLower() == blacklistedtags.at(i).toLower())
				{ detected.append(blacklistedtags.at(i)); }
			}
		}
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(this, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}
	zoomWindow *zoom = new zoomWindow(image, image->page()->site());
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
		f.write(favs.toAscii());
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
			m_replies.clear();
		}
		if (!m_webPics.isEmpty() && m_currentFav < 1)
		{
			for (int i = 0; i < m_webPics.count(); i++)
			{ delete m_webPics.at(i); }
			m_webPics.clear();
			m_details.clear();
		}
		if (!m_webSites.isEmpty() && m_currentFav < 1)
		{
			for (int i = 0; i < m_webSites.count(); i++)
			{ delete m_webSites.at(i); }
			for (int i = 0; i < m_webSites.count()*11; i++)
			{ ui->layoutFavoritesResults->setRowMinimumHeight(i, 0); }
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
				#if defined(Q_OS_WIN)
					ui->retranslateUi(this);
				#endif
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
		QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
		settings.setValue("state", int(windowState()));
		settings.setValue("size", size());
		settings.setValue("pos", pos());
		QStringList sizes = QStringList();
		for (int i = 0; i < ui->tableBatchGroups->columnCount(); i++)
		{ sizes.append(QString::number(ui->tableBatchGroups->horizontalHeader()->sectionSize(i))); }
		settings.setValue("batch", sizes.join(","));
		settings.beginGroup("Favorites");
			QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
			settings.setValue("order", assoc[ui->comboOrderfavorites->currentIndex()]);
			settings.setValue("reverse", bool(ui->comboOrderasc->currentIndex() == 1));
		settings.endGroup();
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
	m_settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	m_tabs[0]->optionsChanged(m_settings);
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
	qDebug() << all;
	if (m_settings->value("Save/path").toString().isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de dossier de sauvegarde !")); return; }
	else if (m_settings->value("Save/filename").toString().isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de format de sauvegarde !")); return; }
	log(tr("Téléchargement groupé commencé."));
	m_getAllId = 0;
	m_getAllDownloaded = 0;
	m_getAllIgnored = 0;
	m_getAllExists = 0;
	m_getAllErrors = 0;
	m_getAllCount = 0;
	m_getAllPageCount = 0;
	m_getAllBeforeId = -1;
	m_getAllImages.clear();
	m_getAllPages.clear();
	if (!m_settings->value("Exec/Group/init").toString().isEmpty())
	{
		log(tr("Execution de la commande d'initialisation' \"%1\"").arg(m_settings->value("Exec/Group/init").toString()));
		m_process = new QProcess;
		m_process->start(m_settings->value("Exec/Group/init").toString());
		if (!m_process->waitForStarted(10000))
		{ log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de la commande d'initialisation : %1.").arg("timed out"))); }
	}
	QList<QTableWidgetItem *> selected = ui->tableBatchGroups->selectedItems();
	int count = selected.size();
	QSet<int> todownload = QSet<int>();
	for (int i = 0; i < count; i++)
	{ todownload.insert(selected.at(i)->row()); }
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (all || todownload.contains(i))
		{
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
					Page *page = new Page(&m_sites, site, tags, m_groupBatchs.at(i).at(1).toInt()+r, pp);
					log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString()));
					connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAllFinishedLoading(Page*)));
					page->load();
					m_groupBatchs[i][8] = page->url().toString();
					m_getAllPages.append(page);
				}
			}
		}
	}
	m_progressdialog->setText(tr("Téléchargement des pages, veuillez patienter..."));
		connect(m_progressdialog, SIGNAL(rejected()), this, SLOT(getAllCancel()));
		m_progressdialog->setValue(0);
		m_progressdialog->show();
		logShow();
}
void mainWindow::getAllFinishedLoading(Page* p)
{
	log(tr("Page reçue <a href=\"%1\">%1</a>").arg(p->url().toString()));
	int n = 0;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(8) == p->url().toString())
		{ n = i; break; }
	}
	QList<Image*> imgs = p->images();
	while (imgs.size() > m_groupBatchs.at(n).at(2).toInt())
	{ imgs.removeAt(m_groupBatchs.at(n).at(2).toInt()); }
	m_getAllImages.append(imgs);
	m_getAllCount++;
	if (m_getAllCount == m_getAllPages.count())
	{
		if (m_getAllImages.isEmpty())
		{
			error(this, tr("<b>Attention :</b> %1").arg(tr("rien n'a été reçu depuis %1. Raisons possibles : tag incorrect, page trop éloignée.").arg(p->site().value("Url"))));
			return;
		}
		int count = 0;
		for (int i = 0; i < m_getAllImages.count(); i++)
		{
			count += m_getAllImages.at(i)->value();
			m_progressdialog->addImage(m_getAllImages.at(i)->url());
		}
		log(tr("Toutes les urls des images ont été reçues."));
		m_progressdialog->setMaximum(count);
		m_progressdialog->setImagesCount(m_getAllImages.count());
		m_progressdialog->setText(tr("Téléchargement des images en cours..."));
		QString fn = m_groupBatchs[n][6];
		QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "general" << "model|artist";
		m_must_get_tags = false;
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (fn.contains("%"+forbidden.at(i)+"%"))
			{ m_must_get_tags = true; }
		}
		if (m_must_get_tags)
		{ log("Downloading detailed tags first."); }
		else
		{ log("Downloading pictures directly."); }
		_getAll();
	}
}
void mainWindow::_getAll()
{
	if (m_getAllId < m_getAllImages.count())
	{
		if (m_must_get_tags)
		{
			m_getAllImages.at(m_getAllId)->loadTags();
			connect(m_getAllImages.at(m_getAllId), SIGNAL(finishedLoadingTags(Image*)), this, SLOT(getAllPerformTags(Image*)));
		}
		else
		{
			Image *img = m_getAllImages.at(m_getAllId);

			// Row
			int site_id = 0;
			for (int i = 0; i < m_groupBatchs.count(); i++)
			{
				if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
				{ site_id = i; break; }
			}

			QString u(img->fileUrl().toString());
			QString path = m_groupBatchs[site_id][6];
			path.replace("%filename%", u.section('/', -1).section('.', 0, -2))
			.replace("%rating%", img->rating())
			.replace("%md5%", img->md5())
			.replace("%website%", img->site())
			.replace("%id%", QString::number(img->id()))
			.replace("%ext%", u.section('.', -1))
			.replace("%search%", m_groupBatchs[site_id][0]);
			QStringList search = m_groupBatchs[site_id][0].split(' ');
			int i = 1;
			while (path.contains("%search_"+QString::number(i)+"%"))
			{
				path.replace("%search_"+QString::number(i)+"%", (search.size() >= i ? search[i-1] : ""));
				i++;
			}
			QString pth = m_groupBatchs[site_id][7];
			pth.replace("\\", "/");
			if (path.left(1) == "/")	{ path = path.right(path.length()-1);	}
			if (pth.right(1) == "/")	{ pth = pth.left(pth.length()-1);		}
			QString all;
			for (int i = 0; i < img->tags().count(); i++)
			{ all += " "+img->tags().at(i)->text(); }
			all = all.trimmed();
			all.replace(" ", m_settings->value("separator", " ").toString());
			path.replace("%all%", all.left(263-pth.length()-path.length()));
			QFile f(pth+"/"+path);
			if (!f.exists())
			{
				bool detected = false;
				QStringList tags = search;
				QList<QChar> modifiers = QList<QChar>() << '~';
				for (int r = 0; r < tags.size(); r++)
				{
					if (modifiers.contains(tags[r][0]))
					{ tags[r] = tags[r].right(tags[r].size()-1); }
				}
				if (!m_settings->value("blacklistedtags").toString().isEmpty())
				{
					QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
					for (int t = 0; t < img->tags().count(); t++)
					{
						if (blacklistedtags.contains(img->tags().at(t)->text(), Qt::CaseInsensitive) && !tags.contains(img->tags().at(t)->text(), Qt::CaseInsensitive))
						{
							detected = true;
							log(tr("Certains tags de l'image sont blacklistés."));
						}
					}
				}
				if (detected && m_groupBatchs[site_id][4] == "false")
				{
					m_getAllId++;
					m_progressdialog->setValue(m_progressdialog->value()+img->value());
					m_progressdialog->setImages(m_getAllId);
					m_getAllIgnored++;
					log(tr("Image ignorée."));
					m_progressdialog->loadedImage(img->url());
					m_getAllDetails.clear();
					_getAll();
				}
				else
				{
					QUrl rl(u);
					QNetworkAccessManager *m = new QNetworkAccessManager(this);
					QNetworkRequest request(rl);
						request.setRawHeader("Referer", u.toAscii());
					m_progressdialog->loadingImage(img->url());
					m_getAllRequest = m->get(request);
					connect(m_getAllRequest, SIGNAL(finished()), this, SLOT(getAllPerformImage()));
					m_getAllRequestExists = true;
					m_downloadTime = new QTime();
					m_downloadTime->start();
					connect(m_getAllRequest, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(getAllProgress(qint64, qint64)));
				}
			}
			else
			{
				m_getAllId++;
				m_progressdialog->setValue(m_progressdialog->value()+img->value());
				m_progressdialog->setImages(m_getAllId);
				m_getAllExists++;
				log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
				m_progressdialog->loadedImage(img->url());
				m_getAllDetails.clear();
				_getAll();
			}
		}
	}
	else
	{
		log("Images download finished.");
		m_progressdialog->setValue(m_progressdialog->maximum());
		m_progressdialog->close();
		QMessageBox::information(
			this,
			tr("Récupération des images"),
			QString(
				tr("%n fichier(s) récupéré(s) avec succès.\r\n", "get_all_ok", m_getAllDownloaded)+
				tr("%n fichier(s) ignoré(s).\r\n", "get_all_ignored", m_getAllIgnored)+
				tr("%n fichier(s) déjà existant(s).\r\n", "get_all_exists", m_getAllExists)+
				tr("%n erreur(s).", "get_all_errors", m_getAllErrors)
			)
		);
		if (!m_settings->value("Exec/Group/init").toString().isEmpty())
		{
			m_process->closeWriteChannel();
			m_process->waitForFinished(1000);
			m_process->close();
		}
		log(tr("Téléchargement groupé terminé"));
		m_progressdialog->clear();
	}
}
void mainWindow::getAllProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	double speed = bytesReceived * 1000.0 / m_downloadTime->elapsed();
	QString unit;
	if (speed < 1024)
	{ unit = "bytes/sec"; }
	else if (speed < 1024*1024)
	{
		speed /= 1024;
		unit = "kB/s";
	}
	else
	{
		speed /= 1024*1024;
		unit = "MB/s";
	}
	m_progressdialog->setSpeed(QString::number(speed)+" "+unit);
	m_progressdialog->statusImage(m_getAllImages.at(m_getAllId)->url(), (100*bytesReceived)/bytesTotal);
	m_progressdialog->setLittleValue((m_getAllImages.at(m_getAllId)->value()*bytesReceived)/bytesTotal);
}
void mainWindow::getAllPerformTags(Image* img)
{
	log(tr("Tags reçus"));
	bool under = m_settings->value("Save/remplaceblanksbyunderscores", false).toBool();
	for (int i = 0; i < img->tags().count(); i++)
	{
		Tag *tag = img->tags().at(i);
		QString normalized = tag->text().replace(" ", "_"), original = normalized;
		m_getAllDetails["alls_original"].append(original);
		normalized = normalized.replace("\\", " ").replace("/", " ").replace(":", " ").replace("|", " ").replace("*", " ").replace("?", " ").replace("\"", " ").replace("<", " ").replace(">", " ").trimmed();
		if (!under)
		{ normalized.replace('_', ' '); }
		if		(tag->type() == "character")	{ m_getAllDetails["characters"].append(normalized); }
		else if (tag->type() == "copyright")	{ m_getAllDetails["copyrights"].append(normalized); }
		else if (tag->type() == "artist")		{ m_getAllDetails["artists"].append(normalized);	}
		else if (tag->type() == "model")		{ m_getAllDetails["models"].append(normalized);		}
		else									{ m_getAllDetails["generals"].append(normalized);	}
		m_getAllDetails["alls"].append(normalized);
	}

	// Row
	int site_id = 0;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
		{ site_id = i; break; }
	}

	// Getting path
	QString u = img->fileUrl().toString();
	QString path = img->path(m_groupBatchs[site_id][6]);

	// Save path
	QString p = m_groupBatchs[site_id][7];
	p.replace("\\", "/");
	if (p.right(1) == "/")
	{ p = p.left(p.length()-1); }
	QString pth = p+"/"+path;

	QFile f(pth);
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".png");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".gif");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".jpeg");	}
	if (!f.exists())
	{
		bool detected = false;
		QStringList tags = m_groupBatchs[site_id][0].split(' ');
		QList<QChar> modifiers = QList<QChar>() << '~';
		for (int r = 0; r < tags.size(); r++)
		{
			if (modifiers.contains(tags[r][0]))
			{ tags[r] = tags[r].right(tags[r].size()-1); }
		}
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
			for (int t = 0; t < img->tags().count(); t++)
			{
				if (blacklistedtags.contains(img->tags().at(t)->text(), Qt::CaseInsensitive) && !tags.contains(img->tags().at(t)->text(), Qt::CaseInsensitive))
				{
					detected = true;
					log(tr("Certains tags de l'image sont blacklistés."));
				}
			}
		}
		if (detected && m_groupBatchs[site_id][4] == "false")
		{
			m_getAllId++;
			m_progressdialog->setValue(m_progressdialog->value()+img->value());
			m_progressdialog->setImages(m_getAllId);
			m_getAllIgnored++;
			log(tr("Image ignorée."));
			m_progressdialog->loadedImage(img->url());
			m_getAllDetails.clear();
			_getAll();
		}
		else
		{
			QNetworkAccessManager *m = new QNetworkAccessManager(this);
			QNetworkRequest request(img->fileUrl());
				request.setRawHeader("Referer", img->fileUrl().toString().toAscii());
			log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a>").arg(img->fileUrl().toString()));
			m_progressdialog->loadingImage(img->url());
			m_getAllRequest = m->get(request);
			connect(m_getAllRequest, SIGNAL(finished()), this, SLOT(getAllPerformImage()));
			m_getAllRequestExists = true;
			m_downloadTime = new QTime();
			m_downloadTime->start();
			connect(m_getAllRequest, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(getAllProgress(qint64, qint64)));
		}
	}
	else
	{
		m_getAllId++;
		m_progressdialog->setValue(m_progressdialog->value()+img->value());
		m_progressdialog->setImages(m_getAllId);
		m_getAllExists++;
		log(tr("Fichier déjà existant : <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
		m_getAllDetails.clear();
		m_progressdialog->loadedImage(img->url());
		_getAll();
	}
}
void mainWindow::getAllPerformImage()
{
	QNetworkReply* reply = m_getAllRequest;
	if (reply->error() == QNetworkReply::OperationCanceledError)
	{ return; }
	log(tr("Image reçue depuis <a href=\"%1\">%1</a>").arg(reply->url().toString()));
	Image *img = m_getAllImages.at(m_getAllId);
	if (reply->error() == QNetworkReply::NoError)
	{
		// Row
		int site_id = 0;
		for (int i = 0; i < m_groupBatchs.count(); i++)
		{
			if (m_groupBatchs.at(i).at(8) == img->page()->url().toString())
			{ site_id = i; break; }
		}

		// Getting path
		QString path = img->path(m_groupBatchs[site_id][6]);
		path.replace("%n%", QString::number(m_getAllId+1));
		QString p = QDir::toNativeSeparators(m_groupBatchs[site_id][7]);
		if (path.left(1) == QDir::toNativeSeparators("/"))	{ path = path.right(path.length()-1);	}
		if (p.right(1) == QDir::toNativeSeparators("/"))	{ p = p.left(p.length()-1);				}
		QString fp = QDir::toNativeSeparators(p+"/"+path);

		QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2));
		if (!path_to_file.exists())
		{
			QDir dir(p);
			if (!dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
			{
				log(tr("<b>Erreur:</b> %1").arg(tr("impossible de créer le dossier de destination: %1.").arg(p+"/"+path.section('/', 0, -2))));
				m_getAllErrors++;
			}
		}
		QFile f(fp);
		f.open(QIODevice::WriteOnly);
			f.write(reply->readAll());
		f.close();

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
			Tag *tag = img->tags().at(i);
			QString original = tag->text().replace(" ", "_");
			if (!m_settings->value("Exec/tag").toString().isEmpty())
			{
				QString exec = m_settings->value("Exec/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag->type())
				.replace("%number%", QString::number(types[tag->type()]));
				log(tr("Execution seule de \"%1\"").arg(exec));
				QProcess::execute(exec);
			}
			if (!m_settings->value("Exec/Group/tag").toString().isEmpty())
			{
				QString exec = m_settings->value("Exec/Group/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag->type())
				.replace("%number%", QString::number(types[tag->type()]));
				log(tr("Execution groupée de \"%1\"").arg(exec));
				m_process->write(exec.toAscii());
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
			m_process->write(exec.toAscii());
		}
		m_getAllDownloaded++;
		m_progressdialog->loadedImage(img->url());
	}
	else
	{
		m_getAllErrors++;
		m_progressdialog->errorImage(img->url());
	}

	m_getAllId++;
	m_progressdialog->setValue(m_progressdialog->value()+img->value());
	m_progressdialog->setImages(m_getAllId);
	m_getAllDetails.clear();
	_getAll();
}
void mainWindow::getAllCancel()
{
	log(tr("Annulation des téléchargements..."));
	if (m_getAllRequestExists)
	{
		m_getAllRequestExists = false;
		if (m_getAllRequest->isRunning())
		{ m_getAllRequest->abort(); }
	}
	m_progressdialog->clear();
	DONE();
}
