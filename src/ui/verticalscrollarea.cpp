#include <QEvent>
#include <QScrollBar>
#include "verticalscrollarea.h"


VerticalScrollArea::VerticalScrollArea(QWidget *parent)
	: QScrollArea(parent)
{
	setWidgetResizable(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void VerticalScrollArea::resizeEvent(QResizeEvent *event)
{
	if (widget())
	{
		int maxWidth = width();
		if (verticalScrollBar()->isVisible())
			maxWidth -= verticalScrollBar()->width();
		widget()->setMaximumWidth(maxWidth);
	}

	QScrollArea::resizeEvent(event);
}
