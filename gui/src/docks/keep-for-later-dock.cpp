#include "docks/keep-for-later-dock.h"
#include <QEvent>
#include <QStringList>
#include <ui_keep-for-later-dock.h>
#include "helpers.h"
#include "models/profile.h"
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

	QStringList kfl = m_profile->getKeptForLater();
	for (const QString &tag : kfl) {
		auto *taglabel = new QAffiche(QString(tag), 0, QColor(), this);
		taglabel->setText(QString(R"(<a href="%1" style="color:black;text-decoration:none;">%1</a>)").arg(tag));
		taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

		connect(taglabel, SIGNAL(linkActivated(QString)), this, SIGNAL(open(QString)));
		connect(taglabel, SIGNAL(middleClicked(QString)), this, SIGNAL(openInNewTab(QString)));

		ui->layoutTags->addWidget(taglabel);
	}
}
