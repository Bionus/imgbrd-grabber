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
#include "TextEdit.h"
#include "QBouton.h"
#include "json.h"

#define VERSION "1.3"

using namespace std;



mainWindow::mainWindow(QString m_program, QStringList m_params) : loaded(false), allow(true), changed(false), ch(0), updating(0), batchGroups(0), batchUniques(0), m_program(m_program), m_params(m_params)
{
	this->resize(800, 600);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle(tr("Grabber"));

	m_serverDate = QDateTime::currentDateTime();
	m_serverDate = m_serverDate.toUTC().addSecs(-60*60*4);

	this->loadFavorites();

	QSettings settings("settings.ini", QSettings::IniFormat);

	this->setWindowState(Qt::WindowStates(settings.value("state", 0).toInt()));
	if (!this->isMaximized())
	{
		this->resize(settings.value("size", QSize(800, 600)).toSize());
		this->move(settings.value("pos", QPoint(200, 200)).toPoint());
	}

	QMap<QString,QStringList> stes;
	QStringList dir, defaults = QStringList() << "xml" << "json" << "regex";
	for (int s = 0; s < 3; s++)
	{
		dir = QDir("sites\\"+settings.value("source_"+s, defaults.at(s)).toString()).entryList(QDir::Files);
		for (int i = 0; i < dir.count(); i++)
		{
			QFile file("sites\\"+settings.value("source_"+s, defaults.at(s)).toString()+"\\"+dir.at(i));
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				if (stes.value(dir.at(i).section('.', 0, -2)).empty())
				{
					stes[dir.at(i).section('.', 0, -2)] = QStringList() << settings.value("source_"+s, defaults.at(s)).toString();
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

	QString sav = settings.value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		this->selected.append(sav.at(i) == '1' ? true : false);
	}



	/* Menu */

	// Options
	menuOptions = menuBar()->addMenu(tr("&Options"));
	actionOptions = menuOptions->addAction(tr("&Options"));
		connect(actionOptions, SIGNAL(triggered()), this, SLOT(options()));
		menuOptions->addAction(actionOptions);


	// Aide
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
	
	
	
	/* Fichiers */

	QWidget *wid = new QWidget; 
	QGridLayout *champs = new QGridLayout;
		radio1 = new QRadioButton(this);
			radio1->setChecked(true);
			champs->addWidget(radio1, 0, 0, 1, 2);
			TextEdit *search = new TextEdit(favorites, this);
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
				m_date->setDisplayFormat(settings.value("dateformat", "dd/MM/yyyy").toString());
				connect(m_date, SIGNAL(dateChanged(QDate)), radio2, SLOT(toggle()));
			champs->addWidget(m_date, 1, 1, 1, 3);
	QHBoxLayout *actions = new QHBoxLayout();
		adv = new QPushButton(this);
			connect(adv, SIGNAL(clicked()), this, SLOT(advanced()));
			actions->addWidget(adv);
		gA = new QPushButton(this);
			connect(gA, SIGNAL(clicked()), this, SLOT(getPage()));
			actions->addWidget(gA);
	QGridLayout *web = new QGridLayout;
	QVBoxLayout *mainlayout = new QVBoxLayout;
		mainlayout->addLayout(champs);
			mainlayout->setAlignment(champs, Qt::AlignTop);
		mainlayout->addLayout(web);
		mainlayout->addLayout(actions);
	wid->setLayout(mainlayout);
	wid->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));

	QVBoxLayout *labelBatch = new QVBoxLayout(this);
		QLabel *txtGroups = new QLabel(tr("Groupes"));
			labelBatch->addWidget(txtGroups);
		batchTableGroups = new QTableWidget(0, 5, this);
			batchTableGroups->setHorizontalHeaderLabels(QStringList() << tr("Tags") << tr("Page") << tr("Images par page") << tr("Limite d'images") << tr("Télécharger les image de la liste noire") << tr("Sources"));
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
		QHBoxLayout *labelBatchButtons = new QHBoxLayout(this);
			QPushButton *buttonGetAll = new QPushButton(tr("Tout télécharger"));
				connect(buttonGetAll, SIGNAL(clicked()), this, SLOT(getAll()));
				labelBatchButtons->addWidget(buttonGetAll);
			QPushButton *buttonClearBatch = new QPushButton(tr("Effacer"));
				connect(buttonClearBatch, SIGNAL(clicked()), this, SLOT(batchClear()));
				labelBatchButtons->addWidget(buttonClearBatch);
		labelBatch->addLayout(labelBatchButtons);
	QWidget *tabBatch = new QWidget;
		tabBatch->setLayout(labelBatch);

	QVBoxLayout *labelLogs = new QVBoxLayout(this);
		_logLabel = new QLabel(tr(""));
			labelLogs->addWidget(_logLabel);
	QWidget *tabLog = new QWidget;
		tabLog->setLayout(labelLogs);

	this->loadLanguage(settings.value("language", "English").toString());
	
	
	
	// Zone centrale
	QTabWidget *tabs = new QTabWidget();
		tabs->addTab(wid, tr("Explorer"));
		tabs->addTab(tabBatch, tr("Télécharger"));
		tabs->addTab(tabLog, tr("Log"));
	setCentralWidget(tabs);
	
	// Sauvegarde
	this->comboSources = comboSources;
	this->artists = artists;
	this->copyrights = copyrights;
	this->characters = characters;
	this->pix = pix;
	this->image = image;
	this->web = web;
	this->search = search;
	this->files = files;
	this->area = area;
	this->status = status;
	this->statusCount = statusCount;
	this->statusPath = statusPath;
	this->statusSize = statusSize;
	this->_log = QStringList();
	this->loaded = true;

	if (!this->m_params.isEmpty())
	{
		search->setText(this->m_params.join(" "));
		search->doColor();
		webUpdate();
	}
}

void mainWindow::log(QString l)
{
	this->_log.prepend(l);
	if (this->loaded)
	{ this->_logLabel->setText(_log.join("<br/>")); }
}
void mainWindow::logClear()
{
	this->_log.clear();
	if (this->loaded)
	{ this->_logLabel->setText(""); }
}

void mainWindow::addGroup()
{
	AddGroupWindow *wAddGroup = new AddGroupWindow(this->favorites, this);
	wAddGroup->show();
}
void mainWindow::addUnique()
{
	AddUniqueWindow *wAddUnique = new AddUniqueWindow(this->sites, this);
	wAddUnique->show();
}

void mainWindow::retranslateStrings()
{
	log("Translating texts...");
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
}
void mainWindow::loadFavorites()
{
	log("Updating favorites...");
	favorites.clear();
	QFile file("favorites.txt");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QStringList wrds;
		while (!file.atEnd())
		{ wrds.append(QString(file.readLine()).split(' ', QString::SkipEmptyParts)); }
		for (int i = 0; i < wrds.count(); i++)
		{ favorites[wrds.at(i)] = 0; }
		file.close();
	}
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
	log("Saving...");
	QSettings settings("settings.ini", QSettings::IniFormat);
		settings.setValue("state", int(this->windowState()));
		settings.setValue("size", this->size());
		settings.setValue("pos", this->pos());
		QString sav;
		for (int i = 0; i < this->selected.count(); i++)
		{ sav += (this->selected.at(i) ? "1" : "0"); }
		settings.setValue("sites", sav);
	QMainWindow::closeEvent(e);
}


void mainWindow::options()
{
	log("Opening options window...");
	optionsWindow *options = new optionsWindow(this);
	options->show();
}



void mainWindow::advanced()
{
	log("Opening sources window...");
	advancedWindow *adv = new advancedWindow(this->selected, this);
	adv->show();
	connect(adv, SIGNAL(closed(advancedWindow*)), this, SLOT(saveAdvanced(advancedWindow*)));
}
void mainWindow::saveAdvanced(advancedWindow *w)
{
	log("Saving changes from sources window...");
	this->selected = w->getSelected();
}



void mainWindow::getAll()
{
	log("Grouped download started.");
	getAllId = 0;
	getAllDownloaded = 0;
	getAllIgnored = 0;
	getAllExists = 0;
	getAllErrors = 0;
	getAllCount = 0;
	allImages = batchs;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllSource(QNetworkReply*)));
	for (int i = 0; i < groupBatchs.count(); i++)
	{
		QString text = " "+groupBatchs.at(i).at(0)+" ";
		text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
			.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
			.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
			.replace("-rating:s ", "-rating:safe ", Qt::CaseInsensitive)
			.replace("-rating:q ", "-rating:questionable ", Qt::CaseInsensitive)
			.replace("-rating:e ", "-rating:explicit ", Qt::CaseInsensitive);
		QStringList tags = text.split(" ", QString::SkipEmptyParts);
		tags.removeDuplicates();
		int pp = groupBatchs.at(i).at(2).toInt();
		pp = pp > 100 ? 100 : pp;
		for (int r = 0; r < ceil(groupBatchs.at(i).at(3).toDouble()/pp); r++)
		{
			QString url = this->sites[groupBatchs.at(i).at(5)].at(2);
				url.replace("{page}", QString::number(groupBatchs.at(i).at(1).toInt()+r));
				url.replace("{tags}", tags.join(" "));
				url.replace("{limit}", QString::number(pp));
			groupBatchs[i][6] = url;
			manager->get(QNetworkRequest(QUrl(url)));
		}
	}
}

void mainWindow::getAllSource(QNetworkReply *r)
{
	QString url = r->url().toString(), source = r->readAll();
	log("Received \""+url+"\"");
	int n = 0;
	for (int i = 0; i < groupBatchs.count(); i++)
	{
		if (groupBatchs.at(i).at(6) == url)
		{ n = i; break; }
	}
	QString site = groupBatchs.at(n).at(5);
	if (this->sites[site].at(0) == "xml")
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
		log("Images download started.");
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
		log("Grouped download finished.");
	}
}
void mainWindow::getAllPerformTags(QNetworkReply* reply)
{
	// Treating tags
	log("Received tags data from \""+reply->url().toString()+"\"");
	if (reply->error() == QNetworkReply::NoError)
	{
		QSettings settings("settings.ini", QSettings::IniFormat);
		bool under = settings.value("Save/remplaceblanksbyunderscores", false).toBool();
		QString source = reply->readAll();
		QRegExp rx(this->sites[this->allImages.at(getAllId).value("site")].at(5));
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = rx.cap(1), normalized = rx.cap(2).replace(" ", "_");
			normalized.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
			if (!under)
			{ normalized.replace('_', ' '); }
			if (type == "character")		{ this->getAllDetails["characters"].append(normalized); }
			else if (type == "copyright")	{ this->getAllDetails["copyrights"].append(normalized); }
			else if (type == "artist")		{ this->getAllDetails["artists"].append(normalized);	}
			else if (type == "model")		{ this->getAllDetails["models"].append(normalized);		}
			else							{ this->getAllDetails["generals"].append(normalized);	}
			this->getAllDetails["alls"].append(normalized);
		}
	}
	// Getting path
	QString u = this->allImages.at(getAllId).value("file_url");
	QSettings settings("settings.ini", QSettings::IniFormat);
	settings.beginGroup("Save");
	QString path = settings.value("filename").toString()
	.replace("%artist%", (this->getAllDetails["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString())))
	.replace("%general%", this->getAllDetails["generals"].join(settings.value("separator").toString()))
	.replace("%copyright%", (this->getAllDetails["copyrights"].isEmpty() ? settings.value("copyright_empty").toString() : (settings.value("copyright_useall").toBool() || this->getAllDetails["copyrights"].count() == 1 ? this->getAllDetails["copyrights"].join(settings.value("copyright_sep").toString()) : settings.value("copyright_value").toString())))
	.replace("%character%", (this->getAllDetails["characters"].isEmpty() ? settings.value("character_empty").toString() : (settings.value("character_useall").toBool() || this->getAllDetails["characters"].count() == 1 ? this->getAllDetails["characters"].join(settings.value("character_sep").toString()) : settings.value("character_value").toString())))
	.replace("%model%", (this->getAllDetails["models"].isEmpty() ? settings.value("model_empty").toString() : (settings.value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString())))
	.replace("%model|artist%", (!this->getAllDetails["models"].isEmpty() ? (settings.value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString()) : (this->getAllDetails["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString()))))
	.replace("%all%", this->getAllDetails["alls"].join(settings.value("separator").toString()))
	.replace("%filename%", u.section('/', -1).section('.', 0, -2))
	.replace("%rating%", this->allImages.at(getAllId).value("rating"))
	.replace("%md5%", this->allImages.at(getAllId).value("md5"))
	.replace("%website%", this->allImages.at(getAllId).value("site"))
	.replace("%ext%", u.section('.', -1));
	// saving path
	QString pth = settings.value("path").toString()+"\\"+path;
	QFile f(pth);
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".png");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".gif");	}
	if (!f.exists())	{ f.setFileName(pth.section('.', 0, -2)+".jpeg");	}
	if (!f.exists())
	{
		bool detected = false;
		QSettings settings("settings.ini", QSettings::IniFormat);
		QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));
		for (int i = 0; i < blacklistedtags.size(); i++)
		{
			if (this->allImages.at(getAllId).value("tags").contains(blacklistedtags.at(i), Qt::CaseInsensitive))
			{ detected = true; }
		}
		if (detected && !settings.value("downloadblacklist").toBool())
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
		QSettings settings("settings.ini", QSettings::IniFormat);
		settings.beginGroup("Save");
		QString path = settings.value("filename").toString()
		.replace("%artist%", (this->getAllDetails["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString())))
		.replace("%general%", this->getAllDetails["generals"].join(settings.value("separator").toString()))
		.replace("%copyright%", (this->getAllDetails["copyrights"].isEmpty() ? settings.value("copyright_empty").toString() : (settings.value("copyright_useall").toBool() || this->getAllDetails["copyrights"].count() == 1 ? this->getAllDetails["copyrights"].join(settings.value("copyright_sep").toString()) : settings.value("copyright_value").toString())))
		.replace("%character%", (this->getAllDetails["characters"].isEmpty() ? settings.value("character_empty").toString() : (settings.value("character_useall").toBool() || this->getAllDetails["characters"].count() == 1 ? this->getAllDetails["characters"].join(settings.value("character_sep").toString()) : settings.value("character_value").toString())))
		.replace("%model%", (this->getAllDetails["models"].isEmpty() ? settings.value("model_empty").toString() : (settings.value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString())))
		.replace("%model|artist%", (!this->getAllDetails["models"].isEmpty() ? (settings.value("model_useall").toBool() || this->getAllDetails["models"].count() == 1 ? this->getAllDetails["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString()) : (this->getAllDetails["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->getAllDetails["artists"].count() == 1 ? this->getAllDetails["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString()))))
		.replace("%all%", this->getAllDetails["alls"].join(settings.value("separator").toString()))
		.replace("%filename%", u.section('/', -1).section('.', 0, -2))
		.replace("%filename%", u.section('/', -1).section('.', 0, -2))
		.replace("%rating%", this->allImages.at(getAllId).value("rating"))
		.replace("%md5%", this->allImages.at(getAllId).value("md5"))
		.replace("%ext%", u.section('.', -1));
		// saving path
		QFile f(settings.value("path").toString()+"\\"+path);
		QDir dir(settings.value("path").toString());
		if (!dir.mkpath(path.section('\\', 0, -2)))
		{ getAllErrors++; }
		else
		{
			f.open(QIODevice::WriteOnly);
			f.write(reply->readAll());
			f.close();
			getAllDownloaded++;
		}
		getAllId++;
		int count = this->progressdialog->value();
		if (this->getAllDetails["alls"].contains("absurdres"))		{ count += 3; }
		else if (this->getAllDetails["alls"].contains("highres"))	{ count += 2; }
		else																			{ count += 1; }
		this->progressdialog->setValue(count); 
		// Loading next tags
		this->getAllDetails.clear();
		this->_getAll();
	}
}
void mainWindow::getAllCancel()
{
	log("Canceling download...");
	this->getAllRequest->abort();
	this->progressdialog->close();
}



void mainWindow::webUpdate()
{
	log("Loading page...");
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
			this->web->removeWidget(this->webPics.at(i));
		}
		this->webPics.clear();
		this->details.clear();
	}
	if (!this->webSites.isEmpty())
	{
		for (int i = 0; i < this->webSites.count(); i++)
		{
			this->webSites.at(i)->hide();
			this->web->removeWidget(this->webSites.at(i));
		}
		this->webSites.clear();
	}
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	this->pagemax = 0;
	this->assoc.clear();
	QSettings settings("settings.ini", QSettings::IniFormat);
	QStringList keys = this->sites.keys();
	for (int i = 0; i < keys.count(); i++)
	{
		QString url;
		if (this->radio1->isChecked() && !this->sites[keys.at(i)].at(2).isEmpty())
		{
			QString text = " "+this->search->toPlainText()+" ";
				text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
				.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
				.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
				.replace("-rating:s ", "-rating:safe ", Qt::CaseInsensitive)
				.replace("-rating:q ", "-rating:questionable ", Qt::CaseInsensitive)
				.replace("-rating:e ", "-rating:explicit ", Qt::CaseInsensitive);
			QStringList tags = text.split(" ", QString::SkipEmptyParts);
			tags.removeDuplicates();
			this->search->setText(tags.join(" "));
			this->search->doColor();
			url = this->sites[keys.at(i)].at(2);
			url.replace("{page}", QString::number(this->page->value()-1+this->sites[keys.at(i)].at(1).toInt()));
			url.replace("{tags}", this->search->toPlainText());
			url.replace("{limit}", QString::number(settings.value("limit", 20).toInt()));
		}
		else if (!this->sites[keys.at(i)].at(3).isEmpty())
		{
			url = this->sites[keys.at(i)].at(3);
			QDateTime now = QDateTime::currentDateTime();
			url.replace("{day}", QString::number(this->m_date->date().day()));
			url.replace("{month}", QString::number(this->m_date->date().month()));
			url.replace("{year}", QString::number(this->m_date->date().year()));
		}
		this->assoc.append(url);
		if (this->selected.at(i) && !url.isEmpty())
		{ this->replies.append(manager->get(QNetworkRequest(QUrl(url)))); }
	}
}
void mainWindow::webZoom(int id)
{
	QStringList detected;
	QSettings settings("settings.ini", QSettings::IniFormat);
	if (!settings.value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));
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
	zoomWindow *zoom = new zoomWindow(m_program, this->details.at(id).value("site"), this->sites[this->details.at(id).value("site")], this->details.at(id).value("id"), this->details.at(id).value("file_url"), this->details.at(id).value("tags"), this->details.at(id).value("md5"), this->details.at(id).value("rating"), this->details.at(id).value("score"), this->details.at(id).value("author"), this);
	zoom->show();
	loadFavorites();
}
void mainWindow::replyFinished(QNetworkReply* r)
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	QString url = r->url().toString();
		log("Received page "+url);
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
		max = ceil(count/settings.value("limit", 20).toFloat());
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
				this->details.append(d);
				this->replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
				results++;
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
				this->details.append(d);
				this->replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
				results++;
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
		txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+tr("Aucun résultat")+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : ""));
	}
	else
	{ txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(QString::number(this->page->value()), (max != 0 ? QString::number(ceil(count/settings.value("limit", 20).toFloat())) : "?"), QString::number(results), (count != 0 ? QString::number(count) : "?"))); }
	connect(txt, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
	int pl = ceil(sqrt(settings.value("limit", 20).toInt()));
	float fl = (float)settings.value("limit", 20).toInt()/pl;
	this->web->addWidget(txt, floor(n/settings.value("columns", 1).toInt())*(ceil(fl)+1), pl*(n%settings.value("columns", 1).toInt()), 1, pl);
	this->web->setRowMinimumHeight(floor(n/settings.value("columns", 1).toInt())*(ceil(fl)+1), 50);
	this->webSites.append(txt);
}
void mainWindow::openUrl(QString url)
{ QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode)); }
void mainWindow::setTags(QString tags)
{ this->search->setText(tags); }
void mainWindow::replyFinishedPic(QNetworkReply* r)
{
	log("Received preview image <a href='"+r->url().toString()+"'>"+r->url().toString()+"</a>");
	QSettings settings("settings.ini", QSettings::IniFormat);
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
		l->setToolTip(tr("<b>Tags :</b> %1<br/><br/><b>ID :</b> %2<br/><b>Classe :</b> %3<br/><b>Score :</b> %4<br/><b>Posteur :</b> %5<br/><br/><b>Dimensions :</b> %6 x %7<br/><b>Taille :</b> %8 %9<br/><b>Date :</b> %10<br/>").arg(this->details.at(n).value("tags"), this->details.at(n).value("id"), assoc[this->details.at(n).value("rating")], this->details.at(n).value("score"), this->details.at(n).value("author"), this->details.at(n).value("width"), this->details.at(n).value("height"), QString::number(round(size)), unit).arg(this->details.at(n).value("created_at")));
		l->setIconSize(QSize(150, 150));
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), this, SLOT(batchChange(int)));
	int pl = ceil(sqrt(settings.value("limit", 20).toInt()));
	float fl = (float)settings.value("limit", 20).toInt()/pl;
	this->web->addWidget(l, floor(id/pl)+(floor(site/settings.value("columns", 1).toInt())*(ceil(fl)+1))+1, id%pl+pl*(site%settings.value("columns", 1).toInt()), 1, 1);
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
	QSettings settings("settings.ini", QSettings::IniFormat);
	QStringList values = QStringList() << search->toPlainText() << QString::number(this->page->value()) << settings.value("limit").toString() << settings.value("limit").toString() << "false" << "danbooru.donmai.us" << "";
	this->batchAddGroup(values);
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
		QLabel *text = new QLabel(
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
		);
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
		tr("Version %1<br />Grabber est une création de Bionus.<br/>N'hésitez pas à visiter le <a href=\"http://www.bionus.fr.cr/\">site</a> pour rester à jour, ou récupérer des fichiers de site ou des traductions.").arg(VERSION)
	);
}
