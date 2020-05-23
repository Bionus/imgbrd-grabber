#include "docks/wiki-dock.h"
#include <QEvent>
#include <ui_wiki-dock.h>
#include "tabs/search-tab.h"


WikiDock::WikiDock(QWidget *parent)
	: Dock(parent), ui(new Ui::WikiDock)
{
	ui->setupUi(this);

	connect(ui->labelWiki, &QLabel::linkActivated, this, WikiDock::open);
}

WikiDock::~WikiDock()
{
	close();
	delete ui;
}

void WikiDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void WikiDock::tabChanged(SearchTab *tab)
{
	m_tab = tab;

	// Only keep the last active tab connected
	if (m_connection) {
		disconnect(m_connection);
	}
	m_connection = connect(tab, &SearchTab::wikiChanged, this, &WikiDock::refresh);

	refresh();
}

void WikiDock::refresh()
{
	static const QString style = "<style>.title { font-weight: bold; } ul { margin-left: -30px; }</style>";
	ui->labelWiki->setText(style + m_tab->wiki());
}
