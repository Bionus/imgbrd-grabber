#include "tagtab.h"
#include "ui_tagtab.h"
#include "ui_mainwindow.h"
#include "QBouton.h"
#include "zoomwindow.h"
#include "searchwindow.h"

#define DONE()	logUpdate(QObject::tr(" Fait"))
extern mainWindow *_mainwindow;



tagTab::tagTab(int id, QMap<QString,QMap<QString,QString> > *sites, QMap<QString,QString> *favorites, QDateTime *serverDate, mainWindow *parent) : searchTab(id, parent), ui(new Ui::tagTab), m_id(id), m_parent(parent), m_serverDate(serverDate), m_favorites(favorites), m_sites(sites), m_pagemax(-1), m_lastTags(QString()), m_sized(false), m_from_history(false), m_stop(true), m_history_cursor(0), m_history(QList<QMap<QString,QString> >()), m_modifiers(QStringList())
{
	ui->setupUi(this);
	ui->widgetMeant->hide();
	setAttribute(Qt::WA_DeleteOnClose);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);

	// Search field
	m_search = new TextEdit(m_favorites->keys(), this);
	m_postFiltering = new TextEdit(m_favorites->keys(), this);
		m_search->setContextMenuPolicy(Qt::CustomContextMenu);
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
					if (sites->value(sites->keys().at(i)).contains("Modifiers"))
					{ m_modifiers.append(sites->value(sites->keys().at(i)).value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts)); }
				}
				completion.append(m_modifiers);
				completion.append(m_favorites->keys());
				completion.removeDuplicates();
				completion.sort();
				QCompleter *completer = new QCompleter(completion, this);
					completer->setCaseSensitivity(Qt::CaseInsensitive);
				m_search->setCompleter(completer);
				m_postFiltering->setCompleter(completer);
			}
		}
		connect(m_search, SIGNAL(returnPressed()), this, SLOT(load()));
		connect(m_search, SIGNAL(favoritesChanged()), _mainwindow, SLOT(updateFavorites()));
		connect(m_search, SIGNAL(favoritesChanged()), _mainwindow, SLOT(updateFavoritesDock()));
		connect(m_search, SIGNAL(kflChanged()), _mainwindow, SLOT(updateKeepForLater()));
		connect(m_postFiltering, SIGNAL(returnPressed()), this, SLOT(load()));
		connect(ui->labelMeant, SIGNAL(linkActivated(QString)), this, SLOT(setTags(QString)));
		ui->layoutFields->insertWidget(1, m_search, 1);
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
	m_search->setFocus();
}

tagTab::~tagTab()
{
	close();
	delete ui;
}

void tagTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), m_serverDate->date(), this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void tagTab::closeEvent(QCloseEvent *e)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	settings.setValue("mergeresults", ui->checkMergeResults->isChecked());
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

	emit closed(this);
	e->accept();
}



void tagTab::optionsChanged()
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



void tagTab::openSourcesWindow()
{
	sourcesWindow *adv = new sourcesWindow(m_selectedSources, m_sites, this);
	connect(adv, SIGNAL(valid(QList<bool>)), this, SLOT(saveSources(QList<bool>)));
	adv->show();
}
void tagTab::saveSources(QList<bool> sel)
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

void tagTab::updateCheckboxes()
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



void tagTab::load()
{
	log(tr("Chargement des résultats..."));

	m_stop = true;
	m_parent->ui->labelWiki->setText("");
	m_pagemax = -1;

	if (!m_from_history)
	{
		QMap<QString,QString> srch = QMap<QString,QString>();
		srch["tags"] = m_search->toPlainText();
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

	if (m_search->toPlainText() != m_lastTags && !m_lastTags.isNull() && m_history_cursor == m_history.size() - 1)
	{ ui->spinPage->setValue(1); }
	m_lastTags = m_search->toPlainText();

	ui->widgetMeant->hide();
	ui->buttonFirstPage->setEnabled(ui->spinPage->value() > 1);
	ui->buttonPreviousPage->setEnabled(ui->spinPage->value() > 1);
	for (int i = 0; i < m_layouts.size(); i++)
	{ clearLayout(m_layouts[i]); }
	qDeleteAll(m_layouts);
	m_layouts.clear();
	clearLayout(ui->layoutResults);
	setWindowTitle(m_search->toPlainText().isEmpty() ? tr("Recherche") : m_search->toPlainText().replace("&", "&&"));
	qDebug() << "titleChanged" << this->windowTitle();
	emit titleChanged(this);
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
			QStringList tags = m_search->toPlainText().toLower().trimmed().split(" ", QString::SkipEmptyParts);
			tags.append(settings.value("add").toString().toLower().trimmed().split(" ", QString::SkipEmptyParts));
			int perpage = ui->spinImagesPerPage->value();
			Page *page = new Page(m_sites, m_sites->keys().at(i), tags, ui->spinPage->value(), perpage, m_postFiltering->toPlainText().toLower().split(" ", QString::SkipEmptyParts), true, this);
			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(Qt::escape(page->url().toString())));
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

void tagTab::finishedLoading(Page* page)
{
	if (m_stop)
	{ return; }

	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(Qt::escape(page->url().toString())));

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat, this);
	QList<Image*> imgs = page->images();
	m_images.append(imgs);
	int perpage = page->site().value("Urls/Selected/Tags").contains("{limit}") ? ui->spinImagesPerPage->value() : imgs.size();
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
				if (m_search->toPlainText().count(" ") > 1)
				{ reasons.append(tr("trop de tags")); }
				if (ui->spinPage->value() > 1000)
				{ reasons.append(tr("page trop éloignée")); }
				QStringList completion;
				QFile words("words.txt");
				if (words.open(QIODevice::ReadOnly | QIODevice::Text) && !m_search->toPlainText().isEmpty())
				{
					while (!words.atEnd())
					{
						QByteArray line = words.readLine();
						completion.append(QString(line).remove("\r\n").remove("\n").split(" ", QString::SkipEmptyParts));
					}
					completion.append(m_favorites->keys());
					completion.removeDuplicates();
					completion.sort();
					QStringList tags = m_search->toPlainText().trimmed().split(" ");
					QMap<QString,QString> results, clean;
					int c = 0;
					for (int t = 0; t < tags.size(); t++)
					{
						QString tag = tags.at(t);
						int lev = (tag.length()/3)+2;
						for (int w = 0; w < completion.size(); w++)
						{
							int d = levenshtein(tag, completion.at(w));
							if (d < lev)
							{
								if (results[tag].isEmpty())
								{ c++; }
								results[tag] = "<b>"+completion.at(w)+"</b>";
								clean[tag] = completion.at(w);
								lev = d;
							}
						}
						if (lev == 0)
						{ results[tag] = tag; c--; }
					}
					if (c > 0)
					{
						QStringList res = results.values(), cl = clean.values();
						ui->widgetMeant->show();
						ui->labelMeant->setText("<a href=\""+Qt::escape(cl.join(" "))+"\" style=\"color:black;text-decoration:none;\">"+res.join(" ")+"</a>");
					}
					words.close();
				}
				txt->setText("<a href=\""+Qt::escape(page->url().toString())+"\">"+m_sites->key(page->site())+"</a> - "+tr("Aucun résultat")+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : ""));
			}
			else
			{ txt->setText("<a href=\""+Qt::escape(page->url().toString())+"\">"+m_sites->key(page->site())+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(ui->spinPage->value()).arg(page->imagesCount() > 0 ? QString::number(maxpage) : "?").arg(imgs.count()).arg(page->imagesCount() > 0 ? QString::number(page->imagesCount()) : "?")); }
			txt->setOpenExternalLinks(true);
			if (page->search().join(" ") != m_search->toPlainText() && settings.value("showtagwarning", true).toBool())
			{
				QStringList uncommon = m_search->toPlainText().toLower().trimmed().split(" ", QString::SkipEmptyParts);
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
		QStringList tags = m_search->toPlainText().split(' ');
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

void tagTab::finishedLoadingTags(Page *page)
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

void tagTab::finishedLoadingPreview(Image *img)
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
		if (m_favorites->keys().contains(img->tags()[i].text()) && !m_search->toPlainText().trimmed().split(" ").contains(img->tags()[i].text()))
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
			.arg(img->fileSize() == 0 ? " " : tr("<b>Taille :</b> %1 %2<br/>").arg(QString::number(round(size)), unit))
			.arg(!img->createdAt().isValid() ? " " : tr("<b>Date :</b> %1").arg(img->createdAt().toString(tr("'le 'dd/MM/yyyy' à 'hh:mm"))))
		);
		l->setIconSize(img->previewImage().size());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(toggled(int,bool)), this, SLOT(toggleImage(int,bool)));
		connect(l, SIGNAL(rightClick(int)), _mainwindow, SLOT(batchChange(int)));
	int perpage = img->page()->site().value("Urls/Selected/Tags").contains("{limit}") ? ui->spinImagesPerPage->value() : img->page()->images().size();
	perpage = perpage > 0 ? perpage : 20;
	int pp = perpage;
	if (ui->checkMergeResults->isChecked())
	{ pp = m_images.count(); }
	int pl = ceil(sqrt(pp));
	if (m_layouts.size() > page)
	{ m_layouts[page]->addWidget(l, floor(float(position % pp) / pl), position % pl); }
}

void tagTab::webZoom(int id)
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

void tagTab::toggleImage(int id, bool toggle)
{
	if (toggle)
	{ selectImage(m_images.at(id)); }
	else
	{ unselectImage(m_images.at(id)); }
}

void tagTab::setTags(QString tags)
{
	activateWindow();
	m_search->setText(tags);
	load();
}

void tagTab::getPage()
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
		emit batchAddGroup(QStringList() << m_search->toPlainText()+" "+settings.value("add").toString().toLower().trimmed() << QString::number(ui->spinPage->value()) << QString::number(perpage) << QString::number(perpage) << settings.value("downloadblacklist").toString() << actuals.at(i) << settings.value("Save/filename").toString() << settings.value("Save/path").toString() << "");
	}
}
void tagTab::getAll()
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
		int limit = m_sites->value(actuals.at(i)).contains("Urls/1/Limit") ? m_sites->value(actuals.at(i)).value("Urls/1/Limit").toInt() : 0;
		emit batchAddGroup(QStringList() << m_search->toPlainText()+" "+settings.value("add").toString().toLower().trimmed() << "1" << QString::number(qMin((limit > 0 ? limit : 1000), qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount()))) << QString::number(qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount())) << settings.value("downloadblacklist").toString() << actuals.at(i) << settings.value("Save/filename").toString() << settings.value("Save/path").toString() << "");
	}
}
void tagTab::getSel()
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

		values.insert("page_url", m_sites->value(img->site())["Urls/Html/Post"]);
		QString t = m_sites->value(img->site()).contains("DefaultTag") ? m_sites->value(img->site())["DefaultTag"] : "";
		values["page_url"].replace("{tags}", t);
		values["page_url"].replace("{id}", values["id"]);

		emit batchAddUnique(values);
	}
}

void tagTab::firstPage()
{
	ui->spinPage->setValue(1);
	load();
}
void tagTab::previousPage()
{
	if (ui->spinPage->value() > 1)
	{
		ui->spinPage->setValue(ui->spinPage->value()-1);
		load();
	}
}
void tagTab::nextPage()
{
	if (ui->spinPage->value() < ui->spinPage->maximum())
	{
		ui->spinPage->setValue(ui->spinPage->value()+1);
		load();
	}
}
void tagTab::lastPage()
{
	ui->spinPage->setValue(m_pagemax);
	load();
}



void tagTab::linkHovered(QString url)
{ m_link = url; }
void tagTab::linkClicked(QString url)
{
	if (Qt::ControlModifier)
	{ _mainwindow->addTab(url); }
	else
	{
		m_search->setPlainText(url);
		load();
	}
}
void tagTab::contextMenu()
{
	QMenu *menu = new QMenu(this);
	if (!this->m_link.isEmpty())
	{
		if (m_favorites->contains(m_link))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Retirer des favoris"), this, SLOT(unfavorite())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Ajouter aux favoris"), this, SLOT(favorite())); }
		QStringList vil = loadViewItLater();
		if (vil.contains(m_link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Ne pas garder pour plus tard"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Garder pour plus tard"), this, SLOT(viewitlater())); }
		menu->addSeparator();
		menu->addAction(QIcon(":/images/icons/tab.png"), tr("Ouvrir dans un nouvel onglet"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}
void tagTab::openInNewTab()
{ _mainwindow->addTab(m_link); }
void tagTab::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(m_link));
}
void tagTab::favorite()
{
	QString v = "50|"+QDateTime::currentDateTime().toString(Qt::ISODate);
	m_favorites->insert(m_link, v);
	QFile f(savePath("favorites.txt"));
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(m_link+"|"+v+"\r\n").toUtf8());
	f.close();
	/*QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	if (!QDir(savePath("thumbs")).exists())
	{ QDir(savePath()).mkdir("thumbs"); }
	img.save(savePath("thumbs/"+m_link+".png"), "PNG");*/
	_mainwindow->updateFavorites();
}
void tagTab::unfavorite()
{
	m_favorites->remove(m_link);
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(m_link+"\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);
	f.open(QIODevice::WriteOnly);
		f.write(favs.toUtf8());
	f.close();
	if (QFile::exists(savePath("thumbs/"+m_link+".png")))
	{ QFile::remove(savePath("thumbs/"+m_link+".png")); }
	_mainwindow->updateFavorites();
}
void tagTab::viewitlater()
{
	QStringList vil = loadViewItLater();
	vil.append(m_link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(vil.join("\r\n").toUtf8());
	f.close();
}
void tagTab::unviewitlater()
{
	QStringList vil = loadViewItLater();
	vil.removeAll(m_link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(vil.join("\r\n").toUtf8());
	f.close();
}

QList<bool> tagTab::sources()
{ return m_selectedSources; }

void tagTab::historyBack()
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
void tagTab::historyNext()
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

QString tagTab::tags()		{ return m_search->toPlainText();	}
QString tagTab::results()	{ return m_tags;					}
QString tagTab::wiki()		{ return m_wiki;					}
