#include "windows.h"
#include <shellapi.h>
#include <string>
#include <sstream>
#include <math.h>
#include <QApplication>
#include <QtXml>
#include <QtNetwork>
#include "functions.h"
#include "mainWindow.h"
#include "zoomWindow.h"
#include "optionsWindow.h"
#include "advancedWindow.h"
#include "addgroupwindow.h"
#include "adduniquewindow.h"
#include "textedit.h"
#include "QBouton.h"
#include "json.h"
#include "favoritewindow.h"

#define VERSION	"1.7"
#define DONE()	logUpdate(tr(" Fait"));

using namespace std;



mainWindow::mainWindow(QString m_program, QStringList m_tags, QMap<QString,QString> m_params) : loaded(false), allow(true), changed(false), ch(0), updating(0), batchGroups(0), batchUniques(0), m_tags(m_tags), m_program(m_program), m_params(m_params), m_log(new QMap<QDateTime,QString>), m_currentFavorite("")
{
	this->resize(800, 600);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle(tr("Grabber"));

	m_settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);

	m_serverDate = QDateTime::currentDateTime();
	m_serverDate = m_serverDate.toUTC().addSecs(-60*60*4);

	m_favorites = loadFavorites();

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
					}
				}
			}
		}
		m_settings->setValue("firstload", false);
	}

	// Loading last window state, size and position from the settings file
	this->setWindowState(Qt::WindowStates(m_settings->value("state", 0).toInt()));
	if (!this->isMaximized())
	{
		this->resize(m_settings->value("size", QSize(800, 600)).toSize());
		this->move(m_settings->value("pos", QPoint(200, 200)).toPoint());
	}

	// Searching for availables sites
	QMap<QString,QStringList> stes;
	QStringList dir, defaults = QStringList() << "xml" << "json" << "regex";
	for (int s = 0; s < 3; s++)
	{
		dir = QDir("sites/"+m_settings->value("source_"+s, defaults.at(s)).toString()).entryList(QDir::Files);
		for (int i = 0; i < dir.count(); i++)
		{
			QFile file("sites/"+m_settings->value("source_"+s, defaults.at(s)).toString()+"/"+dir.at(i));
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				if (stes.value(dir.at(i).section('.', 0, -2)).empty())
				{
					stes[dir.at(i).section('.', 0, -2)] = QStringList() << m_settings->value("source_"+s, defaults.at(s)).toString();
					while (!file.atEnd())
					{
						QString line = file.readLine();
						line.remove("\n");
						stes[dir.at(i).section('.', 0, -2)].append(line);
					}
				}
			}
		}
	}
	QString sel = '1'+QString().fill('0',stes.count()-1);
	this->sites = stes;
	QString sav = m_settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		this->selected.append(sav.at(i) == '1' ? true : false);
	}



	/* Menu */

	// Options (what an useful menu)
	menuOptions = menuBar()->addMenu(tr("&Options"));
	actionOptions = menuOptions->addAction(tr("&Options"));
		connect(actionOptions, SIGNAL(triggered()), this, SLOT(options()));
		menuOptions->addAction(actionOptions);


	// Help (more likely an "about" menu :p)
	menuAide = menuBar()->addMenu(tr("&Aide"));
	actionHelp = menuAide->addAction(tr("&Aide"));
		actionHelp->setShortcut(QKeySequence::HelpContents);
		connect(actionHelp, SIGNAL(triggered()), this, SLOT(help()));
		menuAide->addAction(actionHelp);
	menuAide->addSeparator();
	actionAboutAuthor = menuAide->addAction(tr("&À propos de DB Viewer"));
		connect(actionAboutAuthor, SIGNAL(triggered()), this, SLOT(aboutAuthor()));
		menuAide->addAction(actionAboutAuthor);
	actionAboutQt = menuAide->addAction(tr("&À propos de Qt"));
		connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
		menuAide->addAction(actionAboutQt);
	
	
	
	/* Explore */

	QGridLayout *champs = new QGridLayout;
		radio1 = new QRadioButton(this);
			radio1->setChecked(true);
			champs->addWidget(radio1, 0, 0, 1, 2);
			TextEdit *search = new TextEdit(m_favorites.keys(), this);
				search->setContextMenuPolicy(Qt::CustomContextMenu);
				QStringList completion;
					QFile words("words.txt");
					if (words.open(QIODevice::ReadOnly | QIODevice::Text))
					{
						while (!words.atEnd())
						{
							QByteArray line = words.readLine();
							completion.append(QString(line).remove("\r\n").remove("\n").split(" ", QString::SkipEmptyParts));
						}
						completion.append(m_favorites.keys());
						completion.removeDuplicates();
						completion.sort();
						QCompleter *completer = new QCompleter(completion, this);
						completer->setCaseSensitivity(Qt::CaseInsensitive);
						search->setCompleter(completer);
					}
				connect(search, SIGNAL(textChanged()), radio1, SLOT(toggle()));
				connect(search, SIGNAL(returnPressed()), this, SLOT(webUpdate()));
			page = new QSpinBox;
				page->setValue(1);
				page->setRange(1, 1000);
                connect(page, SIGNAL(valueChanged(int)), radio1, SLOT(toggle()));
			champs->addWidget(search, 0, 1, 1, 2);
			champs->addWidget(page, 0, 3, 1, 1);
		ok = new QPushButton();
			connect(ok, SIGNAL(clicked()), this, SLOT(webUpdate()));
			champs->addWidget(ok, 0, 4, 2, 1);
		radio2 = new QRadioButton(this);
			champs->addWidget(radio2, 1, 0, 1, 2);
			m_date = new QDateEdit;
				m_date->setDateRange(QDate(2000, 1, 1), m_serverDate.date());
				m_date->setDate(m_serverDate.date());
				m_date->setDisplayFormat(m_settings->value("dateformat", "dd/MM/yyyy").toString());
				connect(m_date, SIGNAL(dateChanged(QDate)), radio2, SLOT(toggle()));
				champs->addWidget(m_date, 1, 1, 1, 2);
			m_buttonOpenCalendar = new QPushButton;
				m_calendar = new QCalendarWidget;
					m_calendar->setWindowIcon(QIcon(":/images/icon.ico"));
					m_calendar->setWindowTitle("Grabber - Choisir une date");
					m_calendar->setDateRange(QDate(2000, 1, 1), m_serverDate.date());
					m_calendar->setSelectedDate(m_serverDate.date());
					connect(m_calendar, SIGNAL(activated(QDate)), m_date, SLOT(setDate(QDate)));
					connect(m_date, SIGNAL(dateChanged(QDate)), m_calendar, SLOT(setSelectedDate(QDate)));
					connect(m_calendar, SIGNAL(activated(QDate)), m_calendar, SLOT(close()));
				connect(m_buttonOpenCalendar, SIGNAL(clicked()), m_calendar, SLOT(show()));
				champs->addWidget(m_buttonOpenCalendar, 1, 3, 1, 1);
	QHBoxLayout *actions = new QHBoxLayout;
		adv = new QPushButton(this);
			connect(adv, SIGNAL(clicked()), this, SLOT(advanced()));
			actions->addWidget(adv);
		gA = new QPushButton(this);
			connect(gA, SIGNAL(clicked()), this, SLOT(getPage()));
			actions->addWidget(gA);
	m_web = new QGridLayout;
	QVBoxLayout *mainlayout = new QVBoxLayout;
		mainlayout->addLayout(champs);
			mainlayout->setAlignment(champs, Qt::AlignTop);
		mainlayout->addLayout(m_web);
		mainlayout->addLayout(actions);
			mainlayout->setAlignment(actions, Qt::AlignBottom);
	m_tabExplore = new QWidget;
		m_tabExplore->setLayout(mainlayout);
		m_tabExplore->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));



	/* Onglet favoris */

	m_favoritesLayout = new QVBoxLayout;
		m_layoutFavorites = new QGridLayout;
			QStringList keys = m_favorites.keys();
			QString format = m_settings->value("dateformat", "dd/MM/yyyy").toString();
			for (int i = 0; i < keys.count(); i++)
			{
				QString tag = keys.at(i);
				QStringList xp = m_favorites.value(tag).split("|");
				int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
				QDateTime lastviewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);
				tag.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
				QString imagepath = savePath("thumbs/"+tag+".png");
					QBouton *image = new QBouton(i);
					if (!QFile::exists(imagepath))
					{ imagepath = ":/images/noimage.png"; }
					QPixmap img(imagepath);
					if ((img.width() > 150 || img.height() > 150) && QFile::exists(imagepath))
					{
						img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
						img.save(savePath("thumbs/"+tag+".png"), "PNG");
					}
					image->setIcon(img);
					image->setIconSize(QSize(150, 150));
					image->setFlat(true);
					connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
				QAffiche *caption = new QAffiche(i);
					caption->setText(keys.at(i)+"<br/>("+QString::number(note)+" % - "+lastviewed.toString(format)+")");
					caption->setTextFormat(Qt::RichText);
					caption->setAlignment(Qt::AlignCenter);
				connect(image, SIGNAL(appui(int)), this, SLOT(loadFavorite(int)));
				connect(caption, SIGNAL(clicked(int)), this, SLOT(loadFavorite(int)));
				m_layoutFavorites->addWidget(image, (i/10)*2, i%10);
				m_layoutFavorites->addWidget(caption, (i/10)*2+1, i%10);
				m_favoritesImages.append(image);
				m_favoritesCaptions.append(caption);
			}
		QHBoxLayout *favorites_actions = new QHBoxLayout;
			m_favoritesButtonViewed = new QPushButton(this);
				connect(m_favoritesButtonViewed, SIGNAL(clicked()), this, SLOT(viewed()));
				favorites_actions->addWidget(m_favoritesButtonViewed);
			m_favoritesButtonBack = new QPushButton(this);
				connect(m_favoritesButtonBack, SIGNAL(clicked()), this, SLOT(favoritesBack()));
				favorites_actions->addWidget(m_favoritesButtonBack);
			m_favoritesButtonAdvanced = new QPushButton(this);
				connect(m_favoritesButtonAdvanced, SIGNAL(clicked()), this, SLOT(advanced()));
				favorites_actions->addWidget(m_favoritesButtonAdvanced);
		m_webFavorites = new QGridLayout;
			m_favoritesLayout->addLayout(m_layoutFavorites);
			m_favoritesLayout->addLayout(m_webFavorites);
			m_favoritesLayout->addLayout(favorites_actions);
				m_favoritesLayout->setAlignment(favorites_actions, Qt::AlignBottom);
	 m_tabFavorites = new QWidget;
		m_tabFavorites->setLayout(m_favoritesLayout);
		m_tabFavorites->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));



	/* Onglet batch */

	QVBoxLayout *labelBatch = new QVBoxLayout;
		QLabel *txtGroups = new QLabel(tr("Groupes"));
			labelBatch->addWidget(txtGroups);
		batchTableGroups = new QTableWidget(0, 9, this);
			batchTableGroups->setHorizontalHeaderLabels(QStringList() << tr("Tags") << tr("Page") << tr("Images par page") << tr("Limite d'images") << tr("Télécharger les image de la liste noire") << tr("Source") << tr("Populaires") << tr("Format") << tr("Dossier"));
			labelBatch->addWidget(batchTableGroups);
			connect(batchTableGroups, SIGNAL(cellChanged(int,int)), this, SLOT(updateBatchGroups(int,int)));
		QPushButton *buttonAddGroup = new QPushButton(tr("Ajouter"));
			connect(buttonAddGroup, SIGNAL(clicked()), this, SLOT(addGroup()));
			labelBatch->addWidget(buttonAddGroup);
		QLabel *txtUniques = new QLabel(tr("Images seules"));
			labelBatch->addWidget(txtUniques);
		batchTableUniques = new QTableWidget(0, 6, this);
			batchTableUniques->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Md5") << tr("Classe") << tr("Tags") << tr("Url") << tr("Site"));
			labelBatch->addWidget(batchTableUniques);
		QPushButton *buttonAddUnique = new QPushButton(tr("Ajouter"));
			connect(buttonAddUnique, SIGNAL(clicked()), this, SLOT(addUnique()));
			labelBatch->addWidget(buttonAddUnique);
		QHBoxLayout *labelBatchButtons = new QHBoxLayout;
			QPushButton *buttonGetAll = new QPushButton(tr("Tout télécharger"));
				connect(buttonGetAll, SIGNAL(clicked()), this, SLOT(getAll()));
				labelBatchButtons->addWidget(buttonGetAll);
			QPushButton *buttonClearBatch = new QPushButton(tr("Effacer"));
				connect(buttonClearBatch, SIGNAL(clicked()), this, SLOT(batchClear()));
				labelBatchButtons->addWidget(buttonClearBatch);
		labelBatch->addLayout(labelBatchButtons);
	m_tabBatch = new QWidget;
		m_tabBatch->setLayout(labelBatch);


	/* Onglet log */

	QVBoxLayout *labelLogs = new QVBoxLayout;
		QScrollArea *logscroll = new QScrollArea;
			_logLabel = new QLabel(tr(""));
				_logLabel->setWordWrap(true);
				_logLabel->setTextFormat(Qt::RichText);
				_logLabel->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
				_logLabel->setOpenExternalLinks(true);
			logscroll->setWidget(_logLabel);
			logscroll->setWidgetResizable(true);
			labelLogs->addWidget(logscroll);
		m_logClear = new QPushButton(this);
			connect(m_logClear, SIGNAL(clicked()), this, SLOT(logClear()));
			labelLogs->addWidget(m_logClear);
	 m_tabLog = new QWidget;
		m_tabLog->setLayout(labelLogs);
	
	// Zone centrale
	m_tabs = new QTabWidget;
		m_tabs->addTab(m_tabExplore, "{tab}");
		m_tabs->addTab(m_tabFavorites, "{tab}");
		m_tabs->addTab(m_tabBatch, "{tab}");
		if (m_settings->value("Log/show", true).toBool())
		{ m_tabs->addTab(m_tabLog, "{tab}"); }
	setCentralWidget(m_tabs);
	
	// Sauvegarde
	this->comboSources = comboSources;
	this->artists = artists;
	this->copyrights = copyrights;
	this->characters = characters;
	this->pix = pix;
	this->image = image;
	this->search = search;
	this->files = files;
	this->area = area;
	this->status = status;
	this->statusCount = statusCount;
	this->statusPath = statusPath;
	this->statusSize = statusSize;
	this->loaded = true;

	this->loadLanguage(m_settings->value("language", "English").toString());

	if (this->m_params.keys().contains("batch"))
	{
		this->batchAddGroup(QStringList() << this->m_tags.join(" ") << this->m_params.value("page", "1") << this->m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("limit", m_settings->value("limit", 20).toString()) << "false" << this->m_params.value("booru", this->sites.keys().at(0)) << "false" << this->m_params.value("filename", m_settings->value("filename").toString()) << this->m_params.value("path", m_settings->value("path").toString()) << "");
		m_tabs->setCurrentIndex(2);
		if (!this->m_params.keys().contains("dontstart"))
		{ this->getAll(); }
	}
	else if (!this->m_tags.isEmpty() || m_settings->value("loadatstart", false).toBool())
	{
		search->setText(this->m_tags.join(" "));
		search->doColor();
		webUpdate();
	}

	if (m_settings->value("lastupdate").toDateTime().addSecs(m_settings->value("updatesrate", 86400).toInt()) <= QDateTime::currentDateTime())
	{
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedVersion(QNetworkReply*)));
		manager->get(QNetworkRequest(QUrl("http://imgbrd-grabber.googlecode.com/svn/trunk/release/VERSION")));
	}
}
void mainWindow::loadFavorite(int id)
{
	QString tag = m_favorites.keys().at(id);
	this->m_currentFavorite = tag;
	m_loadFavorite = QDateTime::fromString(m_favorites.value(tag).section('|', 1, 1), Qt::ISODate);
	web(tag);
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
	QFile f("favorites.txt");
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(tag+"\\|([^|]+)\\|([^|]+)\\|([^|]+)\r\n");
	reg.setMinimal(true);
	favs.replace(reg, tag+"|\\1|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"|\\3\r\n");
	f.open(QIODevice::WriteOnly);
		f.write(favs.toAscii());
	f.close();
}
void mainWindow::favoritesBack()
{
	if (!m_currentFavorite.isEmpty())
	{
		m_currentFavorite = "";
		if (!this->replies.isEmpty())
		{
			for (int i = 0; i < this->replies.count(); i++)
			{ this->replies.at(i)->abort(); }
			this->replies.clear();
		}
		if (!this->webPics.isEmpty())
		{
			for (int i = 0; i < this->webPics.count(); i++)
			{
				this->webPics.at(i)->hide();
				this->m_webFavorites->removeWidget(this->webPics.at(i));
			}
			this->webPics.clear();
			this->details.clear();
		}
		if (!this->webSites.isEmpty())
		{
			for (int i = 0; i < this->webSites.count(); i++)
			{
				this->webSites.at(i)->hide();
				this->m_webFavorites->removeWidget(this->webSites.at(i));
			}
			for (int i = 0; i < this->webSites.count()*11; i++)
			{ this->m_webFavorites->setRowMinimumHeight(i, 0); }
			this->webSites.clear();
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
void mainWindow::updateFavorites()
{
	m_favorites = loadFavorites();
	QStringList keys = m_favorites.keys();
	QString format = m_settings->value("dateformat", "dd/MM/yyyy").toString();
	for (int i = 0; i < keys.count(); i++)
	{
		QString tag = keys.at(i);
		QStringList xp = m_favorites.value(tag).split("|");
		int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
		QDateTime lastviewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);
		tag.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
		QString imagepath = savePath("thumbs/"+tag+".png");
		if (!QFile::exists(imagepath))
		{ imagepath = ":/images/noimage.png"; }
		QPixmap img(imagepath);
		if ((img.width() > 150 || img.height() > 150) && QFile::exists(imagepath))
		{
			img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			img.save(savePath("thumbs/"+tag+".png"), "PNG");
		}
		if (m_favoritesImages.count() > i)
		{
			m_favoritesImages.at(i)->setIcon(img);
			m_favoritesCaptions.at(i)->setText(keys.at(i)+"<br/>("+QString::number(note)+" % - "+lastviewed.toString(format)+")");
		}
		else
		{
			QBouton *image = new QBouton(i);
				image->setIcon(img);
				image->setIconSize(QSize(150, 150));
				image->setFlat(true);
				connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
			QAffiche *caption = new QAffiche(i);
				caption->setText(keys.at(i)+"<br/>("+QString::number(note)+" % - "+lastviewed.toString(format)+")");
				caption->setTextFormat(Qt::RichText);
				caption->setAlignment(Qt::AlignCenter);
			connect(image, SIGNAL(appui(int)), this, SLOT(loadFavorite(int)));
			connect(caption, SIGNAL(clicked(int)), this, SLOT(loadFavorite(int)));
			m_layoutFavorites->addWidget(image, (i/10)*2, i%10);
			m_layoutFavorites->addWidget(caption, (i/10)*2+1, i%10);
			m_favoritesImages.append(image);
			m_favoritesCaptions.append(caption);
		}
	}
}

void mainWindow::replyFinishedVersion(QNetworkReply* r)
{
	QString onlineVersion = r->readAll(), version;
	QFile file("VERSION");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!file.atEnd())
		{ version = file.readLine(); }
		file.close();
	}
	else
	{ return; }
	if (onlineVersion.toFloat() > version.toFloat())
	{
		int reply = QMessageBox::question(this, tr("Mise à jour"), tr("Une mise à jour a été détéctée (%1). Souhaitez-vous l'installer ?").arg(onlineVersion), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			#ifdef Q_OS_WIN
				QString exeFileName = m_program.replace("\\", "/").section('/', 0, -2)+"/Updater.exe";
				int result = (int)::ShellExecuteA(0, "open", exeFileName.toUtf8().constData(), 0, 0, SW_SHOWNORMAL);
				if (result == SE_ERR_ACCESSDENIED)
				{ result = (int)::ShellExecuteA(0, "runas", exeFileName.toUtf8().constData(), 0, 0, SW_SHOWNORMAL); }
				if (result <= 32)
				{ log(tr("<b>Erreur :</b> %1").arg(tr("impossible de lancer le programme de mise à jour."))); }
			#else
				QString exeFileName = m_program.replace("\\", "/").section('/', 0, -2)+"/Updater";
				if (!QProcess::startDetached(exeFileName))
				{ log(tr("<b>Erreur :</b> %1").arg(tr("impossible de lancer le programme de mise à jour."))); }
			#endif
			qApp->exit();
		}
	}
	m_settings->setValue("lastupdatecheck", QDateTime::currentDateTime());
}

void mainWindow::log(QString l)
{
	this->m_log->insert(QDateTime::currentDateTime(), l);
	logShow();
}
void mainWindow::logUpdate(QString l)
{
	QDateTime date = m_log->keys().at(m_log->count()-1);
	QString message = m_log->value(date)+l;
	this->m_log->insert(date, message);
	logShow();
}
void mainWindow::logShow()
{
	if (this->loaded)
	{
		QString txt;
		int k;
		for (int i = 0; i < m_log->size(); i++)
		{
			k = m_settings->value("Log/invert", false).toBool() ? m_log->size()-i-1 : i;
			txt += QString(i > 0 ? "<br/>" : "")+"["+m_log->keys().at(k).toString("hh':'mm")+"] "+m_log->values().at(k);
		}
		this->_logLabel->setText(txt);
	}
}
void mainWindow::logClear()
{
	this->m_log->clear();
	if (this->loaded)
	{ this->_logLabel->setText(""); }
}

void mainWindow::addGroup()
{
	AddGroupWindow *wAddGroup = new AddGroupWindow(this->sites.keys(), m_favorites.keys(), this);
	wAddGroup->show();
}
void mainWindow::addUnique()
{
	AddUniqueWindow *wAddUnique = new AddUniqueWindow(this->sites, this);
	wAddUnique->show();
}

void mainWindow::retranslateStrings()
{
	log(tr("Traduction des textes..."));
	menuOptions->setTitle(tr("&Options"));
	actionOptions->setText(tr("&Options"));
	menuAide->setTitle(tr("&Aide"));
	actionHelp->setText(tr("&Aide"));
	actionAboutAuthor->setText(tr("&À propos de DB Viewer"));
	actionAboutQt->setText(tr("&À propos de Qt"));
	radio1->setText(tr("Liste"));
	radio2->setText(tr("Populaires"));
	ok->setText(tr("Ok"));
	adv->setText(tr("Sources"));
	gA->setText(tr("Prendre cette page"));
	m_favoritesButtonViewed->setText(tr("Marquer comme vu"));
	m_favoritesButtonBack->setText(tr("Retour"));
	m_favoritesButtonAdvanced->setText(tr("Sources"));
	batchTableGroups->setHorizontalHeaderLabels(QStringList() << tr("Tags") << tr("Page") << tr("Images par page") << tr("Limite d'images") << tr("Télécharger les image de la liste noire") << tr("Source") << tr("Populaires"));
	batchTableUniques->setHorizontalHeaderLabels(QStringList() << tr("Id") << tr("Md5") << tr("Classe") << tr("Tags") << tr("Url") << tr("Site"));
	m_logClear->setText(tr("Effacer le log"));
	m_tabs->setTabText(0, tr("Explorer"));
	m_tabs->setTabText(1, tr("Favoris"));
	m_tabs->setTabText(2, tr("Télécharger"));
	m_tabs->setTabText(3, tr("Log"));
	m_buttonOpenCalendar->setText(tr("Choisir"));
	logUpdate(tr(" Fait"));
}

void mainWindow::switchTranslator(QTranslator& translator, const QString& filename)
{
	qApp->removeTranslator(&translator);
	if (translator.load(filename))
	{ qApp->installTranslator(&translator); }
}

void mainWindow::loadLanguage(const QString& rLanguage)
{
	if (m_currLang != rLanguage)
	{
		m_currLang = rLanguage;
		QLocale locale = QLocale(m_currLang);
		QLocale::setDefault(locale);
		switchTranslator(m_translator, "languages/"+rLanguage);
		this->retranslateStrings();
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
		m_settings->setValue("state", int(this->windowState()));
		m_settings->setValue("size", this->size());
		m_settings->setValue("pos", this->pos());
	DONE()
	QMainWindow::closeEvent(e);
}


void mainWindow::options()
{
	log(tr("Ouverture de la fenêtre des options..."));
	optionsWindow *options = new optionsWindow(this);
	options->show();
	DONE()
}



void mainWindow::advanced()
{
	log(tr("Ouverture de la fenêtre des sources..."));
	advancedWindow *adv = new advancedWindow(this->selected, this);
	adv->show();
	connect(adv, SIGNAL(closed(advancedWindow*)), this, SLOT(saveAdvanced(advancedWindow*)));
	DONE()
}
void mainWindow::saveAdvanced(advancedWindow *w)
{
	log(tr("Sauvegarde des nouvelles sources..."));
	this->selected = w->getSelected();
	QString sav;
	for (int i = 0; i < this->selected.count(); i++)
	{ sav += (this->selected.at(i) ? "1" : "0"); }
	m_settings->setValue("sites", sav);
	DONE()
}



void mainWindow::getAll()
{
	if (m_settings->value("Save/path").toString().isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de dossier de sauvegarde !")); return; }
	else if (m_settings->value("Save/filename").toString().isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de format de sauvegarde !")); return; }
	log(tr("Téléchargement groupé commencé."));
	getAllId = 0;
	getAllDownloaded = 0;
	getAllIgnored = 0;
	getAllExists = 0;
	getAllErrors = 0;
	getAllCount = 0;
	allImages = batchs;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllSource(QNetworkReply*)));
	if (!m_settings->value("Exec/init").toString().isEmpty())
	{
		m_process = new QProcess;
		m_process->start(m_settings->value("Exec/init").toString());
		m_process->waitForStarted(1000);
	}
	for (int i = 0; i < groupBatchs.count(); i++)
	{
		QString site = groupBatchs.at(i).at(5);
		if (groupBatchs.at(i).at(6) == "true")
		{
			QDate date = QDate::fromString(groupBatchs.at(i).at(0), Qt::ISODate);
			QString url = this->sites[site].at(3);
				url.replace("{day}", QString::number(date.day()));
				url.replace("{month}", QString::number(date.month()));
				url.replace("{year}", QString::number(date.year()));
			groupBatchs[i][9] = url;
			manager->get(QNetworkRequest(QUrl(url)));
		}
		else
		{
			QString text = " "+groupBatchs.at(i).at(0)+" ";
			text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
				.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
				.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
				.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
				.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
				.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
			QStringList tags = text.split(" ", QString::SkipEmptyParts);
			tags.removeDuplicates();
			int pp = groupBatchs.at(i).at(2).toInt();
			pp = pp > 100 ? 100 : pp;
			for (int r = 0; r < ceil(groupBatchs.at(i).at(3).toDouble()/pp); r++)
			{
				if (!this->sites.keys().contains(site))
				{ log(tr("<b>Attention :</b> %1").arg(tr("site \"%1\" not found.").arg(site))); }
				else
				{
					QString url = this->sites[site].at(2);
						url.replace("{page}", QString::number(groupBatchs.at(i).at(1).toInt()+r));
						url.replace("{tags}", tags.join(" "));
						url.replace("{limit}", QString::number(pp));
					groupBatchs[i][9] = url;
					manager->get(QNetworkRequest(QUrl(url)));
				}
			}
		}
	}
}

void mainWindow::getAllSource(QNetworkReply *r)
{
	QString url = r->url().toString(), source = r->readAll();
	log(tr("Recu \"%1\"").arg(url));
	int n = 0;
	for (int i = 0; i < groupBatchs.count(); i++)
	{
		if (groupBatchs.at(i).at(9) == url)
		{ n = i; break; }
	}
	QString site = groupBatchs.at(n).at(5);
	if (source.isEmpty())
	{ log(tr("<b>Attention :</b> %1").arg(tr("rien n'a été reçu.").arg(site))); }
	else if (this->sites[site].at(0) == "xml")
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{ error(this, tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn))); }
		QDomElement docElem = doc.documentElement();
		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int id = 0; id < nodeList.count(); id++)
			{
				QMap<QString, QString> d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = nodeList.at(id).attributes().namedItem(infos.at(i)).nodeValue(); }
				QString date(nodeList.at(id).attributes().namedItem("created_at").nodeValue());
				if (date.toInt() != 0)
				{
					QDateTime timestamp;
					timestamp.setTime_t(date.toInt());
					d["created_at"] = timestamp.toString(tr("'le' dd/MM/yyyy 'à' hh:mm"));
				}
				else
				{ d["created_at"] = qDateTimeFromString(date).toString(tr("'le' dd/MM/yyyy 'à' hh:mm")); }
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				this->allImages.append(d);
			}
		}
	}
	else if (this->sites[site].at(0) == "json")
	{
		QVariant src = Json::parse(source);
		if (!src.isNull())
		{
			QMap<QString, QVariant> sc;
			QList<QVariant> sourc = src.toList();
			for (int id = 0; id < sourc.count(); id++)
			{
				sc = sourc.at(id).toMap();
				QMap<QString, QString> d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = sc.value(infos.at(i)).toString(); }
				QDateTime timestamp;
				timestamp.setTime_t(sc.value("created_at").toMap().value("s").toInt());
				d["created_at"] = timestamp.toString(tr("le dd/MM/yyyy à hh:mm"));
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				this->allImages.append(d);
			}
		}
	}
	else if (this->sites[site].at(0) == "regex")
	{
		QRegExp rx(this->sites[site].at(6));
		QStringList order = this->sites[site].at(7).split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QMap<QString, QString> d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			d["file_url"] = d["preview_url"];
				d["file_url"].remove("preview/");
			d["site"] = site;
			d["site_id"] = QString::number(n);
			d["pos"] = QString::number(id);
			this->allImages.append(d);
			id++;
		}
	}
	getAllCount++;
	if (getAllCount == groupBatchs.count())
	{
		int count = 0;
		for (int i = 0; i < allImages.count(); i++)
		{
			if (allImages.at(i).value("tags").contains("absurdres"))	{ count += 3; }
			else if (allImages.at(i).value("tags").contains("highres"))	{ count += 2; }
			else														{ count += 1; }
		}
		QProgressDialog *progressdialog = new QProgressDialog(tr("Récupération des images"), tr("Annuler"), 0, count, this);
			progressdialog->setModal(true);
			this->progressdialog = progressdialog;
			connect(this->progressdialog, SIGNAL(canceled()), this, SLOT(getAllCancel()));
			this->progressdialog->show();
			this->progressdialog->setValue(0);
		log("All images' urls received.");
		this->_getAll();
	}
}

void mainWindow::_getAll()
{
	if (getAllId < this->allImages.count())
	{
		QString fn = m_settings->value("Save/filename").toString();
		QStringList forbidden = QStringList() << "artist" << "copyright" << "character" << "model" << "general" << "model|artist";
		m_must_get_tags = false;
		for (int i = 0; i < forbidden.count(); i++)
		{
			if (fn.contains("%"+forbidden.at(i)+"%"))
			{ m_must_get_tags = true; }
		}
		log("Images download started.");
		if (m_must_get_tags)
		{
			log("Downloading detailed tags first.");
			QString u = this->sites[this->allImages.at(getAllId).value("site")].at(4);
				u.replace("{id}", this->allImages.at(getAllId).value("id"));
			QUrl rl(u);
			QNetworkAccessManager *m = new QNetworkAccessManager(this);
			connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformTags(QNetworkReply*)));
			QNetworkRequest request(rl);
				request.setRawHeader("Referer", u.toAscii());
			this->getAllRequest = m->get(request);
		}
		else
		{
			QString u(this->allImages.at(getAllId).value("file_url"));
			QString path = m_settings->value("Save/filename").toString()
			.replace("%all%", this->allImages.at(getAllId).value("tags"))
			.replace("%all_original%", this->allImages.at(getAllId).value("tags"))
			.replace("%filename%", u.section('/', -1).section('.', 0, -2))
			.replace("%rating%", this->allImages.at(getAllId).value("rating"))
			.replace("%md5%", this->allImages.at(getAllId).value("md5"))
			.replace("%website%", this->allImages.at(getAllId).value("site"))
			.replace("%ext%", u.section('.', -1));
			QFile file(path);
			if (!file.exists())
			{
				log("Downloading pictures directly.");
				QUrl rl(u);
				QNetworkAccessManager *m = new QNetworkAccessManager(this);
				connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformImage(QNetworkReply*)));
				QNetworkRequest request(rl);
					request.setRawHeader("Referer", u.toAscii());
				this->getAllRequest = m->get(request);
			}
			else
			{ log("Image ignored."); }
		}
	}
	else
	{
		log("Images download finished.");
		this->progressdialog->setValue(this->progressdialog->maximum()); 
		QMessageBox::information(
			this, 
			tr("Récupération des images"), 
			QString(
				tr("%n fichier(s) récupéré(s) avec succès.\r\n", "", getAllDownloaded)+
				tr("%n fichier(s) ignoré(s).\r\n", "", getAllIgnored)+
				tr("%n fichier(s) déjà existant(s).\r\n", "", getAllExists)+
				tr("%n erreur(s).", "", getAllErrors)
			)
		);
		if (!m_settings->value("Exec/init").toString().isEmpty())
		{
			m_process->closeWriteChannel();
			m_process->waitForFinished(1000);
			m_process->close();
		}
		log("Grouped download finished.");
	}
}
void mainWindow::getAllPerformTags(QNetworkReply* reply)
{
	// Treating tags
	log("Received tags data from \""+reply->url().toString()+"\"");
	if (reply->error() == QNetworkReply::NoError)
	{
		bool under = m_settings->value("Save/remplaceblanksbyunderscores", false).toBool();
		QString source = reply->readAll();
		QRegExp rx(this->sites[this->allImages.at(getAllId).value("site")].at(5));
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = rx.cap(1), normalized = rx.cap(2).replace(" ", "_"), original = normalized;
			this->getAllDetails["alls_original"].append(original);
			normalized.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
			if (!under)
			{ normalized.replace('_', ' '); }
			if (type == "character")		{ this->getAllDetails["characters"].append(normalized); }
			else if (type == "copyright")	{ this->getAllDetails["copyrights"].append(normalized); }
			else if (type == "artist")		{ this->getAllDetails["artists"].append(normalized);	}
			else if (type == "model")		{ this->getAllDetails["models"].append(normalized);		}
			else							{ this->getAllDetails["generals"].append(normalized);	}
			this->getAllDetails["alls"].append(normalized);
			if (!m_settings->value("Exec/tag").toString().isEmpty())
			{
				QMap<QString,int> types;
				types["general"] = 0;
				types["artist"] = 1;
				types["general"] = 2;
				types["copyright"] = 3;
				types["character"] = 4;
				types["model"] = 5;
				types["photo_set"] = 6;
				QString exec = m_settings->value("Exec/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", type)
				.replace("%number%", QString::number(types[type]));
				if (!m_settings->value("Exec/init").toString().isEmpty())
				{ m_process->write(exec.toAscii()); }
				else
				{
					m_process->start(exec);
					m_process->waitForStarted(1000);
					m_process->close();
				}
			}
		}
	}
	// Getting path
	QString u = this->allImages.at(getAllId).value("file_url");
	m_settings->beginGroup("Save");
	QString path = m_settings->value("filename").toString()
	.replace("%artist%", (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
	.replace("%general%", this->getAllDetails["generals"].join(m_settings->value("separator").toString()))
	.replace("%copyright%", (this->getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || this->getAllDetails["copyrights"].count() == 1 ? this->getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
	.replace("%character%", (this->getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || this->getAllDetails["characters"].count() == 1 ? this->getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
	.replace("%model%", (this->getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
	.replace("%model|artist%", (!this->getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
	.replace("%all%", this->getAllDetails["alls"].join(m_settings->value("separator").toString()))
	.replace("%all_original%", this->getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
	.replace("%filename%", u.section('/', -1).section('.', 0, -2))
	.replace("%rating%", this->allImages.at(getAllId).value("rating"))
	.replace("%md5%", this->allImages.at(getAllId).value("md5"))
	.replace("%website%", this->allImages.at(getAllId).value("site"))
	.replace("%ext%", u.section('.', -1));
	// saving path
	QString p = m_settings->value("path").toString().replace("\\", "/");
	m_settings->endGroup();
	if (p.right(1) == "/")
	{ p = p.left(p.length()-1); }
	QString pth = p+"\\"+path;
	QFile f(pth);
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".png");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".gif");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".jpeg");	}
	if (!f.exists())
	{
		bool detected = false;
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
			for (int i = 0; i < blacklistedtags.size(); i++)
			{
				if (this->allImages.at(getAllId).value("tags").contains(blacklistedtags.at(i), Qt::CaseInsensitive))
				{ detected = true; }
			}
		}
		if (detected && !m_settings->value("downloadblacklist").toBool())
		{
			int count = this->progressdialog->value();
			if (this->allImages.at(getAllId).value("tags").contains("absurdres"))		{ count += 3; }
			else if (this->allImages.at(getAllId).value("tags").contains("highres"))	{ count += 2; }
			else																										{ count += 1; }
			this->progressdialog->setValue(count); 
			getAllId++;
			getAllIgnored++;
			this->getAllDetails.clear();
			this->_getAll();
		}
		else
		{
			QString u(this->allImages.at(getAllId).value("file_url"));
			QUrl rl(u);
			QNetworkAccessManager *m = new QNetworkAccessManager(this);
			connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformImage(QNetworkReply*)));
			QNetworkRequest request(rl);
				request.setRawHeader("Referer", u.toAscii());
			this->getAllRequest = m->get(request);
		}
	}
	else
	{
		getAllId++;
		int count = this->progressdialog->value();
		if (this->getAllDetails["alls"].contains("absurdres"))		{ count += 3; }
		else if (this->getAllDetails["alls"].contains("highres"))	{ count += 2; }
		else																			{ count += 1; }
		this->progressdialog->setValue(count); 
		getAllExists++;
		// Loading next tags
		this->getAllDetails.clear();
		this->_getAll();
	}
}
void mainWindow::getAllPerformImage(QNetworkReply* reply)
{
	log("Received image from \""+reply->url().toString()+"\"");
	if (reply->error() == QNetworkReply::NoError)
	{
		// Getting path
		QString u = reply->url().toString();
		QSettings settings("m_settings->ini", QSettings::IniFormat);
		m_settings->beginGroup("Save");
		QString path = m_settings->value("filename").toString()
		.replace("%artist%", (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
		.replace("%general%", this->getAllDetails["generals"].join(m_settings->value("separator").toString()))
		.replace("%copyright%", (this->getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || this->getAllDetails["copyrights"].count() == 1 ? this->getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
		.replace("%character%", (this->getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || this->getAllDetails["characters"].count() == 1 ? this->getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
		.replace("%model%", (this->getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
		.replace("%model|artist%", (!this->getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
		.replace("%all%", this->getAllDetails["alls"].join(m_settings->value("separator").toString()))
		.replace("%all_original%", this->getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
		.replace("%filename%", u.section('/', -1).section('.', 0, -2))
		.replace("%rating%", this->allImages.at(getAllId).value("rating"))
		.replace("%md5%", this->allImages.at(getAllId).value("md5"))
		.replace("%ext%", u.section('.', -1))
		.replace("\\", "/");
		if (path.left(1) == "/")
		{ path = path.right(path.length()-1); }
		// saving path
		QString p = m_settings->value("path").toString().replace("\\", "/");
		if (p.right(1) == "/")
		{ p = p.left(p.length()-1); }
		QFile f(p+"/"+path);
		QDir dir(p);
		m_settings->endGroup();
		if (!m_settings->value("Exec/image").toString().isEmpty())
		{
			QString exec = m_settings->value("Exec/image").toString();
			m_settings->beginGroup("Save");
			exec.replace("%artist%", (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
			.replace("%general%", this->getAllDetails["generals"].join(m_settings->value("separator").toString()))
			.replace("%copyright%", (this->getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || this->getAllDetails["copyrights"].count() == 1 ? this->getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
			.replace("%character%", (this->getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || this->getAllDetails["characters"].count() == 1 ? this->getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
			.replace("%model%", (this->getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
			.replace("%model|artist%", (!this->getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (this->getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
			.replace("%all%", this->getAllDetails["alls"].join(m_settings->value("separator").toString()))
			.replace("%all_original%", this->getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
			.replace("%filename%", u.section('/', -1).section('.', 0, -2))
			.replace("%rating%", this->allImages.at(getAllId).value("rating"))
			.replace("%md5%", this->allImages.at(getAllId).value("md5"))
			.replace("%ext%", u.section('.', -1))
			.replace("%filename%", path)
			.replace("\\", "/")
			.replace("%path%", p+"/"+path);
			m_settings->endGroup();
			if (!m_settings->value("Exec/init").toString().isEmpty())
			{ m_process->write(exec.toAscii()); }
			else
			{
				m_process->start(exec);
				m_process->waitForStarted(1000);
				m_process->close();
			}
		}
		QDir path_to_file(p+"/"+path.section('/', 0, -2));
		if (!path_to_file.exists())
		{
			if (!dir.mkpath(path.section('/', 0, -2)))
			{
				log(tr("<b>Error:</b> %1").arg(tr("impossible to create the destination folder: %1.").arg(p+"/"+path.section('/', 0, -2))));
				getAllErrors++;
			}
		}
		f.open(QIODevice::WriteOnly);
		f.write(reply->readAll());
		f.close();
		getAllDownloaded++;
		getAllId++;
		int count = this->progressdialog->value();
		if (this->getAllDetails["alls"].contains("absurdres"))		{ count += 3; }
		else if (this->getAllDetails["alls"].contains("highres"))	{ count += 2; }
		else														{ count += 1; }
		this->progressdialog->setValue(count);
		// Loading next tags
		this->getAllDetails.clear();
		this->_getAll();
	}
	else
	{ getAllErrors++; }
}
void mainWindow::getAllCancel()
{
	log("Canceling download...");
	this->getAllRequest->abort();
	this->progressdialog->close();
}



void mainWindow::webUpdate()
{
	m_loadFavorite = QDateTime();
	web();
}
void mainWindow::web(QString tags)
{
	tags = (tags.isEmpty() ? this->search->toPlainText() : tags);
	if (!this->replies.isEmpty())
	{
		for (int i = 0; i < this->replies.count(); i++)
		{ this->replies.at(i)->abort(); }
		this->replies.clear();
	}
	if (!this->webPics.isEmpty())
	{
		for (int i = 0; i < this->webPics.count(); i++)
		{
			this->webPics.at(i)->hide();
			if (!m_loadFavorite.isNull())
			{ this->m_webFavorites->removeWidget(this->webPics.at(i)); }
			else
			{ this->m_web->removeWidget(this->webPics.at(i)); }
		}
		this->webPics.clear();
		this->details.clear();
	}
	if (!this->webSites.isEmpty())
	{
		for (int i = 0; i < this->webSites.count(); i++)
		{
			this->webSites.at(i)->hide();
			if (!m_loadFavorite.isNull())
			{ this->m_webFavorites->removeWidget(this->webSites.at(i)); }
			else
			{ this->m_web->removeWidget(this->webSites.at(i)); }
		}
		for (int i = 0; i < this->webSites.count()*11; i++)
		{
			if (!m_loadFavorite.isNull())
			{ this->m_webFavorites->setRowMinimumHeight(i, 0); }
			else
			{ this->m_web->setRowMinimumHeight(i, 0); }
		}
		this->webSites.clear();
	}
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	this->pagemax = 0;
	this->assoc.clear();
	QStringList keys = this->sites.keys();
	for (int i = 0; i < keys.count(); i++)
	{
		QString url;
		if (this->radio1->isChecked() && !this->sites[keys.at(i)].at(2).isEmpty())
		{
			QString text = " "+tags+" ";
				text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
				.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
				.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
				.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
				.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
				.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
			QStringList tags = text.split(" ", QString::SkipEmptyParts);
			tags.removeDuplicates();
			if (m_loadFavorite.isNull())
			{
				this->search->setText(tags.join(" "));
				this->search->doColor();
			}
			url = this->sites[keys.at(i)].at(2);
			url.replace("{page}", QString::number(this->page->value()-1+this->sites[keys.at(i)].at(1).toInt()));
			url.replace("{tags}", tags.join(" "));
			url.replace("{limit}", QString::number(m_settings->value("limit", 20).toInt()));
		}
		else if (!this->sites[keys.at(i)].at(3).isEmpty())
		{
			url = this->sites[keys.at(i)].at(3);
			url.replace("{day}", QString::number(this->m_date->date().day()));
			url.replace("{month}", QString::number(this->m_date->date().month()));
			url.replace("{year}", QString::number(this->m_date->date().year()));
		}
		this->assoc.append(url);
		if (this->selected.at(i) && !url.isEmpty())
		{
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(url));
			this->replies.append(manager->get(QNetworkRequest(QUrl(url))));
		}
	}
}
void mainWindow::webZoom(int id)
{
	QStringList detected;
	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
		for (int i = 0; i < blacklistedtags.size(); i++)
		{
			if (this->details.at(id).value("tags").contains(blacklistedtags.at(i), Qt::CaseInsensitive))
			{ detected.append(blacklistedtags.at(i)); }
		}
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(this, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}
	zoomWindow *zoom = new zoomWindow(m_program, this->details.at(id).value("site"), this->sites[this->details.at(id).value("site")], this->details.at(id), this);
	zoom->show();
	m_favorites = loadFavorites();
}
void mainWindow::replyFinished(QNetworkReply* r)
{
	QString url = r->url().toString();
	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(url));
	int n = 0, site_id = 0, results = 0;
	for (int i = 0; i < this->assoc.count(); i++)
	{
		if (this->assoc.at(i) == url)
		{
			site_id = i;
			break;
		}
		if (this->selected.at(i))
		{ n++; }
	}
	int max = 0;
	float count = 0;
	QString site = this->sites.keys().at(site_id), source = r->readAll();
	QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
	connect(mngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedPic(QNetworkReply*)));
	if (this->sites[site].at(0) == "xml")
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{ error(this, tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn))); }
		QDomElement docElem = doc.documentElement();
		// Getting last page
		count = docElem.attributes().namedItem("count").nodeValue().toFloat();
		max = ceil(count/m_settings->value("limit", 20).toFloat());
		if (max < 1)
		{ max = 1; }
		if (pagemax < max)
		{
			this->pagemax = max;
			this->page->setMaximum(max);
		}
		// Reading posts
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int id = 0; id < nodeList.count(); id++)
			{
				QMap<QString, QString> d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = nodeList.at(id).attributes().namedItem(infos.at(i)).nodeValue(); }
				QString date(nodeList.at(id).attributes().namedItem("created_at").nodeValue());
				QDateTime timestamp;
				if (date.toInt() != 0)
				{ timestamp.setTime_t(date.toInt()); }
				else
				{ timestamp = qDateTimeFromString(date); }
				d["created_at"] = timestamp.toString(tr("'le' dd/MM/yyyy 'à' hh:mm"));
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				if (m_loadFavorite.isNull() || timestamp > m_loadFavorite)
				{
					this->details.append(d);
					this->replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
					results++;
				}
			}
		}
	}
	else if (this->sites[site].at(0) == "json")
	{
		QVariant src = Json::parse(source);
		if (!src.isNull())
		{
			QMap<QString, QVariant> sc;
			QList<QVariant> sourc = src.toList();
			for (int id = 0; id < sourc.count(); id++)
			{
				sc = sourc.at(id).toMap();
				QMap<QString, QString> d;
				QStringList infos;
				infos << "status" << "source" << "has_comments" << "file_url" << "sample_url" << "change" << "sample_width" << "has_children" << "preview_url" << "width" << "md5" << "preview_width" << "sample_height" << "parent_id" << "height" << "has_notes" << "creator_id" << "file_size" << "id" << "preview_height" << "rating" << "tags" << "author" << "score";
				for (int i = 0; i < infos.count(); i++)
				{ d[infos.at(i)] = sc.value(infos.at(i)).toString(); }
				QDateTime timestamp;
				timestamp.setTime_t(sc.value("created_at").toMap().value("s").toInt());
				d["created_at"] = timestamp.toString(tr("le dd/MM/yyyy à hh:mm"));
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				if (m_loadFavorite.isNull() || timestamp > m_loadFavorite)
				{
					this->details.append(d);
					this->replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
					results++;
				}
			}
		}
	}
	else if (this->sites[site].at(0) == "regex")
	{
		// Getting last page
		QRegExp rxlast(this->sites[site].at(8));
		rxlast.setMinimal(true);
		rxlast.indexIn(source, 0);
		max = rxlast.cap(1).toInt();
		if (max < 1)
		{ max = 1; }
		if (pagemax < max)
		{
			this->pagemax = max;
			this->page->setMaximum(max);
		}
		// Getting images
		QRegExp rx(this->sites[site].at(6));
		QStringList order = this->sites[site].at(7).split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QMap<QString, QString> d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			d["file_url"] = d["preview_url"];
				d["file_url"].remove("preview/");
			d["site"] = site;
			d["site_id"] = QString::number(n);
			d["pos"] = QString::number(id);
			this->details.append(d);
			this->replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
			results++;
			id++;
		}
	}
	QLabel *txt = new QLabel();
	if (results == 0)
	{
		QStringList reasons = QStringList();
		if (source.isEmpty())
		{ reasons.append(tr("serveur hors-ligne")); }
		if (this->search->toPlainText().count(" ") > 1)
		{ reasons.append(tr("trop de tags")); }
		if (this->page->value() > 1000)
		{ reasons.append(tr("page trop éloignée")); }
		txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+(!m_loadFavorite.isNull() ? tr("Aucun résultat depuis le %1").arg(m_loadFavorite.toString(m_settings->value("dateformat", "dd/MM/yyyy").toString())) : tr("Aucun résultat")+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : "")));
	}
	else
	{ txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(QString::number(this->page->value()), (max != 0 ? QString::number(ceil(count/m_settings->value("limit", 20).toFloat())) : "?"), QString::number(results), (count != 0 ? QString::number(count) : "?"))); }
	connect(txt, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	if (!m_loadFavorite.isNull())
	{
		this->m_webFavorites->addWidget(txt, floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), pl*(n%m_settings->value("columns", 1).toInt()), 1, pl);
		this->m_webFavorites->setRowMinimumHeight(floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), 50);
	}
	else
	{
		this->m_web->addWidget(txt, floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), pl*(n%m_settings->value("columns", 1).toInt()), 1, pl);
		this->m_web->setRowMinimumHeight(floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), 50);
	}
	this->webSites.append(txt);
}
void mainWindow::openUrl(QString url)
{ QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode)); }
void mainWindow::setTags(QString tags)
{ this->search->setText(tags); }
void mainWindow::replyFinishedPic(QNetworkReply* r)
{
	log("Received preview image <a href='"+r->url().toString()+"'>"+r->url().toString()+"</a>");
	QSettings settings("m_settings->ini", QSettings::IniFormat);
	int id = 0, site = 0, n = 0;
	for (int i = 0; i < this->details.count(); i++)
	{
		if (this->details.at(i).value("preview_url") == r->url().toString())
		{
			site = this->details.at(i).value("site_id").toInt();
			id = this->details.at(i).value("pos").toInt();
			n = i;
			break;
		}
	}
	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	QString unit;
	int size = this->details.at(n).value("file_size").toInt();
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
	QPixmap pic;
		pic.loadFromData(r->readAll());
	if (pic.isNull())
	{ log("<b>Warning:</b> one of the preview pictures (<a href='"+r->url().toString()+"'>"+r->url().toString()+"</a>) is empty."); }
	QBouton *l = new QBouton(n, this);
		l->setIcon(pic);
		l->setToolTip(QString("%1%2%3%4%5%6%7%8")
			.arg(this->details.at(n).value("tags").isEmpty() ? "" : tr("<b>Tags :</b> %1<br/><br/>").arg(this->details.at(n).value("tags")))
			.arg(this->details.at(n).value("id").isEmpty() ? "" : tr("<b>ID :</b> %1<br/>").arg(this->details.at(n).value("id")))
			.arg(this->details.at(n).value("rating").isEmpty() ? "" : tr("<b>Classe :</b> %1<br/>").arg(assoc[this->details.at(n).value("rating")]))
			.arg(this->details.at(n).value("score").isEmpty() ? "" : tr("<b>Score :</b> %1<br/>").arg(this->details.at(n).value("score")))
			.arg(this->details.at(n).value("author").isEmpty() ? "" : tr("<b>Posteur :</b> %1<br/><br/>").arg(this->details.at(n).value("author")))
			.arg(this->details.at(n).value("width").isEmpty() || this->details.at(n).value("height").isEmpty() ? "" : tr("<b>Dimensions :</b> %1 x %2<br/>").arg(this->details.at(n).value("width"), this->details.at(n).value("height")))
			.arg(this->details.at(n).value("file_size").isEmpty() ? "" : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(round(size)), unit))
			.arg(this->details.at(n).value("created_at").isEmpty() ? "" : tr("<b>Date :</b> %1").arg(this->details.at(n).value("created_at")))
		);
		l->setIconSize(QSize(150, 150));
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), this, SLOT(batchChange(int)));
	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	if (!m_loadFavorite.isNull())
	{ this->m_webFavorites->addWidget(l, floor(id/pl)+(floor(site/m_settings->value("columns", 1).toInt())*(ceil(fl)+1))+1, id%pl+pl*(site%m_settings->value("columns", 1).toInt()), 1, 1); }
	else
	{ this->m_web->addWidget(l, floor(id/pl)+(floor(site/m_settings->value("columns", 1).toInt())*(ceil(fl)+1))+1, id%pl+pl*(site%m_settings->value("columns", 1).toInt()), 1, 1); }
	this->webPics.append(l);
}

void mainWindow::batchChange(int id)
{
	int n = 0;
	for (int i = 0; i < this->webPics.count(); i++)
	{
		if (this->webPics.at(i)->id() == id)
		{ n = i; break; }
	}
	this->batchAddUnique(this->details.at(n));
}
void mainWindow::batchAddUnique(QMap<QString,QString> values)
{
	if (!this->batchs.removeOne(values))
	{
		this->batchs.append(values);
		QStringList types = QStringList() << "id" << "md5" <<  "rating" << "tags" << "file_url" << "site";
		QTableWidgetItem *item;
		batchTableUniques->setRowCount(batchUniques+1);
		for (int t = 0; t < types.count(); t++)
		{
			item = new QTableWidgetItem;
				item->setFlags(Qt::NoItemFlags);
				item->setText(values.value(types.at(t)));
			batchTableUniques->setItem(batchUniques, t, item);
		}
		batchUniques++;
	}
	else
	{ /*batchTableUniques->removeRow(0);*/ }
}
void mainWindow::updateBatchGroups(int y, int x)
{
	if (allow)
	{ groupBatchs[y][x] = batchTableGroups->item(y,x)->text(); }
}

void mainWindow::getPage()
{
	QStringList actuals, keys = this->sites.keys();
	for (int i = 0; i < this->selected.count(); i++)
	{
		if (this->selected.at(i))
		{ actuals.append(keys.at(i)); }
	}
	for (int i = 0; i < actuals.count(); i++)
	{
		if (radio2->isChecked())
		{ this->batchAddGroup(QStringList() << m_date->date().toString(Qt::ISODate) << 0 << 0 << 0 << "false" << actuals.at(i) << "true" << m_settings->value("filename").toString() << m_settings->value("path").toString() << ""); }
		else
		{ this->batchAddGroup(QStringList() << search->toPlainText() << QString::number(this->page->value()) << m_settings->value("limit", 20).toString() << m_settings->value("limit", 20).toString() << "false" << actuals.at(i) << "false" << m_settings->value("filename").toString() << m_settings->value("path").toString() << ""); }
	}
}
void mainWindow::batchAddGroup(const QStringList& values)
{
	this->groupBatchs.append(values);
	QTableWidgetItem *item;
	this->batchTableGroups->setRowCount(this->batchGroups+1);
	this->allow = false;
	for (int t = 0; t < values.count(); t++)
	{
		item = new QTableWidgetItem;
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			item->setText(values.at(t));
		this->batchTableGroups->setItem(this->batchGroups, t, item);
	}
	this->allow = true;
	this->batchGroups++;
}
void mainWindow::batchClear()
{
	this->batchs.clear();
	this->batchUniques = 0;
	batchTableUniques->clearContents();
	batchTableUniques->setRowCount(0);
	groupBatchs.clear();
	this->batchGroups = 0;
	batchTableGroups->clearContents();
	batchTableGroups->setRowCount(0);
}



void mainWindow::help()
{
	QWidget *wHelp = new QWidget;
		QLabel *text = new QLabel(tr(
			"<h2>Explorer</h2>"
			"Cette page permet de voir les images directement depuis les sites d'images."
			"<h3>Liste</h3>"
			"Affiche les images selon un certains tag."
			"<h4>Tags</h4>"
			"Le premier champ correspond aux critères de la recherche. Vous pouvez accéder aux tags mis en favoris par un clic-droit."
			"<h4>Page</h4>"
			"Le second champ correspond à la page demandée et est plafonné à 1000."
			"<h3>Populaires</h3>"
			"Affiche les images populaires à une certaine date. Les champs correspondent à la date et sont demandés dans l'ordre JJ/MM/YY."
			"<h3>Sources</h3>"
			"Permet de choisir les sites sources des images, qui se diviseront en lignes et colonnes."
			"<h3>Prendre cette page</h3>"
			"Ajoute la page actuelle aux téléchargements, sans les lancer."
			"<h2>Téléchargement</h2>"
			"Celles-ci servent à télécharger en masse une série d'images."
			"<h3>Groupées</h3>"
			"Pour télécharger un certain nombre d'images selon un certain tag sur certains sites. Vous pouvez modifier ces options dans l'onglet \"Téléchargements\"."
			"<h3>Uniques</h3>"
			"Pour télécharger certaines images. Ajoutez-en à la liste en faisant un clic-droit sur l'image désirée, et retirez-en en en faisant un second."
			"<h2>Log</h2>"
			"En cas de problème, l'onglet log peut vous donner une indication sur ce qui ne va pas."
		));
			text->setWordWrap(true);
			text->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		QScrollArea *scrollArea = new QScrollArea;
			scrollArea->setWidget(text);
			//scrollArea->setWidgetResizable(true);
			scrollArea->setFrameShape(QFrame::NoFrame);
		QPushButton *closeButton = new QPushButton(tr("Fermer"));
			connect(closeButton, SIGNAL(clicked()), wHelp, SLOT(close()));
		QVBoxLayout *layout = new QVBoxLayout;
			layout->addWidget(scrollArea);
			layout->addWidget(closeButton);
	wHelp->setLayout(layout);
	wHelp->setWindowIcon(QIcon(":/images/icon.ico"));
	wHelp->setWindowTitle(tr("Grabber")+" - "+tr("Aide"));
	wHelp->setWindowFlags(Qt::Window);
	wHelp->resize(QSize(800, 600));
	wHelp->show();
}
void mainWindow::aboutAuthor()
{
	QMessageBox::information(
		this,
		tr("À propos de Grabber"),
		tr("Version %1<br />Grabber est une création de Bionus.<br/>N'hésitez pas à visiter le <a href=\"http://code.google.com/p/imgbrd-grabber/\">site</a> pour rester à jour, ou récupérer des fichiers de site ou des traductions.").arg(VERSION)
	);
}
