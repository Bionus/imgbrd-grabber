#include <QMessageBox>
#include <QMessageAuthenticationCode>
#include "favoritestab.h"
#include "ui_favoritestab.h"
#include "ui_mainwindow.h"
#include "QBouton.h"
#include "zoomwindow.h"
#include "favoritewindow.h"
#include "searchwindow.h"

#define DONE()	logUpdate(QObject::tr(" Fait"))
extern mainWindow *_mainwindow;



favoritesTab::favoritesTab(int id, QMap<QString,Site*> *sites, QMap<QString,QString> *favorites, QDateTime *serverDate, mainWindow *parent) : searchTab(id, parent), ui(new Ui::favoritesTab), m_id(id), m_parent(parent), m_serverDate(serverDate), m_favorites(favorites), m_sites(sites), m_pagemax(-1), m_lastTags(QString()), m_sized(false), m_from_history(false), m_stop(true), m_history_cursor(0), m_currentFav(0), m_history(QList<QMap<QString,QString> >()), m_modifiers(QStringList())
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);

	// Search field
	m_postFiltering = new TextEdit(m_favorites->keys(), this);
		m_postFiltering->setContextMenuPolicy(Qt::CustomContextMenu);
		if (settings.value("autocompletion", true).toBool())
		{
			QStringList completion;
			QFile words("words.txt");
			if (words.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				while (!words.atEnd())
				{
					QByteArray line = words.readLine();
					completion.append(QString(line).trimmed().split(" ", QString::SkipEmptyParts));
				}
				for (int i = 0; i < sites->size(); i++)
				{
					if (sites->value(sites->keys().at(i))->contains("Modifiers"))
					{ m_modifiers.append(sites->value(sites->keys().at(i))->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts)); }
				}
				completion.append(m_modifiers);
				completion.append(m_favorites->keys());
				completion.removeDuplicates();
				completion.sort();
				QCompleter *completer = new QCompleter(completion, this);
					completer->setCaseSensitivity(Qt::CaseInsensitive);
				m_postFiltering->setCompleter(completer);
			}
		}
		connect(m_postFiltering, SIGNAL(returnPressed()), this, SLOT(load()));
		ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);

	// Sources
	QString sel = '1'+QString().fill('0',m_sites->count()-1);
	QString sav = settings.value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selectedSources.append(sav.at(i) == '1' ? true : false);
	}

	// Others
	ui->checkMergeResults->setChecked(settings.value("mergeresults", false).toBool());
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	QPalette pal = palette();
		pal.setColor(backgroundRole(), QColor(255,255,255));
		setPalette(pal);
		setAutoFillBackground(true);
	updateCheckboxes();
	updateFavorites();

	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		ui->comboOrder->setCurrentIndex(assoc.indexOf(settings.value("Favorites/order", "name").toString()));
		ui->comboAsc->setCurrentIndex(int(settings.value("Favorites/reverse", false).toBool()));
		settings.setValue("reverse", bool(ui->comboAsc->currentIndex() == 1));
	ui->widgetResults->hide();
}

favoritesTab::~favoritesTab()
{
	close();
	delete ui;
}

void favoritesTab::closeEvent(QCloseEvent *e)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	settings.setValue("mergeresults", ui->checkMergeResults->isChecked());
	settings.beginGroup("Favorites");
		QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		settings.setValue("order", assoc[ui->comboOrder->currentIndex()]);
		settings.setValue("reverse", bool(ui->comboAsc->currentIndex() == 1));
	settings.endGroup();
	settings.sync();

	qDeleteAll(m_pages);
	m_pages.clear();
	qDeleteAll(m_images);
	m_images.clear();
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();
	for (int i = 0; i < m_layouts.size(); i++)
	{ clearLayout(m_layouts[i]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();

	emit(closed(this));
	e->accept();
}



void favoritesTab::updateFavorites()
{
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[ui->comboOrder->currentIndex()];
	bool reverse = (ui->comboAsc->currentIndex() == 1);
	*m_favorites = loadFavorites();
	QStringList keys = m_favorites->keys();
	QList<QMap<QString,QString> > favorites;
	for (int i = 0; i < keys.size(); i++)
	{
		QMap<QString,QString> d;
		QString tag = keys.at(i);
		d["id"] = QString::number(i);
		d["name"] = tag;
		QStringList xp = m_favorites->value(tag).split("|");
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
	clearLayout(ui->layoutFavorites);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QString display = settings.value("favorites_display", "ind").toString();
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
		if (display.contains("i"))
		{
			QBouton *image = new QBouton(favorites[i]["id"].toInt(), this, settings.value("resizeInsteadOfCropping", true).toBool(), QColor(), this);
				image->setIcon(img);
				image->setIconSize(img.size());
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(int)), this, SLOT(favoriteProperties(int)));
				connect(image, SIGNAL(middleClick(int)), this, SLOT(addTabFavorite(int)));
				connect(image, SIGNAL(appui(int)), this, SLOT(loadFavorite(int)));
			ui->layoutFavorites->addWidget(image, (i/10)*2, i%10);
		}
		QAffiche *caption = new QAffiche(favorites[i]["id"].toInt(), 0 ,QColor(), this);
		caption->setText((display.contains("n") ? favorites[i]["name"] : "") + (display.contains("d") ? "<br/>("+favorites[i]["note"]+" % - "+QDateTime::fromString(favorites[i]["lastviewed"], Qt::ISODate).toString(format)+")" : ""));
			caption->setTextFormat(Qt::RichText);
			caption->setAlignment(Qt::AlignCenter);
			caption->setToolTip(xt);
		if (!caption->text().isEmpty())
		{
			connect(caption, SIGNAL(clicked(int)), this, SLOT(loadFavorite(int)));
			ui->layoutFavorites->addWidget(caption, (i/10)*2+1, i%10);
		}
	}
}

void favoritesTab::addTabFavorite(int id)
{
	QString tag = m_favorites->keys().at(id);
	m_parent->addTab(tag);
}

void favoritesTab::optionsChanged()
{
	log(tr("Mise à jour des options de l'onglet \"%1\".").arg(windowTitle()));
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	ui->retranslateUi(this);
	ui->spinImagesPerPage->setValue(settings.value("limit", 20).toInt());
	ui->spinColumns->setValue(settings.value("columns", 1).toInt());
	/*QPalette p = ui->widgetResults->palette();
	p.setColor(ui->widgetResults->backgroundRole(), QColor(settings.value("serverBorderColor", "#000000").toString()));
	ui->widgetResults->setPalette(p);*/
	ui->layoutResults->setHorizontalSpacing(settings.value("Margins/main", 10).toInt());
}



void favoritesTab::openSourcesWindow()
{
	sourcesWindow *adv = new sourcesWindow(m_selectedSources, m_sites, this);
	connect(adv, SIGNAL(valid(QList<bool>)), this, SLOT(saveSources(QList<bool>)));
	adv->show();
}
void favoritesTab::saveSources(QList<bool> sel)
{
	log(tr("Sauvegarde des sources..."));
	m_selectedSources = sel;
	QString sav;
	for (int i = 0; i < m_selectedSources.count(); i++)
	{ sav += (m_selectedSources.at(i) ? "1" : "0"); }
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	settings.setValue("sites", sav);
	DONE();
	updateCheckboxes();
}

void favoritesTab::updateCheckboxes()
{
	log(tr("Mise à jour des cases à cocher."));
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();
	QStringList urls = m_sites->keys();
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	int n = settings.value("Sources/Letters", 3).toInt(), m = n;
	for (int i = 0; i < urls.size(); i++)
	{
		if (urls[i].startsWith("www."))
		{ urls[i] = urls[i].right(urls[i].length() - 4); }
		else if (urls[i].startsWith("chan."))
		{ urls[i] = urls[i].right(urls[i].length() - 5); }
		if (n < 0)
		{
			m = urls.at(i).indexOf('.');
			if (n < -1 && urls.at(i).indexOf('.', m+1) != -1)
			{ m = urls.at(i).indexOf('.', m+1); }
		}
		QCheckBox *c = new QCheckBox(urls.at(i).left(m), this);
			c->setChecked(m_selectedSources.at(i));
			ui->layoutSourcesList->addWidget(c);
		m_checkboxes.append(c);
	}
	DONE();
}



void favoritesTab::load()
{
	log(tr("Chargement des résultats..."));

	m_stop = true;
	m_parent->ui->labelWiki->setText("");
	m_pagemax = -1;

	if (!m_from_history)
	{
		QMap<QString,QString> srch = QMap<QString,QString>();
		srch["tags"] = m_currentTags;
		srch["page"] = QString::number(ui->spinPage->value());
		srch["ipp"] = QString::number(ui->spinImagesPerPage->value());
		srch["columns"] = QString::number(ui->spinColumns->value());
		m_history.append(srch);

		if (m_history.size() > 1)
		{
			m_history_cursor++;
			ui->buttonHistoryBack->setEnabled(true);
			ui->buttonHistoryNext->setEnabled(false);
		}
	}
	m_from_history = false;

	if (m_currentTags != m_lastTags && !m_lastTags.isNull() && m_history_cursor == m_history.size() - 1)
	{ ui->spinPage->setValue(1); }
	m_lastTags = m_currentTags;

	ui->buttonFirstPage->setEnabled(ui->spinPage->value() > 1);
	ui->buttonPreviousPage->setEnabled(ui->spinPage->value() > 1);
	for (int i = 0; i < m_layouts.size(); i++)
	{ clearLayout(m_layouts[i]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();
	clearLayout(ui->layoutResults);

	m_tags = "";
	m_parent->ui->labelTags->setText("");
	for (int i = 0; i < m_pages.size(); i++)
	{
		m_pages.value(m_pages.keys().at(i))->abort();
		m_pages.value(m_pages.keys().at(i))->abortTags();
	}
	//qDeleteAll(m_pages);
	m_pages.clear();
	for (int i = 0; i < m_images.size(); i++)
	{ m_images.at(i)->abortPreview(); }
	//qDeleteAll(m_images);
	m_images.clear();

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	for (int i = 0; i < m_selectedSources.size(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{
			QStringList tags = m_currentTags.toLower().trimmed().split(" ", QString::SkipEmptyParts);
			tags.append(settings.value("add").toString().toLower().trimmed().split(" ", QString::SkipEmptyParts));
			int perpage = ui->spinImagesPerPage->value();
			Page *page = new Page(m_sites->value(m_sites->keys().at(i)), m_sites, tags, ui->spinPage->value(), perpage, m_postFiltering->toPlainText().toLower().split(" ", QString::SkipEmptyParts), true, this);
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
			connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
			m_pages.insert(page->website(), page);
			QGridLayout *l = new QGridLayout;
			l->setHorizontalSpacing(settings.value("Margins/horizontal", 6).toInt());
			l->setVerticalSpacing(settings.value("Margins/vertical", 6).toInt());
			m_layouts.append(l);
			m_stop = false;
			page->load();
			if (settings.value("useregexfortags", true).toBool())
			{
				connect(page, SIGNAL(finishedLoadingTags(Page*)), this, SLOT(finishedLoadingTags(Page*)));
				page->loadTags();
			}
		}
	}
	if (ui->checkMergeResults->isChecked() && m_layouts.size() > 0)
	{ ui->layoutResults->addLayout(m_layouts[0], 0, 0, 1, 1); }
	m_page = 0;

	emit changed(this);
}

void favoritesTab::finishedLoading(Page* page)
{
	if (m_stop)
	{ return; }

	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	QList<Image*> imgs;
	foreach (Image *img, page->images())
	{
		if (img->createdAt() > m_loadFavorite)
		{ imgs.append(img); }
	}
	m_images.append(imgs);
	int perpage = page->site()->value("Urls/Selected/Tags").contains("{limit}") ? ui->spinImagesPerPage->value() : imgs.size();
	int maxpage = ceil(page->imagesCount()/((float)perpage));

	if (maxpage < m_pagemax || m_pagemax == -1)
	{ m_pagemax = maxpage; }
	ui->buttonNextPage->setEnabled(maxpage > ui->spinPage->value() || page->imagesCount() == -1 || (page->imagesCount() == 0 && page->images().count() > 0));
	ui->buttonLastPage->setEnabled(maxpage > ui->spinPage->value());

	if (!ui->checkMergeResults->isChecked())
	{
		int pos = m_pages.values().indexOf(page);
		if (pos < 0)
		{ return; }
		QLabel *txt = new QLabel(this);
			if (imgs.count() == 0)
			{
				QStringList reasons = QStringList();
				if (page->source().isEmpty())
				{ reasons.append(tr("serveur hors-ligne")); }
				if (m_currentTags.count(" ") > 1)
				{ reasons.append(tr("trop de tags")); }
				if (ui->spinPage->value() > 1000)
				{ reasons.append(tr("page trop éloignée")); }
				txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+m_sites->key(page->site())+"</a> - "+tr("Aucun résultat depuis le %1").arg(m_loadFavorite.toString(tr("dd/MM/yyyy 'à' hh:mm")))+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : ""));
			}
			else
			{ txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+m_sites->key(page->site())+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(ui->spinPage->value()).arg(page->imagesCount() > 0 ? QString::number(maxpage) : "?").arg(imgs.count()).arg(page->imagesCount() > 0 ? QString::number(page->imagesCount()) : "?")); }
			txt->setOpenExternalLinks(true);
			if (page->search().join(" ") != m_currentTags && settings.value("showtagwarning", true).toBool())
			{
				QStringList uncommon = m_currentTags.toLower().trimmed().split(" ", QString::SkipEmptyParts);
				uncommon.append(settings.value("add").toString().toLower().trimmed().split(" ", QString::SkipEmptyParts));
				for (int i = 0; i < page->search().size(); i++)
				{
					if (uncommon.contains(page->search().at(i)))
					{ uncommon.removeAll(page->search().at(i)); }
				}
				if (!uncommon.isEmpty())
				{ txt->setText(txt->text()+"<br/>"+QString(tr("Des modificateurs ont été otés de la recherche car ils ne sont pas compatibles avec cet imageboard : %1.")).arg(uncommon.join(" "))); }
			}
			if (!page->errors().isEmpty() && settings.value("showwarnings", true).toBool())
			{ txt->setText(txt->text()+"<br/>"+page->errors().join("<br/>")); }
		int page_x = pos % ui->spinColumns->value(), page_y = (pos / ui->spinColumns->value()) * 2;
		ui->layoutResults->addWidget(txt, page_y, page_x, 1, 1);
		ui->layoutResults->setRowMinimumHeight(page_y, height()/20);
		if (m_layouts.size() > pos)
		{ ui->layoutResults->addLayout(m_layouts[pos], page_y + 1, page_x, 1, 1); }
		if (imgs.count() >= settings.value("hidefavorites", 20).toInt())
		{ ui->widgetFavorites->hide(); }
	}

	if (!settings.value("useregexfortags", true).toBool())
	{
		// Tags for this page
		QList<Tag> taglist;
		QStringList tagsGot;
		for (int i = 0; i < m_pages.count(); i++)
		{
			QList<Tag> tags = m_pages.value(m_pages.keys().at(i))->tags();
			for (int t = 0; t < tags.count(); t++)
			{
				if (!tags[t].text().isEmpty())
				{
					if (tagsGot.contains(tags[t].text()))
					{ taglist[tagsGot.indexOf(tags[t].text())].setCount(taglist[tagsGot.indexOf(tags[t].text())].count()+tags[t].count()); }
					else
					{
						taglist.append(tags[t]);
						tagsGot.append(tags[t].text());
					}
				}
			}
		}

		// We sort tags by frequency
		qSort(taglist.begin(), taglist.end(), sortByFrequency);

		// Then we show them, styled if possible
		QStringList tlist = QStringList() << "artists" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "blacklisteds";
		QStringList defaults = QStringList() << "#aa0000" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000";
		QMap<QString,QString> styles;
		QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
		for (int i = 0; i < tlist.size(); i++)
		{
			QFont font;
			font.fromString(settings.value("Coloring/Fonts/"+tlist.at(i)).toString());
			styles[tlist.at(i)] = "color:"+settings.value("Coloring/Colors/"+tlist.at(i), defaults.at(i)).toString()+"; "+qfonttocss(font);
		}
		QString tags;
		int last = 0, h = height()/10;
		for (int i = 0; i < taglist.size(); i++)
		{
			if (i < h || last == taglist[i].count())
			{
				last = taglist[i].count();
				if (m_favorites->contains(taglist[i].text()))
				{ taglist[i].setType("favorite"); }
				QString n = taglist[i].text();
				n.replace(" ", "_");
				tags += "<a href=\""+n+"\" style=\""+(styles.contains(taglist[i].type()+"s") ? styles[taglist[i].type()+"s"] : styles["generals"])+"\">"+taglist[i].text()+"</a>"+(taglist[i].count() > 0 ? " ("+QString::number(taglist[i].count())+")" : "")+"<br/>";
			}
		}

		m_tags = tags;
		m_parent->ui->labelTags->setText(tags);
	}

	m_page++;
	if (ui->checkMergeResults->isChecked())
	{
		if (m_page != m_pages.size())
		{ return; }
		QStringList md5s;
		for (int i = 0; i < m_images.count(); i++)
		{
			if (md5s.contains(m_images.at(i)->md5()))
			{ m_images.removeAt(i); i--; }
			else
			{ md5s.append(m_images.at(i)->md5()); }
		}
		imgs = m_images;
	}

	// Loading images
	for (int i = 0; i < imgs.count(); i++)
	{
		QStringList detected;
		Image *img = imgs.at(i);
		QStringList tags = m_currentTags.split(' ');
		QList<QChar> modifiers = QList<QChar>() << '~';
		for (int r = 0; r < tags.size(); r++)
		{
			if (modifiers.contains(tags[r][0]))
			{ tags[r] = tags[r].right(tags[r].size()-1); }
		}
		if (!settings.value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(" "));
			for (int t = 0; t < img->tags().count(); t++)
			{
				if (blacklistedtags.contains(img->tags()[t].text(), Qt::CaseInsensitive) && !tags.contains(img->tags()[t].text(), Qt::CaseInsensitive))
				{ detected.append(img->tags()[t].text().toLower()); }
			}
		}
		if (!detected.isEmpty() && settings.value("hideblacklisted", false).toBool())
		{ log(tr("Image #%1 ignorée. Raison : %2.").arg(i).arg("\""+detected.join(", ")+"\""));; }
		else
		{
			connect(img, SIGNAL(finishedLoadingPreview(Image*)), this, SLOT(finishedLoadingPreview(Image*)));
			img->loadPreview();
		}
	}
}

void favoritesTab::finishedLoadingTags(Page *page)
{
	// Tags for this page
	QList<Tag> taglist;
	QStringList tagsGot;
	for (int i = 0; i < m_pages.count(); i++)
	{
		QList<Tag> tags = m_pages.value(m_pages.keys().at(i))->tags();
		for (int t = 0; t < tags.count(); t++)
		{
			if (!tags[t].text().isEmpty())
			{
				if (tagsGot.contains(tags[t].text()))
				{ taglist[tagsGot.indexOf(tags[t].text())].setCount(taglist[tagsGot.indexOf(tags[t].text())].count()+tags[t].count()); }
				else
				{
					taglist.append(tags[t]);
					tagsGot.append(tags[t].text());
				}
			}
		}
	}

	// We sort tags by frequency
	qSort(taglist.begin(), taglist.end(), sortByFrequency);

	// Then we show them, styled if possible
	QStringList tlist = QStringList() << "artists" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "blacklisteds";
	QStringList defaults = QStringList() << "#aa0000" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000";
	QMap<QString,QString> styles;
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	for (int i = 0; i < tlist.size(); i++)
	{
		QFont font;
		font.fromString(settings.value("Coloring/Fonts/"+tlist.at(i)).toString());
		styles[tlist.at(i)] = "color:"+settings.value("Coloring/Colors/"+tlist.at(i), defaults.at(i)).toString()+"; "+qfonttocss(font);
	}
	QString tags;
	for (int i = 0; i < taglist.count(); i++)
	{
		if (m_favorites->contains(taglist[i].text()))
		{ taglist[i].setType("favorite"); }
		QString n = taglist[i].text();
		n.replace(" ", "_");
		tags += "<a href=\""+n+"\" style=\""+(styles.contains(taglist[i].type()+"s") ? styles[taglist[i].type()+"s"] : styles["generals"])+"\">"+taglist[i].text()+"</a>"+(taglist[i].count() > 0 ? " ("+QString::number(taglist[i].count())+")" : "")+"<br/>";
	}

	m_tags = tags;
	m_parent->ui->labelTags->setText(tags);
	if (!page->wiki().isEmpty())
	{
		m_wiki = "<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+page->wiki();
		m_parent->ui->labelWiki->setText(m_wiki);
	}
}

void favoritesTab::finishedLoadingPreview(Image *img)
{
	if (m_stop)
	{ return; }

	int position = m_images.indexOf(img), page = 0;
	if (!ui->checkMergeResults->isChecked())
	{ page = m_pages.values().indexOf(img->page()); }
	if (img->previewImage().isNull())
	{
		log(tr("<b>Attention :</b> %1").arg(tr("une des miniatures est vide (<a href=\"%1\">%1</a>).").arg(img->previewUrl().toString())));
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

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	QColor color;
	if (img->status() == "pending")
	{ color = QColor("#0000ff"); }
	if (img->parentId() != 0)
	{ color = QColor("#cccc00"); }
	if (img->hasChildren())
	{ color = QColor("#00ff00"); }
	for (int i = 0; i < img->tags().count(); i++)
	{
		if (m_favorites->keys().contains(img->tags()[i].text()) && !m_currentTags.trimmed().split(" ").contains(img->tags()[i].text()))
		{ color = QColor("#ffc0cb"); break; }
	}
	QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(" "));
	QStringList detected = img->blacklisted(blacklistedtags);
	if (!detected.isEmpty())
	{ color = QColor("#000000"); }
	QBouton *l = new QBouton(position, settings.value("resizeInsteadOfCropping", true).toBool(), settings.value("borders", 3).toInt(), color, this);
		l->setIcon(img->previewImage());
		l->setCheckable(true);
		l->setChecked(m_selectedImages.contains(img->url()));
		QString t;
		for (int i = 0; i < img->tags().count(); i++)
		{ t += " "+img->tags()[i].stylished(m_favorites->keys()); }
		l->setToolTip(QString("%1%2%3%4%5%6%7%8")
			.arg(img->tags().isEmpty() ? " " : tr("<b>Tags :</b> %1<br/><br/>").arg(t.trimmed()))
			.arg(img->id() == 0 ? " " : tr("<b>ID :</b> %1<br/>").arg(img->id()))
			.arg(img->rating().isEmpty() ? " " : tr("<b>Classe :</b> %1<br/>").arg(img->rating()))
			.arg(img->hasScore() ? tr("<b>Score :</b> %1<br/>").arg(img->score()) : " ")
			.arg(img->author().isEmpty() ? " " : tr("<b>Posteur :</b> %1<br/><br/>").arg(img->author()))
			.arg(img->width() == 0 || img->height() == 0 ? " " : tr("<b>Dimensions :</b> %1 x %2<br/>").arg(QString::number(img->width()), QString::number(img->height())))
			.arg(img->fileSize() == 0 ? " " : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(size), unit))
			.arg(!img->createdAt().isValid() ? " " : tr("<b>Date :</b> %1").arg(img->createdAt().toString(tr("'le 'dd/MM/yyyy' à 'hh:mm"))))
		);
		l->setIconSize(img->previewImage().size());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(toggled(int,bool)), this, SLOT(toggleImage(int,bool)));
		connect(l, SIGNAL(rightClick(int)), _mainwindow, SLOT(batchChange(int)));
	int perpage = img->page()->site()->value("Urls/Selected/Tags").contains("{limit}") ? ui->spinImagesPerPage->value() : img->page()->images().size();
	perpage = perpage > 0 ? perpage : 20;
	int pp = perpage;
	if (ui->checkMergeResults->isChecked())
	{ pp = m_images.count(); }
	int pl = ceil(sqrt((double)pp));
	if (m_layouts.size() > page)
	{ m_layouts[page]->addWidget(l, floor(float(position % pp) / pl), position % pl); }
}

void favoritesTab::webZoom(int id)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	Image *image = m_images.at(id);

	if (!settings.value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(" "));
		QStringList detected = image->blacklisted(blacklistedtags);
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(m_parent, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}

	zoomWindow *zoom = new zoomWindow(image, image->page()->site(), m_sites, this);
	zoom->show();
	connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
}

void favoritesTab::toggleImage(int id, bool toggle)
{
	if (toggle)
	{ selectImage(m_images.at(id)); }
	else
	{ unselectImage(m_images.at(id)); }
}

void favoritesTab::setTags(QString tags)
{
	activateWindow();
	m_currentTags = tags;
	load();
}

void favoritesTab::getPage()
{
	QStringList actuals, keys = m_sites->keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{ actuals.append(keys.at(i)); }
	}
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	bool unloaded = settings.value("getunloadedpages", false).toBool();
	for (int i = 0; i < actuals.count(); i++)
	{
		int perpage = unloaded ? ui->spinImagesPerPage->value() : m_pages.value(actuals.at(i))->images().count();
		emit batchAddGroup(QStringList() << m_currentTags+" "+settings.value("add").toString().toLower().trimmed() << QString::number(ui->spinPage->value()) << QString::number(perpage) << QString::number(perpage) << settings.value("downloadblacklist").toString() << actuals.at(i) << settings.value("Save/filename").toString() << settings.value("Save/path").toString() << "");
	}
}
void favoritesTab::getAll()
{
	QStringList actuals, keys = m_sites->keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{ actuals.append(keys.at(i)); }
	}
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	for (int i = 0; i < actuals.count(); i++)
	{
		int limit = m_sites->value(actuals.at(i))->contains("Urls/1/Limit") ? m_sites->value(actuals.at(i))->value("Urls/1/Limit").toInt() : 0;
		emit batchAddGroup(QStringList() << m_currentTags+" "+settings.value("add").toString().toLower().trimmed() << "1" << QString::number(qMin((limit > 0 ? limit : 1000), qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount()))) << QString::number(qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount())) << settings.value("downloadblacklist").toString() << actuals.at(i) << settings.value("Save/filename").toString() << settings.value("Save/path").toString() << "");
	}
}
void favoritesTab::getSel()
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	foreach (Image *img, m_selectedImagesPtrs)
	{
		QStringList tags;
		foreach (Tag tag, img->tags())
		{ tags.append(tag.text()); }

		QMap<QString,QString> values;
		values.insert("id", QString::number(img->id()));
		values.insert("md5", img->md5());
		values.insert("rating", img->rating());
		values.insert("tags", tags.join(" "));
		values.insert("file_url", img->fileUrl().toString());
		values.insert("site", img->site());
		values.insert("filename", settings.value("Save/filename").toString());
		values.insert("folder", settings.value("Save/path").toString());

		values.insert("page_url", m_sites->value(img->site())->value("Urls/Html/Post"));
		QString t = m_sites->value(img->site())->contains("DefaultTag") ? m_sites->value(img->site())->value("DefaultTag") : "";
		values["page_url"].replace("{tags}", t);
		values["page_url"].replace("{id}", values["id"]);

		emit batchAddUnique(values);
	}
}

void favoritesTab::firstPage()
{
	ui->spinPage->setValue(1);
	load();
}
void favoritesTab::previousPage()
{
	if (ui->spinPage->value() > 1)
	{
		ui->spinPage->setValue(ui->spinPage->value()-1);
		load();
	}
}
void favoritesTab::nextPage()
{
	if (ui->spinPage->value() < ui->spinPage->maximum())
	{
		ui->spinPage->setValue(ui->spinPage->value()+1);
		load();
	}
}
void favoritesTab::lastPage()
{
	ui->spinPage->setValue(m_pagemax);
	load();
}



void favoritesTab::linkHovered(QString url)
{ m_link = url; }
void favoritesTab::linkClicked(QString url)
{
	if (Qt::ControlModifier)
	{ _mainwindow->addTab(url); }
	else
	{
		m_currentTags = url;
		load();
	}
}
void favoritesTab::openInNewTab()
{ _mainwindow->addTab(m_link); }
void favoritesTab::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(m_link));
}

QList<bool> favoritesTab::sources()
{ return m_selectedSources; }

void favoritesTab::historyBack()
{
	if (m_history_cursor > 0)
	{
		m_from_history = true;
		m_history_cursor--;

		ui->spinPage->setValue(m_history[m_history_cursor]["page"].toInt());
		ui->spinImagesPerPage->setValue(m_history[m_history_cursor]["ipp"].toInt());
		ui->spinColumns->setValue(m_history[m_history_cursor]["columns"].toInt());
		setTags(m_history[m_history_cursor]["tags"]);

		ui->buttonHistoryNext->setEnabled(true);
		if (m_history_cursor == 0)
		{ ui->buttonHistoryBack->setEnabled(false); }
	}
}
void favoritesTab::historyNext()
{
	if (m_history_cursor < m_history.size() - 1)
	{
		m_from_history = true;
		m_history_cursor++;

		ui->spinPage->setValue(m_history[m_history_cursor]["page"].toInt());
		ui->spinImagesPerPage->setValue(m_history[m_history_cursor]["ipp"].toInt());
		ui->spinColumns->setValue(m_history[m_history_cursor]["columns"].toInt());
		setTags(m_history[m_history_cursor]["tags"]);

		ui->buttonHistoryBack->setEnabled(true);
		if (m_history_cursor == m_history.size() - 1)
		{ ui->buttonHistoryNext->setEnabled(false); }
	}
}

QString favoritesTab::tags()	{ return m_currentTags;	}
QString favoritesTab::results()	{ return m_tags;		}
QString favoritesTab::wiki()	{ return m_wiki;		}

void favoritesTab::loadFavorite(int id)
{
	ui->widgetResults->show();
	QString tag = m_favorites->keys().at(id);
	m_currentTags = tag;
	m_loadFavorite = QDateTime::fromString(m_favorites->value(tag).section('|', 1, 1), Qt::ISODate);
	load();
}
void favoritesTab::checkFavorites()
{
	ui->widgetFavorites->show();
	m_currentFav = -1;
	m_currentTags = QString();
	loadNextFavorite();
}
void favoritesTab::loadNextFavorite()
{
	if (m_currentFav+1 >= m_favorites->size())
	{ return; }
	else
	{
		m_currentFav++;
		m_currentTags = m_favorites->keys().at(m_currentFav);
		m_loadFavorite = QDateTime::fromString(m_favorites->value(m_currentTags).section('|', 1, 1), Qt::ISODate);
		load();
	}
}
void favoritesTab::viewed()
{
	if (m_currentTags.isEmpty())
	{
		int reponse = QMessageBox::question(this, tr("Marquer comme vu"), tr("Êtes-vous sûr de vouloir marquer tous vos favoris comme vus ?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			QStringList keys = m_favorites->keys();
			for (int i = 0; i < keys.count(); i++)
			{ setFavoriteViewed(keys.at(i)); }
		}
	}
	else
	{ setFavoriteViewed(m_currentTags); }
	updateFavorites();
	m_parent->updateFavoritesDock();
}
void favoritesTab::setFavoriteViewed(QString tag)
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
void favoritesTab::favoritesBack()
{
	ui->widgetResults->hide();
	ui->widgetFavorites->show();
	if (!m_currentTags.isEmpty() || m_currentFav != -1)
	{
		m_currentTags = "";
		m_currentFav = -1;
		ui->widgetFavorites->show();
		/*if (!m_replies.isEmpty() && m_currentFav < 1)
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
		}*/
	}
}
void favoritesTab::favoriteProperties(int id)
{
	if (id == -1)
	{ id = m_currentFav; }
	QString tag = m_favorites->keys().at(id);
	QStringList xp = m_favorites->value(tag).split("|");
	int note = xp.isEmpty() ? 50 : xp.takeFirst().toInt();
	QDateTime lastviewed = xp.isEmpty() ? QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0, 0)) : QDateTime::fromString(xp.takeFirst(), Qt::ISODate);
	favoriteWindow *fwin = new favoriteWindow(tag, note, lastviewed, this);
	connect(fwin, SIGNAL(favoritesChanged()), this, SLOT(updateFavorites()));
	fwin->show();
}

void favoritesTab::setImagesPerPage(int ipp)
{ ui->spinImagesPerPage->setValue(ipp); }
void favoritesTab::setColumns(int columns)
{ ui->spinColumns->setValue(columns); }
void favoritesTab::setPostFilter(QString postfilter)
{ m_postFiltering->setText(postfilter); }

int favoritesTab::imagesPerPage()	{ return ui->spinImagesPerPage->value();	}
int favoritesTab::columns()			{ return ui->spinColumns->value();			}
QString favoritesTab::postFilter()	{ return m_postFiltering->toPlainText();	}
