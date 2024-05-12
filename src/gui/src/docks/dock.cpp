#include "dock.h"


Dock::Dock(QWidget *parent)
	: QWidget(parent), m_currentTab(nullptr)
{}

void Dock::tabChanged(SearchTab *tab)
{
	m_currentTab = tab;
}
