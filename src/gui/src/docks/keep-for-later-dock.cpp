#include "docks/keep-for-later-dock.h"
#include <QCursor>
#include <QEvent>
#include <QStringList>
#include <ui_keep-for-later-dock.h>
#include "helpers.h"
#include "models/profile.h"
#include "tag-context-menu.h"
#include "tags/tag.h"
#include "ui/QAffiche.h"


KeepForLaterDock::KeepForLaterDock(Profile *profile, QWidget *parent)
	: Dock(parent), ui(new Ui::KeepForLaterDock), m_profile(profile)
{
	ui->setupUi(this);

	connect(m_profile, &Profile::keptForLaterChanged, this, &KeepForLaterDock::refresh);

	refresh();
}

KeepForLaterDock::~KeepForLaterDock()
{
	close();
	delete ui;
}

void KeepForLaterDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void KeepForLaterDock::refresh()
{
	clearLayout(ui->layoutTags);

	int i = 0;
	QStringList kfl = m_profile->getKeptForLater();
	for (const QString &tag : kfl) {
		auto *taglabel = new QAffiche(QString(tag), 0, QColor(), this);
		taglabel->setText(tag);
		taglabel->setCursor(Qt::PointingHandCursor);

		if (i++ % 2 == 1) {
			taglabel->setStyleSheet("QAffiche { background-color: rgba(128, 128, 128, 10%); }");
		}

		connect(taglabel, SIGNAL(clicked(QString)), this, SIGNAL(open(QString)));
		connect(taglabel, SIGNAL(middleClicked(QString)), this, SIGNAL(openInNewTab(QString)));
		connect(taglabel, SIGNAL(mouseOver(QString)), this, SLOT(setHover(QString)));
		connect(taglabel, SIGNAL(mouseOut()), this, SLOT(clearHover()));

		ui->layoutTags->addWidget(taglabel);
	}
}

void KeepForLaterDock::setHover(const QString &tag)
{
	m_isHover = true;
	m_hover = tag;
}

void KeepForLaterDock::clearHover()
{
	m_isHover = false;
}

void KeepForLaterDock::contextMenu(const QPoint &pos)
{
	Q_UNUSED(pos);

	if (!m_isHover) {
		return;
	}

	TagContextMenu *menu = new TagContextMenu(m_hover, {}, {}, m_profile, false, this);
	connect(menu, &TagContextMenu::openNewTab, this, &KeepForLaterDock::emitOpenInNewTab);
	menu->exec(QCursor::pos());
}

void KeepForLaterDock::emitOpenInNewTab()
{
	emit openInNewTab(m_hover);
}
