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

		if (!m_scrollEnabled)
			widget()->setMaximumHeight(height());
	}

	QScrollArea::resizeEvent(event);
}

void VerticalScrollArea::setScrollEnabled(bool enabled)
{
	m_scrollEnabled = enabled;
	setVerticalScrollBarPolicy(enabled ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}
