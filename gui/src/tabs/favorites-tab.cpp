#include "tabs/favorites-tab.h"
#include <QMessageBox>
#include <QtMath>
#include <ui_favorites-tab.h>
#include <algorithm>
#include "downloader/download-query-group.h"
#include "favoritewindow.h"
#include "functions.h"
#include "helpers.h"
#include "mainwindow.h"
#include "models/favorite.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "ui/fixed-size-grid-layout.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"
#include "ui/textedit.h"

#define FAVORITES_THUMB_SIZE 150


favoritesTab::favoritesTab(Profile *profile, mainWindow *parent)
	: searchTab(profile, parent), ui(new Ui::favoritesTab), m_currentFav(0)
{
	ui->setupUi(this);

	// Promote favorites layout into fixed-size grid layout
	const int hSpace = m_settings->value("Margins/horizontal", 6).toInt();
	const int vSpace = m_settings->value("Margins/vertical", 6).toInt();
	m_favoritesLayout = new FixedSizeGridLayout(hSpace, vSpace);
	const bool fixedWidthLayout = m_settings->value("resultsFixedWidthLayout", false).toBool();
	if (fixedWidthLayout)
	{
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
	ui_widgetMeant = nullptr;
	ui_labelMeant = nullptr;
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
	ui_buttonEndlessLoad = nullptr;
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
		static QStringList assoc = QStringList() << QStringLiteral("name") << QStringLiteral("note") << QStringLiteral("lastviewed");
		m_settings->setValue("order", assoc[ui->comboOrder->currentIndex()]);
		m_settings->setValue("reverse", ui->comboAsc->currentIndex() == 1);
	m_settings->endGroup();
	m_settings->sync();

	emit closed(this);
	e->accept();
}



void favoritesTab::updateFavorites()
{
	static const QStringList assoc = QStringList() << "name" << "note" << "lastviewed";
	const QString &order = assoc[ui->comboOrder->currentIndex()];
	const bool reverse = (ui->comboAsc->currentIndex() == 1);

	if (order == "note")
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByNote); }
	else if (order == "lastviewed")
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByLastViewed); }
	else
	{ std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByName); }
	if (reverse)
	{ m_favorites = reversed(m_favorites); }

	const QString format = tr("MM/dd/yyyy");
	clearLayout(m_favoritesLayout);

	QString display = m_settings->value("favorites_display", "ind").toString();
	const qreal upscale = m_settings->value("thumbnailUpscale", 1.0).toDouble();
	const int borderSize = m_settings->value("borders", 3).toInt();
	const int dim = qFloor(FAVORITES_THUMB_SIZE * upscale + borderSize * 2);

	for (Favorite &fav : m_favorites)
	{
		const QString xt = tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(format));
		QWidget *w = new QWidget(ui->scrollAreaWidgetContents);
		auto *l = new QVBoxLayout;
		l->setMargin(0);
		w->setLayout(l);

		int maxNewImages = 0;
		bool precise = true;
		for (const Monitor& monitor : qAsConst(fav.getMonitors()))
		{
			if (monitor.cumulated() > maxNewImages)
			{
				maxNewImages = monitor.cumulated();
				precise = monitor.preciseCumulated();
			}
		}

		if (display.contains("i"))
		{
			QPixmap img = fav.getImage();
			QBouton *image = new QBouton(fav.getName(), false, false, 0, QColor(), this);
				image->scale(img, upscale);
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
		if (display.contains("n"))
		{
			label += fav.getName();
			if (maxNewImages > 0 && !display.contains("i"))
			{ label += QStringLiteral(" <b style='color:red'>(%1%2)</b>").arg(maxNewImages).arg(!precise ? "+" : QString()); }
		}
		if (display.contains("d"))
		{ label += "<br/>("+QString::number(fav.getNote())+" % - "+fav.getLastViewed().toString(format)+")"; }

		QAffiche *caption = new QAffiche(fav.getName(), 0, QColor(), this);
			caption->setText(label);
			caption->setTextFormat(Qt::RichText);
			caption->setAlignment(Qt::AlignCenter);
			caption->setToolTip(xt);
			caption->setFixedWidth(dim);
		if (!caption->text().isEmpty())
		{
			connect(caption, SIGNAL(clicked(QString)), this, SLOT(loadFavorite(QString)));
			l->addWidget(caption);
		}

		m_favoritesLayout->addWidget(w);
	}
}


void favoritesTab::load()
{
	updateTitle();

	loadTags(m_currentTags.trimmed().split(' ', QString::SkipEmptyParts));
}

bool favoritesTab::validateImage(const QSharedPointer<Image> &img, QString &error)
{
	return (img->createdAt() > m_loadFavorite || img->createdAt().isNull()) && searchTab::validateImage(img, error);
}

void favoritesTab::write(QJsonObject &json) const
{
	Q_UNUSED(json);
}


void favoritesTab::addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, const QString &noResultsMessage)
{
	Q_UNUSED(noResultsMessage);
	searchTab::addResultsPage(page, imgs, merged, tr("No result since the %1").arg(m_loadFavorite.toString(tr("MM/dd/yyyy 'at' hh:mm"))));
	ui->splitter->setSizes(QList<int>() << (m_images.count() >= m_settings->value("hidefavorites", 20).toInt() ? 0 : 1) << 1);
}

void favoritesTab::setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, const QString &noResultsMessage)
{
	Q_UNUSED(noResultsMessage);
	searchTab::setPageLabelText(txt, page, imgs, tr("No result since the %1").arg(m_loadFavorite.toString(tr("MM/dd/yyyy 'at' hh:mm"))));
}

void favoritesTab::setTags(const QString &tags, bool preload)
{
	activateWindow();
	m_currentTags = tags;

	if (preload)
		load();
	else
		updateTitle();
}

void favoritesTab::getPage()
{
	QStringList actuals, keys = m_sites.keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
		{ actuals.append(keys.at(i)); }
	}
	const bool unloaded = m_settings->value("getunloadedpages", false).toBool();
	for (int i = 0; i < actuals.count(); i++)
	{
		const auto &page = m_pages[actuals[i]].first();
		const QString search = m_currentTags+" "+m_settings->value("add").toString().toLower().trimmed();
		const int perpage = unloaded ? ui->spinImagesPerPage->value() : page->images().count();
		const QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);

		emit batchAddGroup(DownloadQueryGroup(m_settings, search, ui->spinPage->value(), perpage, perpage, postFiltering, m_sites.value(actuals.at(i))));
	}
}
void favoritesTab::getAll()
{
	QStringList actuals, keys = m_sites.keys();
	for (int i = 0; i < m_checkboxes.count(); i++)
	{
		if (m_checkboxes.at(i)->isChecked())
			actuals.append(keys.at(i));
	}

	for (const QString &actual : actuals)
	{
		const auto &page = m_pages[actual].first();

		const int highLimit = page->highLimit();
		const int currentCount = page->images().count();
		const int imageCount = page->imagesCount() >= 0 ? page->imagesCount() : page->maxImagesCount();
		const int total = imageCount > 0 ? qMax(currentCount, imageCount) : (highLimit > 0 ? highLimit : currentCount);
		const int perPage = highLimit > 0 ? (imageCount > 0 ? qMin(highLimit, imageCount) : highLimit) : currentCount;
		if ((perPage == 0 && total == 0) || (currentCount == 0 && imageCount <= 0))
			continue;

		const QString search = m_currentTags + " " + m_settings->value("add").toString().toLower().trimmed();
		const QStringList postFiltering = m_postFiltering->toPlainText().split(' ', QString::SkipEmptyParts);
		Site *site = m_sites.value(actual);

		emit batchAddGroup(DownloadQueryGroup(m_settings, search, 1, perPage, total, postFiltering, site));
	}
}


QList<Site*> favoritesTab::sources()
{ return m_selectedSources; }

QString favoritesTab::tags() const
{ return m_currentTags;	}

void favoritesTab::loadFavorite(const QString &name)
{
	const int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(name));
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
		const int reponse = QMessageBox::question(this, tr("Mark as viewed"), tr("Are you sure you want to mark all your favorites as viewed?"), QMessageBox::Yes | QMessageBox::No);
		if (reponse == QMessageBox::Yes)
		{
			for (const Favorite &fav : qAsConst(m_favorites))
			{ setFavoriteViewed(fav.getName()); }
		}
	}
	else
	{ setFavoriteViewed(m_currentTags); }

	m_profile->emitFavorite();
}
void favoritesTab::setFavoriteViewed(const QString &tag)
{
	log(QStringLiteral("Marking \"%1\" as viewed...").arg(tag));

	const int index = tag.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(tag));
	if (index < 0)
		return;

	Favorite &fav = m_favorites[index];
	fav.setLastViewed(QDateTime::currentDateTime());

	for (Monitor &monitor : fav.getMonitors())
		monitor.setCumulated(0, true);

	DONE();
}
void favoritesTab::favoritesBack()
{
	ui->widgetResults->hide();
	ui->widgetFavorites->show();
	ui->splitter->setSizes(QList<int>() << 1 << 0);

	if (!m_currentTags.isEmpty() || m_currentFav != -1)
	{
		m_currentTags = QString();
		m_currentFav = -1;
		ui->widgetFavorites->show();
	}
}
void favoritesTab::favoriteProperties(const QString &name)
{
	const int index = name.isEmpty() ? m_currentFav : m_favorites.indexOf(Favorite(name));
	if (index < 0)
		return;

	const Favorite fav = m_favorites[index];
	auto fwin = new favoriteWindow(m_profile, fav, this);
	fwin->show();
}

void favoritesTab::focusSearch()
{ }


void favoritesTab::changeEvent(QEvent *event)
{
	// Automatically retranslate this tab on language change
	if (event->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
		setWindowTitle(tr("Favorites"));
		emit titleChanged(this);
	}

	QWidget::changeEvent(event);
}

void favoritesTab::updateTitle()
{
	// No-op, the Favorites tab never changes its title
}
