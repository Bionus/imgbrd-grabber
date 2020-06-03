#include "dock.h"


Dock::Dock(QWidget *parent)
	: QWidget(parent)
{}

void Dock::tabChanged(SearchTab *tab)
{
	Q_UNUSED(tab);
}
