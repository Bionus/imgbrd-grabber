#include "tab-selector.h"
#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include "tabs/search-tab.h"


TabSelector::TabSelector(QTabWidget *tabWidget, QWidget *parent)
	: QPushButton(parent), m_tabWidget(tabWidget)
{
	m_menu = new QMenu(this);
	m_menu->setStyleSheet("QMenu { menu-scrollable: 1; }");
	connect(m_menu, &QMenu::aboutToShow, this, &TabSelector::menuAboutToShow);
	connect(m_menu, &QMenu::triggered, this, &TabSelector::actionTriggered);
	setMenu(m_menu);

	updateCounter();
}

void TabSelector::setShowTabCount(bool showTabCount)
{
	m_showTabCount = showTabCount;
}

void TabSelector::markStaticTab(QWidget *tab)
{
	m_staticTabs.append(tab);
}

void TabSelector::updateCounter()
{
	if (m_showTabCount) {
		const QString count = QString::number(m_tabWidget->count());
		setText(count);
		setMaximumWidth(23 + 7 * count.length());
	} else {
		setMaximumWidth(20);
	}
}

void TabSelector::menuAboutToShow()
{
	m_menu->clear();

	// Add static tabs at the beginning
	for (QWidget *widget : m_staticTabs) {
		m_menu->addAction(widget->windowTitle())->setData(QVariant::fromValue(widget));
	}

	// Add separator if necessary
	if (m_tabWidget->count() > m_staticTabs.count()) {
		m_menu->addSeparator();
	}

	// Add "dynamic" tabs
	for (int i = 0; i < m_tabWidget->count(); ++i) {
		QWidget *widget = m_tabWidget->widget(i);
		if (m_staticTabs.contains(widget)) {
			continue;
		}

		m_menu->addAction(widget->windowTitle())->setData(QVariant::fromValue(widget));
	}
}

void TabSelector::actionTriggered(QAction *action)
{
	QWidget *widget = action->data().value<QWidget*>();
	if (widget == nullptr) {
		return;
	}

	m_tabWidget->setCurrentWidget(widget);
}
