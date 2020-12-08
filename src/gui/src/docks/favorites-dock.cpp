#include "docks/favorites-dock.h"
#include <QCursor>
#include <QEvent>
#include <QStringList>
#include <ui_favorites-dock.h>
#include "functions.h"
#include "helpers.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "tabs/search-tab.h"
#include "tag-context-menu.h"
#include "tags/tag.h"
#include "ui/QAffiche.h"


FavoritesDock::FavoritesDock(Profile *profile, QWidget *parent)
	: Dock(parent), ui(new Ui::FavoritesDock), m_profile(profile), m_favorites(profile->getFavorites())
{
	ui->setupUi(this);

	connect(profile, &Profile::favoritesChanged, this, &FavoritesDock::refresh);

	refresh();
}

FavoritesDock::~FavoritesDock()
{
	close();
	delete ui;
}

void FavoritesDock::setHover(const QString &tag)
{
	m_isHover = true;
	m_hover = tag;
}

void FavoritesDock::clearHover()
{
	m_isHover = false;
}

void FavoritesDock::contextMenu(const QPoint &pos)
{
	Q_UNUSED(pos);

	if (!m_isHover) {
		return;
	}

	const QList<Site*> sites = m_currentTab->loadSites();
	TagContextMenu *menu = new TagContextMenu(m_hover, {}, {}, m_profile, sites, false, this);
	connect(menu, &TagContextMenu::openNewTab, this, &FavoritesDock::emitOpenInNewTab);
	menu->exec(QCursor::pos());
}

void FavoritesDock::emitOpenInNewTab()
{
	emit openInNewTab(m_hover);
}

void FavoritesDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void FavoritesDock::changeSortDirection()
{
	m_descending = !m_descending;

	static const QIcon asc(":/images/icons/arrow-down.png");
	static const QIcon desc(":/images/icons/arrow-up.png");
	ui->buttonSortDirection->setIcon(m_descending ? desc : asc);

	refresh();
}

void FavoritesDock::refresh()
{
	clearLayout(ui->layoutFavorites);

	// Sort
	static const QStringList assoc { "name", "note", "lastviewed" };
	const QString order = assoc[qMax(ui->comboSortBy->currentIndex(), 0)];
	if (order == "note") {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByNote);
	} else if (order == "lastviewed") {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByLastViewed);
	} else {
		std::sort(m_favorites.begin(), m_favorites.end(), Favorite::sortByName);
	}

	// Reverse
	if (m_descending) {
		m_favorites = reversed(m_favorites);
	}

	int i = 0;
	for (const Favorite &fav : qAsConst(m_favorites)) {
		QAffiche *lab = new QAffiche(QString(fav.getName()), 0, QColor(), this);
		lab->setText(fav.getName());
		lab->setToolTip("<img src=\"" + fav.getImagePath() + "\" /><br/>" + tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %%<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(Qt::DefaultLocaleShortDate)));
		lab->setCursor(Qt::PointingHandCursor);

		if (i++ % 2 == 1) {
			lab->setStyleSheet("QAffiche { background-color: rgba(128, 128, 128, 10%); }");
		}

		connect(lab, SIGNAL(clicked(QString)), this, SIGNAL(open(QString)));
		connect(lab, SIGNAL(middleClicked(QString)), this, SIGNAL(openInNewTab(QString)));
		connect(lab, SIGNAL(mouseOver(QString)), this, SLOT(setHover(QString)));
		connect(lab, SIGNAL(mouseOut()), this, SLOT(clearHover()));

		ui->layoutFavorites->addWidget(lab);
	}
}
