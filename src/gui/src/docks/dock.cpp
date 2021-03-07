#include "dock.h"


Dock::Dock(QWidget *parent)
	: QWidget(parent)
{}

void Dock::tabChanged(SearchTab *tab)
{
	m_currentTab = tab;
}
