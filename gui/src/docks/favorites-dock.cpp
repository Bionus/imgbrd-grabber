#include "docks/favorites-dock.h"
#include <QEvent>
#include <QStringList>
#include <ui_favorites-dock.h>
#include "functions.h"
#include "helpers.h"
#include "models/favorite.h"
#include "models/profile.h"
#include "ui/QAffiche.h"


FavoritesDock::FavoritesDock(Profile *profile, QWidget *parent)
	: QWidget(parent), ui(new Ui::FavoritesDock), m_favorites(profile->getFavorites())
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

void FavoritesDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
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
	const bool reverse = ui->comboSortDirection->currentIndex() == 1;
	if (reverse) {
		m_favorites = reversed(m_favorites);
	}

	for (const Favorite &fav : qAsConst(m_favorites)) {
		QAffiche *lab = new QAffiche(QString(fav.getName()), 0, QColor(), this);
		lab->setText(QString(R"(<a href="%1" style="color:black;text-decoration:none;">%2</a>)").arg(fav.getName(), fav.getName()));
		lab->setToolTip("<img src=\"" + fav.getImagePath() + "\" /><br/>" + tr("<b>Name:</b> %1<br/><b>Note:</b> %2 %%<br/><b>Last view:</b> %3").arg(fav.getName(), QString::number(fav.getNote()), fav.getLastViewed().toString(Qt::DefaultLocaleShortDate)));

		connect(lab, SIGNAL(linkActivated(QString)), this, SIGNAL(open(QString)));
		connect(lab, SIGNAL(middleClicked(QString)), this, SIGNAL(openInNewTab(QString)));

		ui->layoutFavorites->addWidget(lab);
	}
}
