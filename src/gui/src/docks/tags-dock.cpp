#include "docks/tags-dock.h"
#include <QCursor>
#include <QEvent>
#include <QStringList>
#include <QUrl>
#include <ui_tags-dock.h>
#include "helpers.h"
#include "tabs/search-tab.h"
#include "tag-context-menu.h"
#include "tags/tag-stylist.h"
#include "ui/QAffiche.h"


TagsDock::TagsDock(Profile *profile, QWidget *parent)
	: Dock(parent), ui(new Ui::TagsDock), m_profile(profile)
{
	ui->setupUi(this);
}

TagsDock::~TagsDock()
{
	close();
	delete ui;
}

void TagsDock::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		ui->retranslateUi(this);
	}

	QWidget::changeEvent(event);
}

void TagsDock::tabChanged(SearchTab *tab)
{
	Dock::tabChanged(tab);

	// Only keep the last active tab connected
	if (m_connection) {
		disconnect(m_connection);
	}
	m_connection = connect(tab, &SearchTab::tagsChanged, this, &TagsDock::refresh);

	refresh();
}

void TagsDock::refresh()
{
	clearLayout(ui->layoutTags);

	QAffiche *taglabel = new QAffiche(QVariant(), 0, QColor(), this);
	taglabel->setText(TagStylist(m_profile).stylished(m_currentTab->results(), true, true).join("<br/>"));
	taglabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);

	connect(taglabel, SIGNAL(middleClicked(QString)), this, SIGNAL(open(QString)));
	connect(taglabel, SIGNAL(linkActivated(QString)), this, SIGNAL(openInNewTab(QString)));
	connect(taglabel, &QAffiche::linkHovered, this, &TagsDock::linkHovered);

	// Context menu
	taglabel->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(taglabel, &QWidget::customContextMenuRequested, this, &TagsDock::contextMenu);

	ui->layoutTags->addWidget(taglabel);
}

void TagsDock::linkHovered(const QString &tag)
{
	m_link = QUrl::fromPercentEncoding(tag.toUtf8());
}

void TagsDock::contextMenu()
{
	if (m_link.isEmpty()) {
		return;
	}

	const QList<Site*> sites = m_currentTab->loadSites();
	TagContextMenu *menu = new TagContextMenu(m_link, m_currentTab->results(), QUrl(), m_profile, sites, false, this);
	connect(menu, &TagContextMenu::openNewTab, this, &TagsDock::emitOpenInNewTab);
	menu->exec(QCursor::pos());
}

void TagsDock::emitOpenInNewTab()
{
	emit openInNewTab(m_link);
}
