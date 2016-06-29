#include <QTabBar>
#include "qclosabletabwidget.h"


QClosableTabWidget::QClosableTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	tabBar()->installEventFilter(this);
}

bool QClosableTabWidget::eventFilter(QObject *o, QEvent *e)
{
    if (o == tabBar() && e->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        if (mouseEvent->button() == Qt::MiddleButton)
        {
            int index = tabBar()->tabAt(mouseEvent->pos());
            widget(index)->deleteLater();
            removeTab(index);
            return true;
        }
	}

	return QTabWidget::eventFilter(o, e);
}
