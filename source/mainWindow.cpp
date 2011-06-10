#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "optionsWindow.h"
#include "startwindow.h"
#include "favoritewindow.h"
#include "addgroupwindow.h"
#include "adduniquewindow.h"
#include "zoomWindow.h"
#include "functions.h"
#include "json.h"
#include <QtXml>

#define VERSION	"1.8"
#define DONE()	logUpdate(tr(" Fait"));



mainWindow::mainWindow(QString program, QStringList tags, QMap<QString,QString> params) : ui(new Ui::mainWindow), m_params(params), m_program(program), m_tags(tags), m_currentPageIsPopular(false)
{
	ui->setupUi(this);

	m_log = new QMap<QDateTime,QString>;

	m_settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		ui->comboOrderfavorites->setCurrentIndex(assoc.indexOf(m_settings->value("Favorites/order").toString()));
		ui->comboOrderasc->setCurrentIndex(int(m_settings->value("Favorites/reverse").toBool()));
		m_settings->setValue("reverse", bool(ui->comboOrderasc->currentIndex() == 1));
	loadLanguage(m_settings->value("language", "English").toString());

	m_serverDate = QDateTime::currentDateTime().toUTC().addSecs(-60*60*4);
	m_timezonedecay = QDateTime::currentDateTime().time().hour()-m_serverDate.time().hour();

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

	// Selected ones
	QString sel = '1'+QString().fill('0',stes.count()-1);
	m_sites = stes;
	QString sav = m_settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selected.append(sav.at(i) == '1' ? true : false);
	}

	// Search field
	m_search = new TextEdit(m_favorites.keys(), this);
		m_search->setContextMenuPolicy(Qt::CustomContextMenu);
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
				m_search->setCompleter(completer);
			}
		connect(m_search, SIGNAL(returnPressed()), this, SLOT(webUpdateTags()));
		ui->layoutFields->addWidget(m_search, 0, 1);

	// Calendar
	ui->datePopular->setDateRange(QDate(2000, 1, 1), m_serverDate.date());
		ui->datePopular->setDate(m_serverDate.date());
	m_calendar = new QCalendarWidget;
		m_calendar->setWindowIcon(QIcon(":/images/icon.ico"));
		m_calendar->setWindowTitle("Grabber - Choisir une date");
		m_calendar->setDateRange(QDate(2000, 1, 1), m_serverDate.date());
		m_calendar->setSelectedDate(m_serverDate.date());
		connect(m_calendar, SIGNAL(activated(QDate)), ui->datePopular, SLOT(setDate(QDate)));
		connect(ui->datePopular, SIGNAL(dateChanged(QDate)), m_calendar, SLOT(setSelectedDate(QDate)));
		connect(m_calendar, SIGNAL(activated(QDate)), m_calendar, SLOT(close()));
	connect(ui->buttonCalendar, SIGNAL(clicked()), m_calendar, SLOT(show()));

	// Console usage
	if (this->m_params.keys().contains("batch"))
	{
		batchAddGroup(QStringList() << m_tags.join(" ") << m_params.value("page", "1") << m_params.value("limit", m_settings->value("limit", 20).toString()) << this->m_params.value("limit", m_settings->value("limit", 20).toString()) << "false" << this->m_params.value("booru", m_sites.keys().at(0)) << "false" << this->m_params.value("filename", m_settings->value("filename").toString()) << this->m_params.value("path", m_settings->value("path").toString()) << "");
		ui->tabWidget->setCurrentIndex(2);
		if (!m_params.keys().contains("dontstart"))
		{ /*getAll();*/ }
	}
	else if (!m_tags.isEmpty() || m_settings->value("loadatstart", false).toBool())
	{
		m_search->setText(this->m_tags.join(" "));
		m_search->doColor();
		webUpdateTags();
	}

	m_search->setFocus();
}

mainWindow::~mainWindow()
{
	delete ui;
}



void mainWindow::getPage()
{
	QStringList actuals, keys = m_sites.keys();
	for (int i = 0; i < m_selected.count(); i++)
	{
		if (m_selected.at(i))
		{ actuals.append(keys.at(i)); }
	}
	for (int i = 0; i < actuals.count(); i++)
	{
		if (m_currentPageIsPopular)
		{ this->batchAddGroup(QStringList() << ui->datePopular->date().toString(Qt::ISODate) << 0 << 0 << 0 << "false" << actuals.at(i) << "true" << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << ""); }
		else
		{ this->batchAddGroup(QStringList() << m_search->toPlainText() << QString::number(ui->spinPage->value()) << m_settings->value("limit", 20).toString() << m_settings->value("limit", 20).toString() << "false" << actuals.at(i) << "false" << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << ""); }
	}
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
			item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
			item->setText(values.at(t));
		ui->tableBatchGroups->setItem(ui->tableBatchGroups->rowCount()-1, t, item);
	}
	m_allow = true;
}
void mainWindow::batchAddUnique(QMap<QString,QString> values)
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
				QMap<QString, QString> assoc;
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
void mainWindow::batchClear()
{
	m_batchs.clear();
	ui->tableBatchUniques->clearContents();
	ui->tableBatchUniques->setRowCount(0);
	m_groupBatchs.clear();
	ui->tableBatchGroups->clearContents();
	ui->tableBatchGroups->setRowCount(0);
}
void mainWindow::batchChange(int id)
{
	int n = 0;
	for (int i = 0; i < m_webPics.count(); i++)
	{
		if (m_webPics.at(i)->id() == id)
		{ n = i; break; }
	}
	batchAddUnique(m_details.at(n));
}
void mainWindow::updateBatchGroups(int y, int x)
{
	if (m_allow)
	{ m_groupBatchs[y][x] = ui->tableBatchGroups->item(y,x)->text(); }
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
bool sortByNote(const QMap<QString,QString> &s1, const QMap<QString,QString> &s2)
{ return s1["note"].toInt() < s2["note"].toInt(); }
bool sortByName(const QMap<QString,QString> &s1, const QMap<QString,QString> &s2)
{ return s1["name"].toLower() < s2["name"].toLower(); }
bool sortByLastviewed(const QMap<QString,QString> &s1, const QMap<QString,QString> &s2)
{ return QDateTime::fromString(s1["lastviewed"], Qt::ISODate) < QDateTime::fromString(s2["lastviewed"], Qt::ISODate); }
void mainWindow::updateFavorites()
{
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[ui->comboOrderfavorites->currentIndex()];
	bool reverse = (ui->comboOrderasc->currentIndex() == 1);
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
		if (m_favoritesImages.count() > i)
		{
			m_favoritesImages.at(i)->setIcon(img);
			m_favoritesImages.at(i)->setId(favorites[i]["id"].toInt());
			m_favoritesCaptions.at(i)->setText(favorites[i]["name"]+"<br/>("+favorites[i]["note"]+" % - "+QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format)+")");
		}
		else
		{
			QString xt = tr("<b>Nom :</b> %1<br/><b>Note :</b> %2 %%<br/><b>Dernière vue :</b> %3").arg(favorites[i]["name"], favorites[i]["note"], QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format));
			QBouton *image = new QBouton(favorites[i]["id"].toInt());
				image->setIcon(img);
				image->setIconSize(QSize(150, 150));
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
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
	QString tag = m_favorites.keys().at(id);
	m_currentFavorite = tag;
	m_loadFavorite = QDateTime::fromString(m_favorites.value(tag).section('|', 1, 1), Qt::ISODate);
	web(tag);
}
void mainWindow::webUpdateTags()
{
	m_loadFavorite = QDateTime();
	web();
}
void mainWindow::webUpdatePopular()
{
	m_loadFavorite = QDateTime();
	web("", true);
}
void mainWindow::web(QString tags, bool popular)
{
	m_currentPageIsPopular = popular;
	tags = (tags.isEmpty() ? m_search->toPlainText() : tags);
	if (!m_replies.isEmpty())
	{
		for (int i = 0; i < m_replies.count(); i++)
		{ m_replies.at(i)->abort(); }
		m_replies.clear();
	}
	if (!m_webPics.isEmpty())
	{
		for (int i = 0; i < m_webPics.count(); i++)
		{
			m_webPics.at(i)->hide();
			if (!m_loadFavorite.isNull())
			{ ui->layoutFavoritesResults->removeWidget(m_webPics.at(i)); }
			else
			{ ui->layoutResults->removeWidget(m_webPics.at(i)); }
		}
		m_webPics.clear();
		m_details.clear();
	}
	if (!m_webSites.isEmpty())
	{
		for (int i = 0; i < m_webSites.count(); i++)
		{
			m_webSites.at(i)->hide();
			if (!m_loadFavorite.isNull())
			{ ui->layoutFavoritesResults->removeWidget(m_webSites.at(i)); }
			else
			{ ui->layoutResults->removeWidget(m_webSites.at(i)); }
		}
		for (int i = 0; i < m_webSites.count()*11; i++)
		{
			if (!m_loadFavorite.isNull())
			{ ui->layoutFavoritesResults->setRowMinimumHeight(i, 0); }
			else
			{ ui->layoutResults->setRowMinimumHeight(i, 0); }
		}
		m_webSites.clear();
	}
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	m_pagemax = 0;
	m_assoc.clear();
	QStringList keys = m_sites.keys();
	for (int i = 0; i < keys.count(); i++)
	{
		QString url;
		if (!popular && !m_sites[keys.at(i)].at(2).isEmpty())
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
				m_search->setText(tags.join(" "));
				m_search->doColor();
			}
			url = m_sites[keys.at(i)].at(2);
			url.replace("{page}", QString::number(ui->spinPage->value()-1+m_sites[keys.at(i)].at(1).toInt()));
			url.replace("{tags}", tags.join(" ").replace("&", "%26"));
			url.replace("{limit}", QString::number(m_settings->value("limit", 20).toInt()));
		}
		else if (!m_sites[keys.at(i)].at(3).isEmpty())
		{
			url = m_sites[keys.at(i)].at(3);
			url.replace("{day}", QString::number(ui->datePopular->date().day()));
			url.replace("{month}", QString::number(ui->datePopular->date().month()));
			url.replace("{year}", QString::number(ui->datePopular->date().year()));
		}
		m_assoc.append(url);
		if (m_selected.at(i) && !url.isEmpty())
		{
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(url));
			m_replies.append(manager->get(QNetworkRequest(QUrl::fromEncoded(url.toAscii()))));
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
			if (m_details.at(id).value("tags").contains(blacklistedtags.at(i), Qt::CaseInsensitive))
			{ detected.append(blacklistedtags.at(i)); }
		}
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(this, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}
	zoomWindow *zoom = new zoomWindow(m_program, m_details.at(id).value("site"), m_sites[m_details.at(id).value("site")], m_details.at(id), this);
	zoom->show();
	m_favorites = loadFavorites();
}
void mainWindow::replyFinished(QNetworkReply* r)
{
	QString url = r->url().toString();
	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(url));
	QString redir = r->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
	if (!redir.isEmpty())
	{
		log(tr("Page redirigée vers <a href=\"%1\">%1</a>").arg(redir));
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
		m_replies.append(manager->get(QNetworkRequest(QUrl(redir))));
		return;
	}
	int n = 0, site_id = 0, results = 0;
	for (int i = 0; i < m_assoc.count(); i++)
	{
		if (m_assoc.at(i) == url)
		{
			site_id = i;
			break;
		}
		if (m_selected.at(i))
		{ n++; }
	}
	int max = 0;
	float count = 0;
	QString site = m_sites.keys().at(site_id), source = r->readAll();
	QNetworkAccessManager *mngr = new QNetworkAccessManager(this);
	connect(mngr, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedPic(QNetworkReply*)));
	if (m_sites[site].at(0) == "xml")
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{
			qDebug() << source;
			log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn))));
			error(this, tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
			return;
		}
		QDomElement docElem = doc.documentElement();
		// Getting last page
		count = docElem.attributes().namedItem("count").nodeValue().toFloat();
		max = ceil(count/m_settings->value("limit", 20).toFloat());
		if (max < 1)
		{ max = 1; }
		if (m_pagemax < max)
		{
			m_pagemax = max;
			ui->spinPage->setMaximum(max);
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
				{ timestamp = qDateTimeFromString(date, m_timezonedecay); }
				d["created_at"] = timestamp.toString(tr("'le' dd/MM/yyyy 'à' hh:mm"));
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				if (m_loadFavorite.isNull() || timestamp > m_loadFavorite)
				{
					m_details.append(d);
					m_replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
					results++;
				}
			}
		}
	}
	else if (m_sites[site].at(0) == "json")
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
					m_details.append(d);
					m_replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
					results++;
				}
			}
		}
	}
	else if (m_sites[site].at(0) == "regex")
	{
		// Getting last page
		QRegExp rxlast(m_sites[site].at(8));
		rxlast.setMinimal(true);
		rxlast.indexIn(source, 0);
		max = rxlast.cap(1).toInt();
		if (max < 1)
		{ max = 1; }
		if (m_pagemax < max)
		{
			m_pagemax = max;
			ui->spinPage->setMaximum(max);
		}
		// Getting images
		QRegExp rx(m_sites[site].at(6));
		QStringList order = m_sites[site].at(7).split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		while (((pos = rx.indexIn(source, pos)) != -1) && id < m_settings->value("limit", 20).toInt())
		{
			pos += rx.matchedLength();
			QMap<QString, QString> d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			if (d["preview_url"][0] == '/')
			{ d["preview_url"] = "http://"+site+d["preview_url"]; }
			if (m_sites[site][9].isEmpty())
			{
				d["file_url"] = d["preview_url"];
				d["file_url"].remove("preview/");
			}
			else
			{
				d["file_url"] = m_sites[site].at(9);
				d["file_url"].replace("{id}", d["id"])
				.replace("{md5}", d["md5"])
				.replace("{ext}", "jpg");
			}
			d["site"] = site;
			d["site_id"] = QString::number(n);
			d["pos"] = QString::number(id);
			m_details.append(d);
			m_replies.append(mngr->get(QNetworkRequest(QUrl(d["preview_url"]))));
			results++;
			id++;
		}
	}
	QLabel *txt = new QLabel();
	m_countPage[site] = results;
	if (results == 0)
	{
		QStringList reasons = QStringList();
		if (source.isEmpty())
		{ reasons.append(tr("serveur hors-ligne")); }
		if (m_search->toPlainText().count(" ") > 1)
		{ reasons.append(tr("trop de tags")); }
		if (ui->spinPage->value() > 1000)
		{ reasons.append(tr("page trop éloignée")); }
		txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+(!m_loadFavorite.isNull() ? tr("Aucun résultat depuis le %1").arg(m_loadFavorite.toString(m_settings->value("dateformat", "dd/MM/yyyy").toString())) : tr("Aucun résultat")+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : "")));
	}
	else
	{ txt->setText(site+" - <a href=\""+url+"\">"+url+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(QString::number(ui->spinPage->value()), (max != 0 ? QString::number(ceil(count/m_settings->value("limit", 20).toFloat())) : "?"), QString::number(results), (count != 0 ? QString::number(count) : "?"))); }
	txt->setOpenExternalLinks(true);
	//int pl = ceil(sqrt(results));
	//float fl = (float)results/pl;
	int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
	float fl = (float)m_settings->value("limit", 20).toInt()/pl;
	if (!m_loadFavorite.isNull())
	{
		ui->layoutFavoritesResults->addWidget(txt, floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), pl*(n%m_settings->value("columns", 1).toInt()), 1, pl);
		ui->layoutFavoritesResults->setRowMinimumHeight(floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), 50);
	}
	else
	{
		ui->layoutResults->addWidget(txt, floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), pl*(n%m_settings->value("columns", 1).toInt()), 1, pl);
		ui->layoutResults->setRowMinimumHeight(floor(n/m_settings->value("columns", 1).toInt())*(ceil(fl)+1), 50);
	}
	m_webSites.append(txt);
}
void mainWindow::setTags(QString tags)
{ m_search->setText(tags); }
void mainWindow::replyFinishedPic(QNetworkReply* r)
{
	// TODO: bug qqpart ici
	log("Received preview image <a href='"+r->url().toString()+"'>"+r->url().toString()+"</a>");
	int id = 0, site = 0, n = 0;
	QString ste;
	for (int i = 0; i < m_details.count(); i++)
	{
		if (m_details.at(i).value("preview_url") == r->url().toString())
		{
			site = m_details.at(i).value("site_id").toInt();
			id = m_details.at(i).value("pos").toInt();
			ste = m_details.at(i).value("site");
			n = i;
			break;
		}
	}
	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	QString unit;
	int size = m_details.at(n).value("file_size").toInt();
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
			.arg(m_details.at(n).value("tags").isEmpty() ? "" : tr("<b>Tags :</b> %1<br/><br/>").arg(m_details.at(n).value("tags")))
			.arg(m_details.at(n).value("id").isEmpty() ? "" : tr("<b>ID :</b> %1<br/>").arg(m_details.at(n).value("id")))
			.arg(m_details.at(n).value("rating").isEmpty() ? "" : tr("<b>Classe :</b> %1<br/>").arg(assoc[m_details.at(n).value("rating")]))
			.arg(m_details.at(n).value("score").isEmpty() ? "" : tr("<b>Score :</b> %1<br/>").arg(m_details.at(n).value("score")))
			.arg(m_details.at(n).value("author").isEmpty() ? "" : tr("<b>Posteur :</b> %1<br/><br/>").arg(m_details.at(n).value("author")))
			.arg(m_details.at(n).value("width").isEmpty() || m_details.at(n).value("height").isEmpty() ? "" : tr("<b>Dimensions :</b> %1 x %2<br/>").arg(m_details.at(n).value("width"), m_details.at(n).value("height")))
			.arg(m_details.at(n).value("file_size").isEmpty() ? "" : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(round(size)), unit))
			.arg(m_details.at(n).value("created_at").isEmpty() ? "" : tr("<b>Date :</b> %1").arg(m_details.at(n).value("created_at")))
		);
		l->setIconSize(QSize(150, 150));
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), this, SLOT(batchChange(int)));
	if (m_countPage[ste] != 0)
	{
		int pl = ceil(sqrt(m_settings->value("limit", 20).toInt()));
		float fl = (float)m_settings->value("limit", 20).toInt()/pl;
		//int pl = ceil(sqrt(m_countPage[ste]));
		//float fl = (float)m_countPage[ste]/pl;
		if (!m_loadFavorite.isNull())
		{ ui->layoutFavoritesResults->addWidget(l, floor(id/pl)+(floor(site/m_settings->value("columns", 1).toInt())*(ceil(fl)+1))+1, id%pl+pl*(site%m_settings->value("columns", 1).toInt()), 1, 1); }
		else
		{ ui->layoutResults->addWidget(l, floor(id/pl)+(floor(site/m_settings->value("columns", 1).toInt())*(ceil(fl)+1))+1, id%pl+pl*(site%m_settings->value("columns", 1).toInt()), 1, 1); }
		m_webPics.append(l);
	}
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
	DONE()
}
void mainWindow::favoritesBack()
{
	if (!m_currentFavorite.isEmpty())
	{
		m_currentFavorite = "";
		if (!m_replies.isEmpty())
		{
			for (int i = 0; i < m_replies.count(); i++)
			{ m_replies.at(i)->abort(); }
			m_replies.clear();
		}
		if (!m_webPics.isEmpty())
		{
			for (int i = 0; i < m_webPics.count(); i++)
			{
				m_webPics.at(i)->hide();
				ui->layoutFavoritesResults->removeWidget(m_webPics.at(i));
			}
			m_webPics.clear();
			m_details.clear();
		}
		if (!m_webSites.isEmpty())
		{
			for (int i = 0; i < m_webSites.count(); i++)
			{
				m_webSites.at(i)->hide();
				ui->layoutFavoritesResults->removeWidget(m_webSites.at(i));
			}
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

void mainWindow::log(QString l)
{
	qDebug() << l;
	m_log->insert(QDateTime::currentDateTime(), l);
	logShow();
}
void mainWindow::logUpdate(QString l)
{
	qDebug() << l;
	QDateTime date = m_log->keys().at(m_log->count()-1);
	QString message = m_log->value(date)+l;
	m_log->insert(date, message);
	logShow();
}
void mainWindow::logShow()
{
	QString txt;
	int k;
	for (int i = 0; i < m_log->size(); i++)
	{
		k = m_settings->value("Log/invert", false).toBool() ? m_log->size()-i-1 : i;
		txt += QString(i > 0 ? "<br/>" : "")+"["+m_log->keys().at(k).toString("hh':'mm")+"] "+m_log->values().at(k);
	}
	ui->labelLog->setText(txt);
}
void mainWindow::logClear()
{
	m_log->clear();
	ui->labelLog->setText("");
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
		switchTranslator(m_translator, "languages/"+m_currLang);
		log(tr("Traduction des textes en %1...").arg(m_currLang));
		ui->retranslateUi(this);
		logUpdate(tr(" Fait"));
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
		m_settings->beginGroup("Favorites");
			QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
			m_settings->setValue("order", assoc[ui->comboOrderfavorites->currentIndex()]);
			m_settings->setValue("reverse", bool(ui->comboOrderasc->currentIndex() == 1));
		m_settings->endGroup();
	DONE()
	e->accept();
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
	advancedWindow *adv = new advancedWindow(m_selected, this);
	adv->show();
	connect(adv, SIGNAL(valid(advancedWindow*)), this, SLOT(saveAdvanced(advancedWindow*)));
	DONE()
}
void mainWindow::saveAdvanced(advancedWindow *w)
{
	log(tr("Sauvegarde des nouvelles sources..."));
	m_selected = w->getSelected();
	QString sav;
	for (int i = 0; i < m_selected.count(); i++)
	{ sav += (m_selected.at(i) ? "1" : "0"); }
	m_settings->setValue("sites", sav);
	DONE()
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

/* Batch download */
void mainWindow::getAll()
{
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
	m_allImages = m_batchs;
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllSource(QNetworkReply*)));
	if (!m_settings->value("Exec/init").toString().isEmpty())
	{
		m_process = new QProcess;
		m_process->start(m_settings->value("Exec/init").toString());
		m_process->waitForStarted(1000);
	}
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		QString site = m_groupBatchs.at(i).at(5);
		if (m_groupBatchs.at(i).at(6) == "true")
		{
			QDate date = QDate::fromString(m_groupBatchs.at(i).at(0), Qt::ISODate);
			QString url = m_sites[site].at(3);
				url.replace("{day}", QString::number(date.day()));
				url.replace("{month}", QString::number(date.month()));
				url.replace("{year}", QString::number(date.year()));
			m_groupBatchs[i][9] = url;
			m_getAllPageCount++;
			manager->get(QNetworkRequest(QUrl(url)));
		}
		else
		{
			QString text = " "+m_groupBatchs.at(i).at(0)+" ";
			text.replace(" rating:s ", " rating:safe ", Qt::CaseInsensitive)
				.replace(" rating:q ", " rating:questionable ", Qt::CaseInsensitive)
				.replace(" rating:e ", " rating:explicit ", Qt::CaseInsensitive)
				.replace(" -rating:s ", " -rating:safe ", Qt::CaseInsensitive)
				.replace(" -rating:q ", " -rating:questionable ", Qt::CaseInsensitive)
				.replace(" -rating:e ", " -rating:explicit ", Qt::CaseInsensitive);
			QStringList tags = text.split(" ", QString::SkipEmptyParts);
			tags.removeDuplicates();
			int pp = m_groupBatchs.at(i).at(2).toInt();
			pp = pp > 100 ? 100 : pp;
			for (int r = 0; r < ceil(m_groupBatchs.at(i).at(3).toDouble()/pp); r++)
			{
				if (!m_sites.keys().contains(site))
				{ log(tr("<b>Attention :</b> %1").arg(tr("site \"%1\" not found.").arg(site))); }
				else
				{
					QString url = m_sites[site].at(2);
						url.replace("{page}", QString::number(m_groupBatchs.at(i).at(1).toInt()+r));
						url.replace("{tags}", tags.join(" ").replace("&", "%26"));
						url.replace("{limit}", QString::number(pp));
					m_groupBatchs[i][9] = url;
					m_getAllPageCount++;
					manager->get(QNetworkRequest(QUrl::fromEncoded(url.toAscii())));
				}
			}
		}
	}
}

void mainWindow::getAllSource(QNetworkReply *r)
{
	QString url = r->url().toString(), source = r->readAll();
	QList<QMap<QString, QString> > imgs;
	log(tr("Recu <a href=\"%1\">%1</a>").arg(url));
	int n = 0;
	for (int i = 0; i < m_groupBatchs.count(); i++)
	{
		if (m_groupBatchs.at(i).at(9) == url)
		{ n = i; break; }
	}
	QString site = m_groupBatchs.at(n).at(5);
	if (source.isEmpty())
	{ log(tr("<b>Attention :</b> %1").arg(tr("rien n'a été reçu.").arg(site))); }
	else if (m_sites[site].at(0) == "xml")
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{
			log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn))));
			error(this, tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)));
			return;
		}
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
				{ d["created_at"] = qDateTimeFromString(date, m_timezonedecay).toString(tr("'le' dd/MM/yyyy 'à' hh:mm")); }
				d["site"] = site;
				d["site_id"] = QString::number(n);
				d["pos"] = QString::number(id);
				imgs.append(d);
			}
		}
	}
	else if (m_sites[site].at(0) == "json")
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
				imgs.append(d);
			}
		}
	}
	else if (m_sites[site].at(0) == "regex")
	{
		QRegExp rx(m_sites[site].at(6));
		QStringList order = m_sites[site].at(7).split('|');
		rx.setMinimal(true);
		int pos = 0, id = 0;
		QString pagereg = m_sites[site].at(2);
			pagereg.replace("{page}", "(\\d*)")
			.replace("{tags}", "(?:\\w*)")
			.replace("{limit}", "(?:\\d*)");
		QRegExp regexp(pagereg+"\r\n");
			regexp.setMinimal(true);
			regexp.indexIn(url+"\r\n", 0);
		int page = regexp.cap(1).toInt();
		while (((pos = rx.indexIn(source, pos)) != -1) && (page-1)*m_groupBatchs.at(n).at(2).toInt()+id < m_groupBatchs.at(n).at(3).toInt())
		{
			pos += rx.matchedLength();
			QMap<QString, QString> d;
			for (int i = 0; i < order.size(); i++)
			{ d[order.at(i)] = rx.cap(i+1); }
			if (d["preview_url"][0] == '/')
			{ d["preview_url"] = "http://"+site+d["preview_url"]; }
			if (m_sites[site][9].isEmpty())
			{
				d["file_url"] = d["preview_url"];
				d["file_url"].remove("preview/");
			}
			else
			{
				d["file_url"] = m_sites[site].at(9);
				d["file_url"].replace("{id}", d["id"])
				.replace("{md5}", d["md5"])
				.replace("{ext}", "jpg");
			}
			d["site"] = site;
			d["site_id"] = QString::number(n);
			d["pos"] = QString::number(id);
			imgs.append(d);
			id++;
		}
	}
	if (imgs.isEmpty())
	{
		qDebug() << m_allImages.last();
	}
	else
	{ m_allImages.append(imgs); }
	m_getAllCount++;
	if (m_getAllCount == m_getAllPageCount)
	{
		int count = 0;
		for (int i = 0; i < m_allImages.count(); i++)
		{
			if (m_allImages.at(i).value("tags").contains("absurdres"))		{ count += 3; }
			else if (m_allImages.at(i).value("tags").contains("highres"))	{ count += 2; }
			else															{ count += 1; }
		}
		QProgressDialog *progressdialog = new QProgressDialog(tr("Récupération des images"), tr("Annuler"), 0, count, this);
			progressdialog->setModal(true);
			m_progressdialog = progressdialog;
			connect(m_progressdialog, SIGNAL(canceled()), this, SLOT(getAllCancel()));
			m_progressdialog->show();
			m_progressdialog->setValue(0);
		log("All images' urls received.");
		QString fn = m_settings->value("Save/filename").toString();
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
	if (m_getAllId < m_allImages.count())
	{
		if (m_must_get_tags)
		{
			QString u = m_sites[m_allImages.at(m_getAllId).value("site")].at(4);
				u.replace("{id}", m_allImages.at(m_getAllId).value("id"));
			QUrl rl(u);
			QNetworkAccessManager *m = new QNetworkAccessManager(this);
			connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformTags(QNetworkReply*)));
			QNetworkRequest request(rl);
				request.setRawHeader("Referer", u.toAscii());
			m_getAllRequest = m->get(request);
		}
		else
		{
			QString u(m_allImages.at(m_getAllId).value("file_url"));
			QString path = m_settings->value("Save/filename").toString()
			.replace("%all%", m_allImages.at(m_getAllId).value("tags"))
			.replace("%all_original%", m_allImages.at(m_getAllId).value("tags"))
			.replace("%filename%", u.section('/', -1).section('.', 0, -2))
			.replace("%rating%", m_allImages.at(m_getAllId).value("rating"))
			.replace("%md5%", m_allImages.at(m_getAllId).value("md5"))
			.replace("%website%", m_allImages.at(m_getAllId).value("site"))
			.replace("%ext%", u.section('.', -1));
			QFile file(path);
			if (!file.exists())
			{
				QUrl rl(u);
				QNetworkAccessManager *m = new QNetworkAccessManager(this);
				connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformImage(QNetworkReply*)));
				QNetworkRequest request(rl);
					request.setRawHeader("Referer", u.toAscii());
				m_getAllRequest = m->get(request);
			}
			else
			{ log("Image ignored."); }
		}
	}
	else
	{
		log("Images download finished.");
		m_progressdialog->setValue(m_progressdialog->maximum());
		QMessageBox::information(
			this,
			tr("Récupération des images"),
			QString(
				tr("%n fichier(s) récupéré(s) avec succès.\r\n", "", m_getAllDownloaded)+
				tr("%n fichier(s) ignoré(s).\r\n", "", m_getAllIgnored)+
				tr("%n fichier(s) déjà existant(s).\r\n", "", m_getAllExists)+
				tr("%n erreur(s).", "", m_getAllErrors)
			)
		);
		if (!m_settings->value("Exec/init").toString().isEmpty())
		{
			m_process->closeWriteChannel();
			m_process->waitForFinished(1000);
			m_process->close();
		}
		log("Batch download finished.");
	}
}
void mainWindow::getAllPerformTags(QNetworkReply* reply)
{
	// Treating tags
	log(tr("Tags reçus depuis <a href=\"%1\">%1</a>").arg(reply->url().toString()));
	if (reply->error() == QNetworkReply::NoError)
	{
		bool under = m_settings->value("Save/remplaceblanksbyunderscores", false).toBool();
		QString source = reply->readAll();
		QRegExp rx(m_sites[m_allImages.at(m_getAllId).value("site")].at(5));
		rx.setMinimal(true);
		int pos = 0;
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = rx.cap(1), normalized = rx.cap(2).replace(" ", "_"), original = normalized;
			m_getAllDetails["alls_original"].append(original);
			normalized.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
			if (!under)
			{ normalized.replace('_', ' '); }
			if (type == "character")		{ m_getAllDetails["characters"].append(normalized); }
			else if (type == "copyright")	{ m_getAllDetails["copyrights"].append(normalized); }
			else if (type == "artist")		{ m_getAllDetails["artists"].append(normalized);	}
			else if (type == "model")		{ m_getAllDetails["models"].append(normalized);		}
			else							{ m_getAllDetails["generals"].append(normalized);	}
			m_getAllDetails["alls"].append(normalized);
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
	QString u = m_allImages.at(m_getAllId).value("file_url");
	m_settings->beginGroup("Save");
	QString path = m_settings->value("filename").toString()
	.replace("%artist%", (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
	.replace("%general%", m_getAllDetails["generals"].join(m_settings->value("separator").toString()))
	.replace("%copyright%", (m_getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || m_getAllDetails["copyrights"].count() == 1 ? m_getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
	.replace("%character%", (m_getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || m_getAllDetails["characters"].count() == 1 ? m_getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
	.replace("%model%", (m_getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
	.replace("%model|artist%", (!m_getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
	.replace("%all%", m_getAllDetails["alls"].join(m_settings->value("separator").toString()))
	.replace("%all_original%", m_getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
	.replace("%filename%", u.section('/', -1).section('.', 0, -2))
	.replace("%rating%", m_allImages.at(m_getAllId).value("rating"))
	.replace("%md5%", m_allImages.at(m_getAllId).value("md5"))
	.replace("%website%", m_allImages.at(m_getAllId).value("site"))
	.replace("%ext%", u.section('.', -1))
	.replace("\\", "/");
	// saving path
	QString p = m_settings->value("path").toString().replace("\\", "/");
	m_settings->endGroup();
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
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(' '));
			for (int i = 0; i < blacklistedtags.size(); i++)
			{
				if (m_allImages.at(m_getAllId).value("tags").contains(blacklistedtags.at(i), Qt::CaseInsensitive))
				{
					detected = true;
					log(tr("Certains tags de l'image sont blacklistés"));
				}
			}
		}
		if (detected && !m_settings->value("downloadblacklist").toBool())
		{
			int count = m_progressdialog->value();
			if (m_allImages.at(m_getAllId).value("tags").contains("absurdres"))		{ count += 3; }
			else if (m_allImages.at(m_getAllId).value("tags").contains("highres"))	{ count += 2; }
			else																										{ count += 1; }
			m_progressdialog->setValue(count);
			m_getAllId++;
			m_getAllIgnored++;
			log(tr("Image ignorée"));
			m_getAllDetails.clear();
			_getAll();
		}
		else
		{
			QString u(m_allImages.at(m_getAllId).value("file_url"));
			QUrl rl(u);
			QNetworkAccessManager *m = new QNetworkAccessManager(this);
			connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(getAllPerformImage(QNetworkReply*)));
			QNetworkRequest request(rl);
				request.setRawHeader("Referer", u.toAscii());
			m_getAllRequest = m->get(request);
		}
	}
	else
	{
		m_getAllId++;
		int count = m_progressdialog->value();
		if (m_getAllDetails["alls"].contains("absurdres"))		{ count += 3; }
		else if (m_getAllDetails["alls"].contains("highres"))	{ count += 2; }
		else													{ count += 1; }
		m_progressdialog->setValue(count);
		m_getAllExists++;
		log(tr("Fichier déjà existant"));
		// Loading next tags
		m_getAllDetails.clear();
		_getAll();
	}
}
void mainWindow::getAllPerformImage(QNetworkReply* reply)
{
	log(tr("Image reçue depuis <a href=\"%1\">%1</a>").arg(reply->url().toString()));
	if (reply->error() == QNetworkReply::NoError)
	{
		// Getting path
		QString u = reply->url().toString();
		m_settings->beginGroup("Save");
		QString path = m_settings->value("filename").toString()
		.replace("%artist%", (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
		.replace("%general%", m_getAllDetails["generals"].join(m_settings->value("separator").toString()))
		.replace("%copyright%", (m_getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || m_getAllDetails["copyrights"].count() == 1 ? m_getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
		.replace("%character%", (m_getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || m_getAllDetails["characters"].count() == 1 ? m_getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
		.replace("%model%", (m_getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
		.replace("%model|artist%", (!m_getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
		.replace("%all%", m_getAllDetails["alls"].join(m_settings->value("separator").toString()))
		.replace("%all_original%", m_getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
		.replace("%filename%", u.section('/', -1).section('.', 0, -2))
		.replace("%rating%", m_allImages.at(m_getAllId).value("rating"))
		.replace("%md5%", m_allImages.at(m_getAllId).value("md5"))
		.replace("%website%", m_allImages.at(m_getAllId).value("site"))
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
			exec.replace("%artist%", (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString())))
			.replace("%general%", m_getAllDetails["generals"].join(m_settings->value("separator").toString()))
			.replace("%copyright%", (m_getAllDetails["copyrights"].isEmpty() ? m_settings->value("copyright_empty").toString() : (m_settings->value("copyright_useall").toBool() || m_getAllDetails["copyrights"].count() == 1 ? m_getAllDetails["copyrights"].join(m_settings->value("copyright_sep").toString()) : m_settings->value("copyright_value").toString())))
			.replace("%character%", (m_getAllDetails["characters"].isEmpty() ? m_settings->value("character_empty").toString() : (m_settings->value("character_useall").toBool() || m_getAllDetails["characters"].count() == 1 ? m_getAllDetails["characters"].join(m_settings->value("character_sep").toString()) : m_settings->value("character_value").toString())))
			.replace("%model%", (m_getAllDetails["models"].isEmpty() ? m_settings->value("model_empty").toString() : (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString())))
			.replace("%model|artist%", (!m_getAllDetails["models"].isEmpty() ? (m_settings->value("model_useall").toBool() || m_getAllDetails["models"].count() == 1 ? m_getAllDetails["models"].join(m_settings->value("model_sep").toString()) : m_settings->value("model_value").toString()) : (m_getAllDetails["artists"].isEmpty() ? m_settings->value("artist_empty").toString() : (m_settings->value("artist_useall").toBool() || m_getAllDetails["artists"].count() == 1 ? m_getAllDetails["artists"].join(m_settings->value("artist_sep").toString()) : m_settings->value("artist_value").toString()))))
			.replace("%all%", m_getAllDetails["alls"].join(m_settings->value("separator").toString()))
			.replace("%all_original%", m_getAllDetails["alls_original"].join(m_settings->value("separator").toString()))
			.replace("%filename%", u.section('/', -1).section('.', 0, -2))
			.replace("%rating%", m_allImages.at(m_getAllId).value("rating"))
			.replace("%md5%", m_allImages.at(m_getAllId).value("md5"))
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
				log(tr("<b>Erreur:</b> %1").arg(tr("impossible de créer le dossier de destination: %1.").arg(p+"/"+path.section('/', 0, -2))));
				m_getAllErrors++;
			}
		}
		f.open(QIODevice::WriteOnly);
		f.write(reply->readAll());
		f.close();
		m_getAllDownloaded++;
		m_getAllId++;
		int count = m_progressdialog->value();
		if (m_getAllDetails["alls"].contains("absurdres"))		{ count += 3; }
		else if (m_getAllDetails["alls"].contains("highres"))	{ count += 2; }
		else													{ count += 1; }
		m_progressdialog->setValue(count);
		// Loading next tags
		m_getAllDetails.clear();
		_getAll();
	}
	else
	{ m_getAllErrors++; }
}
void mainWindow::getAllCancel()
{
	log("Canceling download...");
	m_getAllRequest->abort();
	m_progressdialog->close();
	DONE()
}
