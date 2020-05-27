#include "tabs/favorites-tab.h"
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QtMath>
#include <ui_favorites-tab.h>
#include <algorithm>
#include "downloader/download-query-group.h"
#include "favorite-window.h"
#include "functions.h"
#include "helpers.h"
#include "logger.h"
#include "main-window.h"
#include "models/favorite.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "ui/fixed-size-grid-layout.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"
#include "ui/text-edit.h"

#define FAVORITES_THUMB_SIZE 150


FavoritesTab::FavoritesTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent)
	: SearchTab(profile, downloadQueue, parent, "Favorites"), ui(new Ui::FavoritesTab), m_currentFav(0)
{
	ui->setupUi(this);

	// Promote favorites layout into fixed-size grid layout
	const int hSpace = m_settings->value("Margins/horizontal", 6).toInt();
	const int vSpace = m_settings->value("Margins/vertical", 6).toInt();
	m_favoritesLayout = new FixedSizeGridLayout(hSpace, vSpace);
	const bool fixedWidthLayout = m_settings->value("resultsFixedWidthLayout", false).toBool();
	if (fixedWidthLayout) {
		const int borderSize = m_settings->value("borders", 3).toInt();
		const qreal upscale = m_settings->value("thumbnailUpscale", 1.0).toDouble();
		m_favoritesLayout->setFixedWidth(qFloor(FAVORITES_THUMB_SIZE * upscale + borderSize * 2));
	}
	QWidget *layoutWidget = new QWidget;
	layoutWidget->setLayout(m_favoritesLayout);
	ui->layoutFavorites->addWidget(layoutWidget, 0, 0);

	// UI members for SearchTab class
	ui_checkMergeResults = ui->checkMergeResults;
	ui_progressMergeResults = ui->progressMergeResults;
	ui_stackedMergeResults = ui->stackedMergeResults;
	ui_spinPage = ui->spinPage;
	ui_spinImagesPerPage = ui->spinImagesPerPage;
	ui_spinColumns = ui->spinColumns;
	ui_layoutResults = ui->layoutResults;
	ui_layoutSourcesList = ui->layoutSourcesList;
	ui_buttonHistoryBack = ui->buttonHistoryBack;
	ui_buttonHistoryNext = ui->buttonHistoryNext;
	ui_buttonNextPage = ui->buttonNextPage;
	ui_buttonLastPage = ui->buttonLastPage;
	ui_buttonGetAll = ui->buttonGetAll;
	ui_buttonGetPage = ui->buttonGetpage;
	ui_buttonGetSel = ui->buttonGetSel;
	ui_buttonFirstPage = ui->buttonFirstPage;
	ui_buttonPreviousPage = ui->buttonPreviousPage;
	ui_scrollAreaResults = ui->scrollAreaResults;

	// Search field
	m_postFiltering = createAutocomplete();
	ui->layoutPlus->addWidget(m_postFiltering, 1, 1, 1, 3);

	// Open tab with closed result splitter
	ui->splitter->setSizes(QList<int>() << 1 << 0);

	// Others
	ui->checkMergeResults->setChecked(m_settings->value("mergeresults", false).toBool());
	optionsChanged();
	ui->widgetPlus->hide();
	setWindowIcon(QIcon());
	updateCheckboxes();

	static QStringList assoc = QStringList() << QStringLiteral("name") << QStringLiteral("note") << QStringLiteral("lastviewed");
		ui->comboOrder->setCurrentIndex(assoc.indexOf(m_settings->value("Favorites/order", "name").toString()));
		ui->comboAsc->setCurrentIndex(static_cast<int>(m_settings->value("Favorites/reverse", false).toBool()));
		m_settings->setValue("reverse", ui->comboAsc->currentIndex() == 1);
	ui->widgetResults->hide();

	connect(m_profile, &Profile::favoritesChanged, this, &FavoritesTab::updateFavorites);
	updateFavorites();
}

FavoritesTab::~FavoritesTab()
{
	close();
	delete ui;
}

void FavoritesTab::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("mergeresults", ui->checkMergeResults->isChecked());
	m_settings->beginGroup("Favorites");
		static QStringList assoc = QStringList() << QStringLiteral("name") << QStringLiteral("note") << QStringLiteral("lastviewed");
		m_settings->setValue("order", assoc[ui->comboOrder->currentIndex()]);
		m_settings->setValue("reverse", ui->comboAsc->currentIndex() == 1);
	m_settings->endGroup();
	m_settings->sync();

	emit closed(this);
	e->accept();
}



void FavoritesTab::updateFavorites()
{
	static const QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	const QString &order = assoc[ui->comboOrder->currentIndex()];
	const bool reverse = (ui->comboAsc->currentIndex() == 1);

	if (order == "note") {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByNote);
	} else if (order == "lastviewed") {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByLastViewed);
	} else {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByName);
	}
	if (reverse) {
		m_favorites = reversed(m_favorites);
	}

	clearLayout(m_favoritesLayout);

	QString display = m_settings->value("favorites_display", "ind").toString();
	const qreal upscale = m_settings->value("thumbnailUpscale", 1.0).toDouble();
	const int borderSize = m_settings->value("borders", 3).toInt();
	const int imageSize = qFloor(FAVORITES_THUMB_SIZE * upscale);
	const int dim = imageSize + borderSize * 2;

	for (Favorite &fav : m_favorites) {
		const QString xt = tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(Qt::DefaultLocaleShortDate));
		QWidget *w = new QWidget(ui->scrollAreaWidgetContents);
		auto *l = new QVBoxLayout;
		l->setMargin(0);
		w->setLayout(l);

		int maxNewImages = 0;
		bool precise = true;
		for (const Monitor &monitor : qAsConst(fav.getMonitors())) {
			if (monitor.cumulated() > maxNewImages) {
				maxNewImages = monitor.cumulated();
				precise = monitor.preciseCumulated();
			}
		}

		if (display.contains("i")) {
			const bool resizeInsteadOfCropping = m_settings->value("resizeInsteadOfCropping", true).toBool();

			QPixmap img = fav.getImage();
			QBouton *image = new QBouton(fav.getName(), resizeInsteadOfCropping, false, 0, QColor(), this);
				image->scale(img, QSize(imageSize, imageSize));
				image->setFixedSize(dim, dim);
				image->setFlat(true);
				image->setToolTip(xt);
				connect(image, SIGNAL(rightClick(QString)), this, SLOT(favoriteProperties(QString)));
				connect(image, SIGNAL(middleClick(QString)), m_parent, SLOT(addTab(QString)));
				connect(image, SIGNAL(appui(QString)), this, SLOT(loadFavorite(QString)));

			if (maxNewImages > 0)
			{ image->setCounter(QString::number(maxNewImages) + (!precise ? "+" : QString())); }

			l->addWidget(image);
		}

		QString label;
		if (display.contains("n")) {
			label += fav.getName();
			if (maxNewImages > 0 && !display.contains("i")) {
				label += QStringLiteral(" <b style='color:red'>(%1%2)</b>").arg(maxNewImages).arg(!precise ? "+" : QString());
			}
		}
		if (display.contains("d")) {
			label += "<br/>(" + QString::number(fav.getNote()) + " % - " + fav.getLastViewed().date().toString(Qt::DefaultLocaleShortDate) + ")";
		}

		QAffiche *caption = new QAffiche(fav.getName(), 0, QColor(), this);
			caption->setText(label);
			caption->setTextFormat(Qt::RichText);
			caption->setAlignment(Qt::AlignCenter);
			caption->setToolTip(xt);
			caption->setFixedWidth(dim);
		if (!caption->text().isEmpty()) {
			connect(caption, SIGNAL(rightClicked(QString)), this, SLOT(favoriteProperties(QString)));
			connect(caption, SIGNAL(middleClicked(QString)), m_parent, SLOT(addTab(QString)));
			connect(caption, SIGNAL(clicked(QString)), this, SLOT(loadFavorite(QString)));
			l->addWidget(caption);
		}

		m_favoritesLayout->addWidget(w);
	}
}


void FavoritesTab::load()
{
	updateTitle();

	loadTags(m_currentTags.trimmed().split(' ', QString::SkipEmptyParts));
}

bool FavoritesTab::validateImage(const QSharedPointer<Image> &img, QString &error)
{
	bool dateOk = img->createdAt() > m_loadFavorite || img->createdAt().isNull();
	return dateOk && SearchTab::validateImage(img, error);
}

void FavoritesTab::write(QJsonObject &json) const
{
	Q_UNUSED(json)
}


void FavoritesTab::addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, const QString &noResultsMessage)
{
	Q_UNUSED(noResultsMessage)

	SearchTab::addResultsPage(page, imgs, merged, tr("No result since the %1").arg(m_loadFavorite.toString(Qt::DefaultLocaleShortDate)));
	ui->splitter->setSizes(QList<int>() << (m_images.count() >= m_settings->value("hidefavorites", 20).toInt() ? 0 : 1) << 1);
}

void FavoritesTab::setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, const QString &noResultsMessage)
{
	Q_UNUSED(noResultsMessage)

	SearchTab::setPageLabelText(txt, page, imgs, tr("No result since the %1").arg(m_loadFavorite.toString(Qt::DefaultLocaleShortDate)));
}

void FavoritesTab::setTags(const QString &tags, bool preload)
{
	m_currentTags = tags;

	if (preload) {
		activateWindow();
		load();
	} else {
		updateTitle();
	}
}

void FavoritesTab::getPage()
{
	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();

	QList<QSharedPointer<Page>> pages = this->getPagesToDownload();
	for (const QSharedPointer<Page> &page : pages) {
		const QStringList search = (m_currentTags + " " + m_settings->value("add").toString().toLower().trimmed()).split(' ', QString::SkipEmptyParts);
		const int perpage = unloaded ? ui->spinImagesPerPage->value() : page->pageImageCount();
		const QStringList postFiltering = postFilter(true);

		emit batchAddGroup(DownloadQueryGroup(m_settings, search, ui->spinPage->value(), perpage, perpage, postFiltering, page->site()));
	}
}
void FavoritesTab::getAll()
{
	QList<QSharedPointer<Page>> pages = this->getPagesToDownload();
	for (const QSharedPointer<Page> &page : pages) {
		const int highLimit = page->highLimit();
		const int currentCount = page->pageImageCount();
		const int imageCount = page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
		const int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
		const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
		if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0)) {
			continue;
		}

		const QStringList search = (m_currentTags + " " + m_settings->value("add").toString().toLower().trimmed()).split(' ', QString::SkipEmptyParts);
		const QStringList postFiltering = postFilter(true);

		emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, page->site()));
	}
}


QList<Site*> FavoritesTab::sources()
{ return m_selectedSources; }

QString FavoritesTab::tags() const
{ return m_currentTags; }

void FavoritesTab::loadFavorite(const QString &name)
{
	const int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(name));
	if (index < 0) {
		return;
	}

	Favorite fav = m_favorites[index];
	m_currentTags = fav.getName();
	m_loadFavorite = fav.getLastViewed();
	m_postFiltering->setPlainText(fav.getPostFiltering().join(' '));

	if (!fav.getSites().isEmpty()) {
		setSources(fav.getSites());
	}

	ui->widgetResults->show();
	load();
}
void FavoritesTab::checkFavorites()
{
	ui->widgetFavorites->show();
	m_currentFav = -1;
	m_currentTags = QString();
	loadNextFavorite();
}
void FavoritesTab::loadNextFavorite()
{
	if (m_currentFav + 1 >= m_favorites.count()) {
		return;
	}

	m_currentFav++;
	m_currentTags = m_favorites[m_currentFav].getName();
	m_loadFavorite = m_favorites[m_currentFav].getLastViewed();

	load();
}
void FavoritesTab::viewed()
{
	if (m_currentTags.isEmpty()) {
		const int reponse = QMessageBox::question(this, tr("Mark as viewed"), tr("Are you sure you want to mark all your favorites as viewed?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes) {
			for (const Favorite &fav : qAsConst(m_favorites)) {
				setFavoriteViewed(fav.getName());
			}
		}
	} else {
		setFavoriteViewed(m_currentTags);
	}

	m_profile->emitFavorite();
}
void FavoritesTab::setFavoriteViewed(const QString &tag, QDateTime date)
{
	log(QStringLiteral("Marking \"%1\" as viewed...").arg(tag));

	const int index = tag.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(tag));
	if (index < 0) {
		return;
	}

	Favorite &fav = m_favorites[index];

	if (!date.isValid()) {
		fav.setLastViewed(QDateTime::currentDateTime());

		for (Monitor &monitor : fav.getMonitors()) {
			monitor.setCumulated(0, true);
		}
	} else {
		fav.setLastViewed(date);
	}

	DONE();
}
void FavoritesTab::favoritesBack()
{
	ui->widgetResults->hide();
	ui->widgetFavorites->show();
	ui->splitter->setSizes(QList<int>() << 1 << 0);

	if (!m_currentTags.isEmpty() || m_currentFav != -1) {
		m_currentTags = QString();
		m_currentFav = -1;
		ui->widgetFavorites->show();
	}
}
void FavoritesTab::favoriteProperties(const QString &name)
{
	const int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(name));
	if (index < 0) {
		return;
	}

	const Favorite fav = m_favorites[index];
	auto fwin = new FavoriteWindow(m_profile, fav, this);
	fwin->show();
}

void FavoritesTab::focusSearch()
{}


void FavoritesTab::changeEvent(QEvent *event)
{
	// Automatically re-translate this tab on language change
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
		updateTitle();
	}

	QWidget::changeEvent(event);
}

void FavoritesTab::thumbnailContextMenu(QMenu *menu, const QSharedPointer<Image> &img)
{
	SearchTab::thumbnailContextMenu(menu, img);

	if (m_currentTags.isEmpty() || !img->createdAt().isValid()) {
		return;
	}

	QAction *first = menu->actions().first();

	// Mark as "last viewed"
	QAction *actionMarkAsLastViewed = new QAction(QIcon(":/images/icons/eye.png"), tr("Mark as last viewed"), menu);
	connect(actionMarkAsLastViewed, &QAction::triggered, [this, img]() {
		this->setFavoriteViewed(m_currentTags, img->createdAt());
	});
	menu->insertAction(first, actionMarkAsLastViewed);

	menu->insertSeparator(first);
}

void FavoritesTab::updateTitle()
{
	setWindowTitle(tr("Favorites") + (m_currentTags.isEmpty() ? "" : " - " + m_currentTags));
	emit titleChanged(this);
}

void FavoritesTab::splitterMoved(int pos, int index)
{
	const QString title = tr("Favorites");

	int min, max;
	ui->splitter->getRange(index, &min, &max);

	if (index == 1 && pos >= max) {
		setWindowTitle(title);
		emit titleChanged(this);
	} else if (windowTitle() == title) {
		updateTitle();
	}
}
