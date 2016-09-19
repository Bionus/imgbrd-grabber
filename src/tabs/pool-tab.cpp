#include <QMessageBox>
#include <QMenu>
#include "pool-tab.h"
#include "ui_pool-tab.h"
#include "ui/QBouton.h"
#include "viewer/zoomwindow.h"
#include "searchwindow.h"
#include "mainwindow.h"


poolTab::poolTab(int id, QMap<QString,Site*> *sites, Profile &profile, mainWindow *parent)
	: searchTab(id, sites, profile, parent), ui(new Ui::poolTab), m_id(id), m_pagemax(-1), m_lastTags(QString()), m_sized(false), m_from_history(false), m_stop(true), m_history_cursor(0), m_history(QList<QMap<QString,QString> >()), m_modifiers(QStringList())
{
	m_favorites = profile.getFavorites();
	ui->setupUi(this);
	ui->widgetMeant->hide();
	setAttribute(Qt::WA_DeleteOnClose);

	QStringList sources = m_sites->keys();
	for (QString source : sources)
	{ ui->comboSites->addItem(source); }

	// Search field
	QStringList favs;
	for (Favorite fav : m_favorites)
		favs.append(fav.getName());
	m_search = new TextEdit(favs, this);
	m_postFiltering = new TextEdit(favs, this);
		m_search->setContextMenuPolicy(Qt::CustomContextMenu);
		m_postFiltering->setContextMenuPolicy(Qt::CustomContextMenu);
		if (m_settings->value("autocompletion", true).toBool())
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
				for (int i = 0; i < m_sites->size(); i++)
				{
					if (m_sites->value(m_sites->keys().at(i))->contains("Modifiers"))
					{ m_modifiers.append(m_sites->value(m_sites->keys().at(i))->value("Modifiers").trimmed().split(" ", QString::SkipEmptyParts)); }
				}
				completion.append(m_modifiers);
				completion.append(favs);
				completion.removeDuplicates();
				completion.sort();
				QCompleter *completer = new QCompleter(completion, this);
					completer->setCaseSensitivity(Qt::CaseInsensitive);
				m_search->setCompleter(completer);
				m_postFiltering->setCompleter(completer);
			}
		}
		connect(m_search, SIGNAL(returnPressed()), this, SLOT(load()));
		connect(m_search, SIGNAL(favoritesChanged()), m_parent, SLOT(updateFavorites()));
		connect(m_search, SIGNAL(favoritesChanged()), m_parent, SLOT(updateFavoritesDock()));
		connect(m_search, SIGNAL(kflChanged()), m_parent, SLOT(updateKeepForLater()));
		connect(m_postFiltering, SIGNAL(returnPressed()), this, SLOT(load()));
		connect(ui->labelMeant, SIGNAL(linkActivated(QString)), this, SLOT(setTags(QString)));
		ui->layoutFields->insertWidget(3, m_search, 1);
		ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);

	// Sources
	QString sel = '1'+QString().fill('0',m_sites->count()-1);
	QString sav = m_settings->value("sites", sel).toString();
	for (int i = 0; i < sel.count(); i++)
	{
		if (sav.count() <= i)
		{ sav[i] = '0'; }
		m_selectedSources.append(sav.at(i) == '1' ? true : false);
	}

	// Others
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	updateCheckboxes();
	m_search->setFocus();
}

poolTab::~poolTab()
{
	close();
	delete ui;
}

void poolTab::on_buttonSearch_clicked()
{
	SearchWindow *sw = new SearchWindow(m_search->toPlainText(), this);
	connect(sw, SIGNAL(accepted(QString)), this, SLOT(setTags(QString)));
	sw->show();
}

void poolTab::closeEvent(QCloseEvent *e)
{
	qDeleteAll(m_pages);
	m_pages.clear();
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



void poolTab::optionsChanged()
{
	log(tr("Mise à jour des options de l'onglet \"%1\".").arg(windowTitle()));
	ui->retranslateUi(this);
	ui->spinImagesPerPage->setValue(m_settings->value("limit", 20).toInt());
	ui->spinColumns->setValue(m_settings->value("columns", 1).toInt());
	/*QPalette p = ui->widgetResults->palette();
	p.setColor(ui->widgetResults->backgroundRole(), QColor(m_settings->value("serverBorderColor", "#000000").toString()));
	ui->widgetResults->setPalette(p);*/
	ui->layoutResults->setHorizontalSpacing(m_settings->value("Margins/main", 10).toInt());
}

void poolTab::updateCheckboxes()
{
	log(tr("Mise à jour des cases à cocher."));
	qDeleteAll(m_checkboxes);
	m_checkboxes.clear();
	QStringList urls = m_sites->keys();
	int n = m_settings->value("Sources/Letters", 3).toInt(), m = n;
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

		bool isChecked = m_selectedSources.size() > i ? m_selectedSources.at(i) : false;
		QCheckBox *c = new QCheckBox(urls.at(i).left(m), this);
			c->setChecked(isChecked);
			ui->layoutSourcesList->addWidget(c);

		m_checkboxes.append(c);
	}
	DONE();
}



void poolTab::load()
{
	log(tr("Chargement des résultats..."));

	m_stop = true;
	m_parent->setWiki("");
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
	emit titleChanged(this);
	m_tags.clear();
	m_parent->setTags(m_tags, this);
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

	QStringList tags = m_search->toPlainText().trimmed().split(" ", QString::SkipEmptyParts);
	tags.append(m_settings->value("add").toString().trimmed().split(" ", QString::SkipEmptyParts));
	tags.prepend("pool:"+QString::number(ui->spinPool->value()));
	int perpage = ui->spinImagesPerPage->value();
	Page *page = new Page(m_sites->value(ui->comboSites->currentText()), m_sites->values(), tags, ui->spinPage->value(), perpage, m_postFiltering->toPlainText().split(" ", QString::SkipEmptyParts), true, this);
	log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
	connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
	m_pages.insert(page->website(), page);
	QGridLayout *l = new QGridLayout;
	l->setHorizontalSpacing(m_settings->value("Margins/horizontal", 6).toInt());
	l->setVerticalSpacing(m_settings->value("Margins/vertical", 6).toInt());
	m_layouts.append(l);
	m_stop = false;
	page->load();
	if (m_settings->value("useregexfortags", true).toBool())
	{
		connect(page, SIGNAL(finishedLoadingTags(Page*)), this, SLOT(finishedLoadingTags(Page*)));
		page->loadTags();
	}

	m_page = 0;

	emit changed(this);
}

void poolTab::finishedLoading(Page* page)
{
	if (m_stop)
	{ return; }

	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));

	QList<Image*> imgs = page->images();
	m_images.append(imgs);

	int maxpage = page->pagesCount();
	if (maxpage < m_pagemax || m_pagemax == -1)
	{ m_pagemax = maxpage; }
	ui->buttonNextPage->setEnabled(maxpage > ui->spinPage->value() || page->imagesCount() == -1 || (page->imagesCount() == 0 && page->images().count() > 0));
	ui->buttonLastPage->setEnabled(maxpage > ui->spinPage->value());

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
				QStringList favs;
				for (Favorite fav : m_favorites)
					favs.append(fav.getName());
				completion.append(favs);
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
					ui->labelMeant->setText("<a href=\""+cl.join(" ").toHtmlEscaped()+"\" style=\"color:black;text-decoration:none;\">"+res.join(" ")+"</a>");
				}
				words.close();
			}
			txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+tr("Aucun résultat")+(reasons.count() > 0 ? "<br/>"+tr("Raisons possibles : %1").arg(reasons.join(", ")) : ""));
		}
		else
		{ txt->setText("<a href=\""+page->url().toString().toHtmlEscaped()+"\">"+page->site()->name()+"</a> - "+tr("Page %1 sur %2 (%3 sur %4)").arg(ui->spinPage->value()).arg(page->pagesCount(false) > 0 ? QString::number(maxpage) : "?").arg(imgs.count()).arg(page->imagesCount(false) > 0 ? QString::number(page->imagesCount(false)) : "?")); }
		txt->setOpenExternalLinks(true);
		if (page->search().join(" ") != m_search->toPlainText() && m_settings->value("showtagwarning", true).toBool())
		{
			QStringList uncommon = m_search->toPlainText().trimmed().split(" ", QString::SkipEmptyParts);
			uncommon.append(m_settings->value("add").toString().trimmed().split(" ", QString::SkipEmptyParts));
			for (int i = 0; i < page->search().size(); i++)
			{
				if (uncommon.contains(page->search().at(i)))
				{ uncommon.removeAll(page->search().at(i)); }
			}
			if (!uncommon.isEmpty())
			{ txt->setText(txt->text()+"<br/>"+QString(tr("Des modificateurs ont été otés de la recherche car ils ne sont pas compatibles avec cet imageboard : %1.")).arg(uncommon.join(" "))); }
		}
		if (!page->errors().isEmpty() && m_settings->value("showwarnings", true).toBool())
		{ txt->setText(txt->text()+"<br/>"+page->errors().join("<br/>")); }
	int page_x = pos % ui->spinColumns->value(), page_y = (pos / ui->spinColumns->value()) * 2;
	ui->layoutResults->addWidget(txt, page_y, page_x, 1, 1);
	ui->layoutResults->setRowMinimumHeight(page_y, height()/20);
	if (m_layouts.size() > pos)
		{ ui->layoutResults->addLayout(m_layouts[pos], page_y + 1, page_x, 1, 1); }

	if (!m_settings->value("useregexfortags", true).toBool())
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
		QStringList tlist = QStringList() << "artists" << "circles" << "copyrights" << "characters" << "models" << "generals" << "favorites" << "blacklisteds";
		QStringList defaults = QStringList() << "#aa0000" << "#55bbff" << "#aa00aa" << "#00aa00" << "#0000ee" << "#000000" << "#ffc0cb" << "#000000";
		QMap<QString,QString> styles;
		for (int i = 0; i < tlist.size(); i++)
		{
			QFont font;
			font.fromString(m_settings->value("Coloring/Fonts/"+tlist.at(i)).toString());
			styles[tlist.at(i)] = "color:"+m_settings->value("Coloring/Colors/"+tlist.at(i), defaults.at(i)).toString()+"; "+qfonttocss(font);
		}
		QString tags;
		int last = 0, h = height()/10;
		for (int i = 0; i < taglist.size(); i++)
		{
			if (i < h || last == taglist[i].count())
			{
				bool favorited = false;
				for (Favorite fav : m_favorites)
					if (fav.getName() == taglist[i].text())
						favorited = true;
				if (favorited)
					taglist[i].setType("favorite");

				last = taglist[i].count();
			}
		}

		m_tags = taglist;
		m_parent->setTags(m_tags, this);
	}

	m_page++;

	// Loading images
	for (int i = 0; i < imgs.count(); i++)
	{
		QStringList detected;
		Image *img = imgs.at(i);
		QStringList tags = m_search->toPlainText().toLower().split(' ');
		QList<QChar> modifiers = QList<QChar>() << '~';
		for (int r = 0; r < tags.size(); r++)
		{
			if (modifiers.contains(tags[r][0]))
			{ tags[r] = tags[r].right(tags[r].size()-1); }
		}
		if (!m_settings->value("blacklistedtags").toString().isEmpty())
		{
			QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().toLower().split(" "));
			for (int t = 0; t < img->tags().count(); t++)
			{
				if (blacklistedtags.contains(img->tags()[t].text().toLower(), Qt::CaseInsensitive) && !tags.contains(img->tags()[t].text().toLower(), Qt::CaseInsensitive))
				{ detected.append(img->tags()[t].text()); }
			}
		}
		if (!detected.isEmpty() && m_settings->value("hideblacklisted", false).toBool())
		{ log(tr("Image #%1 ignorée. Raison : %2.").arg(i).arg("\""+detected.join(", ")+"\""));; }
		else
		{
			connect(img, SIGNAL(finishedLoadingPreview(Image*)), this, SLOT(finishedLoadingPreview(Image*)));
			img->loadPreview();
		}
	}
}

void poolTab::finishedLoadingTags(Page *page)
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

	m_tags = taglist;
	m_parent->setTags(m_tags, this);

	if (!page->wiki().isEmpty())
	{
		m_wiki = "<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+page->wiki();
		m_parent->setWiki(m_wiki);
	}
}

void poolTab::finishedLoadingPreview(Image *img)
{
	if (m_stop)
	{ return; }

	int position = m_images.indexOf(img), page = 0;
	page = m_pages.values().indexOf(img->page());
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

	QColor color;
	if (img->status() == "pending")
	{ color = QColor("#0000ff"); }
	if (img->parentId() != 0)
	{ color = QColor("#cccc00"); }
	if (img->hasChildren())
	{ color = QColor("#00ff00"); }
	for (int i = 0; i < img->tags().count(); i++)
	{
		if (!m_search->toPlainText().trimmed().split(" ").contains(img->tags()[i].text()))
		{
			for (Favorite fav : m_favorites)
			{
				if (fav.getName() == img->tags()[i].text())
				{
					color = QColor("#ffc0cb");
					break;
				}
			}
		}
	}
	QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
	QStringList detected = img->blacklisted(blacklistedtags);
	if (!detected.isEmpty())
	{ color = QColor("#000000"); }
	QBouton *l = new QBouton(position, m_settings->value("resizeInsteadOfCropping", true).toBool(), m_settings->value("borders", 3).toInt(), color, this);
		QString t;
		for (int i = 0; i < img->tags().count(); i++)
		{ t += " "+img->tags()[i].stylished(m_favorites); }
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
		l->scale(img->previewImage(), m_settings->value("thumbnailUpscale", 1.0f).toFloat());
		l->setFlat(true);
		connect(l, SIGNAL(appui(int)), this, SLOT(webZoom(int)));
		connect(l, SIGNAL(rightClick(int)), m_parent, SLOT(batchChange(int)));
	int perpage = img->page()->site()->value("Urls/Selected/Tags").contains("{limit}") ? ui->spinImagesPerPage->value() : img->page()->images().size();
	perpage = perpage > 0 ? perpage : 20;
	int pl = ceil(sqrt((double)perpage));
	int pp = perpage;
	if (m_layouts.size() > page)
	{ m_layouts[page]->addWidget(l, floor(float(position % pp) / pl), position % pl); }
	m_boutons.append(l);
}

void poolTab::webZoom(int id)
{
	Image *image = m_images.at(id);

	if (!m_settings->value("blacklistedtags").toString().isEmpty())
	{
		QStringList blacklistedtags(m_settings->value("blacklistedtags").toString().split(" "));
		QStringList detected = image->blacklisted(blacklistedtags);
		if (!detected.isEmpty())
		{
			int reply = QMessageBox::question(m_parent, tr("List noire"), tr("%n tag(s) figurant dans la liste noire détécté(s) sur cette image : %1. Voulez-vous l'afficher tout de même ?", "", detected.size()).arg(detected.join(", ")), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::No)
			{ return; }
		}
	}

	zoomWindow *zoom = new zoomWindow(image, image->page()->site(), m_sites, m_parent);
	zoom->show();
	connect(zoom, SIGNAL(linkClicked(QString)), this, SLOT(setTags(QString)));
	connect(zoom, SIGNAL(poolClicked(int, QString)), this, SLOT(setPool(int, QString)));
}

void poolTab::getPage()
{
	bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	int perpage = unloaded ? ui->spinImagesPerPage->value() : m_pages.value(ui->comboSites->currentText())->images().count();
	emit batchAddGroup(QStringList() << "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed() << QString::number(ui->spinPage->value()) << QString::number(perpage) << QString::number(perpage) << m_settings->value("downloadblacklist").toString() << ui->comboSites->currentText() << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
}
void poolTab::getAll()
{
	int limit = m_sites->value(ui->comboSites->currentText())->contains("Urls/1/Limit") ? m_sites->value(ui->comboSites->currentText())->value("Urls/1/Limit").toInt() : 0;
	emit batchAddGroup(QStringList() << "pool:"+QString::number(ui->spinPool->value())+" "+m_search->toPlainText()+" "+m_settings->value("add").toString().trimmed() << "1" << QString::number(qMin((limit > 0 ? limit : 200), qMax(m_pages.value(ui->comboSites->currentText())->images().count(), m_pages.value(ui->comboSites->currentText())->imagesCount()))) << QString::number(qMax(m_pages.value(ui->comboSites->currentText())->images().count(), m_pages.value(ui->comboSites->currentText())->imagesCount())) << m_settings->value("downloadblacklist").toString() << ui->comboSites->currentText() << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
}
void poolTab::getSel()
{
	if (m_selectedImagesPtrs.empty())
		return;

	for (Image *img : m_selectedImagesPtrs)
	{
		QStringList tags;
		for (Tag tag : img->tags())
		{ tags.append(tag.text()); }

		QMap<QString,QString> values;
		values.insert("id", QString::number(img->id()));
		values.insert("md5", img->md5());
		values.insert("rating", img->rating());
		values.insert("tags", tags.join(" "));
		values.insert("file_url", img->fileUrl().toString());
		values.insert("date", img->createdAt().toString(Qt::ISODate));
		values.insert("site", img->site());
		values.insert("filename", m_settings->value("Save/filename").toString());
		values.insert("folder", m_settings->value("Save/path").toString());

		values.insert("page_url", m_sites->value(img->site())->value("Urls/Html/Post"));
		QString t = m_sites->value(img->site())->contains("DefaultTag") ? m_sites->value(img->site())->value("DefaultTag") : "";
		values["page_url"].replace("{tags}", t);
		values["page_url"].replace("{id}", values["id"]);

		emit batchAddUnique(values);
	}

	m_selectedImagesPtrs.clear();
	m_selectedImages.clear();
	for (QBouton *l : m_boutons)
	{ l->setChecked(false); }
}

void poolTab::firstPage()
{
	ui->spinPage->setValue(1);
	load();
}
void poolTab::previousPage()
{
	if (ui->spinPage->value() > 1)
	{
		ui->spinPage->setValue(ui->spinPage->value()-1);
		load();
	}
}
void poolTab::nextPage()
{
	if (ui->spinPage->value() < ui->spinPage->maximum())
	{
		ui->spinPage->setValue(ui->spinPage->value()+1);
		load();
	}
}
void poolTab::lastPage()
{
	ui->spinPage->setValue(m_pagemax);
	load();
}



void poolTab::linkHovered(QString url)
{ m_link = url; }
void poolTab::linkClicked(QString url)
{
	if (Qt::ControlModifier)
	{ m_parent->addTab(url); }
	else
	{
		m_search->setPlainText(url);
		load();
	}
}
void poolTab::contextMenu()
{
	QMenu *menu = new QMenu(this);
	if (!this->m_link.isEmpty())
	{
		bool favorited = false;
		for (Favorite fav : m_favorites)
			if (fav.getName() == m_link)
				favorited = true;
		if (favorited)
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Retirer des favoris"), this, SLOT(unfavorite())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Ajouter aux favoris"), this, SLOT(favorite())); }

		QStringList vil = loadViewItLater();
		if (vil.contains(m_link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Ne pas garder pour plus tard"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Garder pour plus tard"), this, SLOT(viewitlater())); }

		menu->addSeparator();
		menu->addAction(QIcon(":/images/icons/tab-plus.png"), tr("Ouvrir dans un nouvel onglet"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}
void poolTab::openInNewTab()
{ m_parent->addTab(m_link); }
void poolTab::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(m_link));
}
void poolTab::viewitlater()
{
	QStringList vil = loadViewItLater();
	vil.append(m_link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(vil.join("\r\n").toUtf8());
	f.close();
}
void poolTab::unviewitlater()
{
	QStringList vil = loadViewItLater();
	vil.removeAll(m_link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(vil.join("\r\n").toUtf8());
	f.close();
}

void poolTab::historyBack()
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
void poolTab::historyNext()
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

void poolTab::setTags(QString tags)
{
	activateWindow();
	m_search->setText(tags);
	load();
}
void poolTab::setPool(int id, QString site)
{
	activateWindow();
	ui->spinPool->setValue(id);
	int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
	load();
}
void poolTab::setSite(QString site)
{
	int index = ui->comboSites->findText(site);
	if (index != -1)
	{ ui->comboSites->setCurrentIndex(index); }
}

void poolTab::focusSearch()
{
	ui->spinPool->focusWidget();
}

void poolTab::setImagesPerPage(int ipp)
{ ui->spinImagesPerPage->setValue(ipp); }
void poolTab::setColumns(int columns)
{ ui->spinColumns->setValue(columns); }
void poolTab::setPostFilter(QString postfilter)
{ m_postFiltering->setText(postfilter); }

int poolTab::imagesPerPage()	{ return ui->spinImagesPerPage->value();	}
int poolTab::columns()			{ return ui->spinColumns->value();			}
QString poolTab::postFilter()	{ return m_postFiltering->toPlainText();	}
QString poolTab::tags()			{ return m_search->toPlainText();			}
QString poolTab::wiki()			{ return m_wiki;							}
QString poolTab::site()			{ return ui->comboSites->currentText();		}
