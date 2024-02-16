#include "ui/qclosabletabwidget.h"
#include <QMouseEvent>
#include <QTabBar>
#include "tabs/search-tab.h"


QClosableTabWidget::QClosableTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	tabBar()->installEventFilter(this);
}

bool QClosableTabWidget::eventFilter(QObject *o, QEvent *e)
{
	if (o == tabBar() && e->type() == QEvent::MouseButtonPress) {
		auto *mouseEvent = dynamic_cast<QMouseEvent*>(e);
		if (mouseEvent != nullptr && mouseEvent->button() == Qt::MiddleButton) {
			const int index = tabBar()->tabAt(mouseEvent->pos());
			QWidget *w = widget(index);
			SearchTab *tab = dynamic_cast<SearchTab*>(w);

			// Non-closable tabs have a maximum width of 16777214 (default: 16777215)
			if (tab != nullptr && !tab->isLocked() && w->maximumWidth() != 16777214) {
				w->close();
				removeTab(index);
				return true;
			}
		}
	}

	return QTabWidget::eventFilter(o, e);
}
