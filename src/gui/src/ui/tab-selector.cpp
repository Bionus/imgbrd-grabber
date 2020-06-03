#include "tab-selector.h"
#include <QAction>
#include <QMenu>
#include <QTabWidget>
#include "tabs/search-tab.h"
#include "ui/click-menu.h"


TabSelector::TabSelector(QTabWidget *tabWidget, QPushButton *backButton, QWidget *parent)
	: QPushButton(parent), m_tabWidget(tabWidget), m_backButton(backButton)
{
	m_menu = new ClickMenu(this);
	m_menu->setStyleSheet("QMenu { menu-scrollable: 1; }");
	connect(m_menu, &ClickMenu::aboutToShow, this, &TabSelector::menuAboutToShow);
	connect(m_menu, &ClickMenu::triggered, this, &TabSelector::actionTriggered);
	connect(m_menu, &ClickMenu::triggeredMiddle, this, &TabSelector::actionTriggeredMiddle);
	setMenu(m_menu);

	if (m_backButton != nullptr) {
		connect(m_backButton, &QPushButton::clicked, this, &TabSelector::back);
	}

	connect(m_tabWidget, &QTabWidget::currentChanged, this, &TabSelector::tabChanged);

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
		const QString count = QString::number(m_tabWidget->count() - m_staticTabs.count());
		setText(count);
		setMaximumWidth(23 + 7 * count.length());
	} else {
		setMaximumWidth(20);
	}
}

void TabSelector::tabChanged(int index)
{
	Q_UNUSED(index)

	m_lastTab = nullptr;
	m_backButton->hide();
}

void TabSelector::back()
{
	if (m_lastTab != nullptr) {
		m_tabWidget->setCurrentWidget(m_lastTab);
	}
}

void TabSelector::menuAboutToShow()
{
	m_menu->clear();
	QWidget *current = m_tabWidget->currentWidget();

	// Add static tabs at the beginning
	for (QWidget *widget : m_staticTabs) {
		QAction *action = m_menu->addAction(widget->windowTitle());
		action->setData(QVariant::fromValue(widget));
		if (widget == current) {
			action->setEnabled(false);
		}
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

		QAction *action = m_menu->addAction(widget->windowTitle());
		action->setData(QVariant::fromValue(widget));
		if (widget == current) {
			action->setEnabled(false);
		}
	}
}

void TabSelector::actionTriggered(QAction *action)
{
	auto *widget = action->data().value<QWidget*>();
	if (widget == nullptr) {
		return;
	}

	QWidget *lastTab = m_tabWidget->currentWidget();
	if (widget != lastTab) {
		m_tabWidget->setCurrentWidget(widget);
		m_lastTab = lastTab;

		m_backButton->show();
	}
}

void TabSelector::actionTriggeredMiddle(QAction *action)
{
	auto *widget = action->data().value<QWidget*>();
	if (widget == nullptr) {
		return;
	}

	if (!m_staticTabs.contains(widget)) {
		widget->close();
	}
}
