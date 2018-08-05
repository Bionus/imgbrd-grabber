#include "ui/qclosabletabwidget.h"
#include <QMouseEvent>
#include <QTabBar>


QClosableTabWidget::QClosableTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	tabBar()->installEventFilter(this);
}

bool QClosableTabWidget::eventFilter(QObject *o, QEvent *e)
{
	if (o == tabBar() && e->type() == QEvent::MouseButtonPress)
	{
		auto *mouseEvent = dynamic_cast<QMouseEvent*>(e);
		if (mouseEvent != nullptr && mouseEvent->button() == Qt::MiddleButton)
		{
			const int index = tabBar()->tabAt(mouseEvent->pos());
			QWidget *w = widget(index);

			// Non-closable tabs have a maximum width of 16777214 (default: 16777215)
			if (w->maximumWidth() != 16777214)
			{
				w->deleteLater();
				removeTab(index);
				return true;
			}
		}
	}

	return QTabWidget::eventFilter(o, e);
}
