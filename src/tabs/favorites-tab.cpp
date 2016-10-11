#include <QMessageBox>
#include "favorites-tab.h"
#include "ui_favorites-tab.h"
#include "ui/QBouton.h"
#include "viewer/zoomwindow.h"
#include "favoritewindow.h"
#include "searchwindow.h"
#include "models/favorite.h"



favoritesTab::favoritesTab(int id, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: searchTab(id, sites, profile, parent), ui(new Ui::favoritesTab), m_id(id), m_favorites(profile->getFavorites()), m_lastTags(QString()), m_sized(false), m_currentFav(0)
{
	ui->setupUi(this);

	// UI members for SearchTab class
	ui_checkMergeResults = ui->checkMergeResults;
	ui_spinPage = ui->spinPage;
	ui_spinImagesPerPage = ui->spinImagesPerPage;
	ui_spinColumns = ui->spinColumns;
	ui_widgetMeant = nullptr;
	ui_labelMeant = nullptr;
	ui_layoutResults = ui->layoutResults;
	ui_layoutSourcesList = ui->layoutSourcesList;
	ui_buttonHistoryBack = ui->buttonHistoryBack;
	ui_buttonHistoryNext = ui->buttonHistoryNext;

	// Search field
	m_postFiltering = createAutocomplete();
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);

	setSelectedSources(m_settings);

	// Others
	ui->checkMergeResults->setChecked(m_settings->value("mergeresults", false).toBool());
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	updateCheckboxes();

	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		ui->comboOrder->setCurrentIndex(assoc.indexOf(m_settings->value("Favorites/order", "name").toString()));
		ui->comboAsc->setCurrentIndex(int(m_settings->value("Favorites/reverse", false).toBool()));
		m_settings->setValue("reverse", bool(ui->comboAsc->currentIndex() == 1));
	ui->widgetResults->hide();

	connect(m_profile, &Profile::favoritesChanged, this, &favoritesTab::updateFavorites);
	updateFavorites();
}

favoritesTab::~favoritesTab()
{
	close();
	delete ui;
}

void favoritesTab::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("mergeresults", ui->checkMergeResults->isChecked());
	m_settings->beginGroup("Favorites");
		QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
		m_settings->setValue("order", assoc[ui->comboOrder->currentIndex()]);
		m_settings->setValue("reverse", bool(ui->comboAsc->currentIndex() == 1));
	m_settings->endGroup();
	m_settings->sync();

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



void favoritesTab::updateFavorites()
{
	QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	QString order = assoc[ui->comboOrder->currentIndex()];
	bool reverse = (ui->comboAsc->currentIndex() == 1);

	if (order == "note")
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByNote); }
	else if (order == "lastviewed")
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByLastviewed); }
	else
	{ qSort(m_favorites.begin(), m_favorites.end(), sortByName); }
	if (reverse)
	{ m_favorites = reversed(m_favorites); }

	QString format = tr("dd/MM/yyyy");
	clearLayout(ui->layoutFavorites);

	QString display = m_settings->value("favorites_display", "ind").toString();
	int i = 0;
	for (Favorite fav : m_favorites)
	{
		QString xt = tr("<b>Nom :</b> %1<br/><b>Note :</b> %2 %%<br/><b>Dernière vue :</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(format));
		QWidget *w = new QWidget(ui->scrollAreaWidgetContents);
		QVBoxLayout *l = new QVBoxLayout(w);

		if (display.contains("i"))
		{
			QPixmap img = fav.getImage();
			QBouton *image = new QBouton(fav.getName(), false, 0, QColor(), this);
				image->setIcon(img);
				image->setIconSize(img.size());
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(QString)), this, SLOT(favoriteProperties(QString)));
				connect(image, SIGNAL(middleClick(QString)), this, SLOT(addTabFavorite(QString)));
				connect(image, SIGNAL(appui(QString)), this, SLOT(loadFavorite(QString)));
			l->addWidget(image);
		}

		QAffiche *caption = new QAffiche(fav.getName(), 0 ,QColor(), this);
			caption->setText((display.contains("n") ? fav.getName() : "") + (display.contains("d") ? "<br/>("+QString::number(fav.getNote())+" % - "+fav.getLastViewed().toString(format)+")" : ""));
			caption->setTextFormat(Qt::RichText);
			caption->setAlignment(Qt::AlignCenter);
			caption->setToolTip(xt);
		if (!caption->text().isEmpty())
		{
			connect(caption, SIGNAL(clicked(QString)), this, SLOT(loadFavorite(QString)));
			l->addWidget(caption);
		}

		ui->layoutFavorites->addWidget(w, i / 8, i % 8);
		++i;
	}
}

void favoritesTab::addTabFavorite(QString name)
{
	m_parent->addTab(name);
}

void favoritesTab::optionsChanged()
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



void favoritesTab::load()
{
	log(tr("Chargement des résultats..."));
	clear();

	if (!m_from_history)
	{ addHistory(m_currentTags, ui->spinPage->value(), ui->spinImagesPerPage->value(), ui->spinColumns->value()); }
	m_from_history = false;

	if (m_currentTags != m_lastTags && !m_lastTags.isNull() && m_history_cursor == m_history.size() - 1)
	{ ui->spinPage->setValue(1); }
	m_lastTags = m_currentTags;

	ui->buttonFirstPage->setEnabled(ui->spinPage->value() > 1);
	ui->buttonPreviousPage->setEnabled(ui->spinPage->value() > 1);

	for (int i = 0; i < m_selectedSources.size(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{
			QGridLayout *l = new QGridLayout;
			l->setHorizontalSpacing(m_settings->value("Margins/horizontal", 6).toInt());
			l->setVerticalSpacing(m_settings->value("Margins/vertical", 6).toInt());
			m_layouts.append(l);

			QStringList tags = m_currentTags.toLower().trimmed().split(" ", QString::SkipEmptyParts);
			tags.append(m_settings->value("add").toString().toLower().trimmed().split(" ", QString::SkipEmptyParts));
			int perpage = ui->spinImagesPerPage->value();
			Page *page = new Page(m_sites->value(m_sites->keys().at(i)), m_sites->values(), tags, ui->spinPage->value(), perpage, m_postFiltering->toPlainText().toLower().split(" ", QString::SkipEmptyParts), true, this);
			connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(finishedLoading(Page*)));
			m_pages.insert(page->website(), page);

			log(tr("Chargement de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));
			page->load();

			if (m_settings->value("useregexfortags", true).toBool())
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
		return;

	log(tr("Réception de la page <a href=\"%1\">%1</a>").arg(page->url().toString().toHtmlEscaped()));

	QList<QSharedPointer<Image>> imgs;
	for (QSharedPointer<Image> img : page->images())
	{
		if (img->createdAt() > m_loadFavorite || img->createdAt().isNull())
		{ imgs.append(img); }
	}
	m_images.append(imgs);
	int maxpage = page->pagesCount();

	if (maxpage < m_pagemax || m_pagemax == -1)
	{ m_pagemax = maxpage; }
	ui->buttonNextPage->setEnabled(maxpage > ui->spinPage->value() || page->imagesCount() == -1 || (page->imagesCount() == 0 && page->images().count() > 0));
	ui->buttonLastPage->setEnabled(maxpage > ui->spinPage->value());

	if (!ui->checkMergeResults->isChecked())
	{
		addResultsPage(page, imgs, tr("Aucun résultat depuis le %1").arg(m_loadFavorite.toString(tr("dd/MM/yyyy 'à' hh:mm"))));
		ui->splitter->setSizes(QList<int>() << (imgs.count() >= m_settings->value("hidefavorites", 20).toInt() ? 0 : 1) << 1);
	}

	if (!m_settings->value("useregexfortags", true).toBool())
	{ setTagsFromPages(m_pages); }

	postLoading(page);
}

void favoritesTab::failedLoading(Page *page)
{
	if (ui->checkMergeResults->isChecked())
	{
		postLoading(page);
	}
}

void favoritesTab::postLoading(Page *page)
{
	QList<QSharedPointer<Image>> imgs;
	if (!waitForMergedResults(ui->checkMergeResults->isChecked(), page, imgs))
		return;

	loadImageThumbnails(page, imgs);

	ui->buttonGetAll->setDisabled(m_images.empty());
	ui->buttonGetpage->setDisabled(m_images.empty());
	ui->buttonGetSel->setDisabled(m_images.empty());
}

void favoritesTab::finishedLoadingTags(Page *page)
{
	setTagsFromPages(m_pages);

	if (!page->wiki().isEmpty())
	{
		m_wiki = "<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>"+page->wiki();
		m_parent->setWiki(m_wiki);
	}
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
	bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	for (int i = 0; i < actuals.count(); i++)
	{
		int perpage = unloaded ? ui->spinImagesPerPage->value() : m_pages.value(actuals.at(i))->images().count();
		emit batchAddGroup(QStringList() << m_currentTags+" "+m_settings->value("add").toString().toLower().trimmed() << QString::number(ui->spinPage->value()) << QString::number(perpage) << QString::number(perpage) << m_settings->value("downloadblacklist").toString() << actuals.at(i) << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
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
	for (int i = 0; i < actuals.count(); i++)
	{
		int limit = m_sites->value(actuals.at(i))->contains("Urls/1/Limit") ? m_sites->value(actuals.at(i))->value("Urls/1/Limit").toInt() : 0;
		emit batchAddGroup(QStringList() << m_currentTags+" "+m_settings->value("add").toString().toLower().trimmed() << "1" << QString::number(qMin((limit > 0 ? limit : 1000), qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount()))) << QString::number(qMax(m_pages.value(actuals.at(i))->images().count(), m_pages.value(actuals.at(i))->imagesCount())) << m_settings->value("downloadblacklist").toString() << actuals.at(i) << m_settings->value("Save/filename").toString() << m_settings->value("Save/path").toString() << "");
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


QList<bool> favoritesTab::sources()
{ return m_selectedSources; }

QString favoritesTab::tags()	{ return m_currentTags;	}
QString favoritesTab::wiki()	{ return m_wiki;		}

void favoritesTab::loadFavorite(QString name)
{
	int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(name);
	if (index < 0)
		return;

	Favorite fav = m_favorites[index];
	m_currentTags = fav.getName();
	m_loadFavorite = fav.getLastViewed();

	ui->widgetResults->show();
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
	if (m_currentFav + 1 >= m_favorites.count())
		return;

	m_currentFav++;
	m_currentTags = m_favorites[m_currentFav].getName();
	m_loadFavorite = m_favorites[m_currentFav].getLastViewed();

	load();
}
void favoritesTab::viewed()
{
	if (m_currentTags.isEmpty())
	{
		int reponse = QMessageBox::question(this, tr("Marquer comme vu"), tr("Êtes-vous sûr de vouloir marquer tous vos favoris comme vus ?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			for (Favorite fav : m_favorites)
			{ setFavoriteViewed(fav.getName()); }
		}
	}
	else
	{ setFavoriteViewed(m_currentTags); }

	m_profile->emitFavorite();
}
void favoritesTab::setFavoriteViewed(QString tag)
{
	log(tr("Marquage comme vu de %1...").arg(tag));

	int index = tag.isEmpty() ? m_currentFav : m_favorites.indexOf(tag);
	if (index < 0)
		return;

	m_favorites[index].setLastViewed(QDateTime::currentDateTime());

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
	}
}
void favoritesTab::favoriteProperties(QString name)
{
	int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(name);
	if (index < 0)
		return;

	Favorite fav = m_favorites[index];
	favoriteWindow *fwin = new favoriteWindow(m_profile, fav, this);
	fwin->show();
}

void favoritesTab::focusSearch()
{
	//m_search->focusWidget();
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
