#include "verticalscrollarea.h"
#include <QEvent>
#include <QScrollBar>


VerticalScrollArea::VerticalScrollArea(QWidget *parent)
	: QScrollArea(parent)
{
	setWidgetResizable(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	setScrollEnabled(true);
}

void VerticalScrollArea::resizeEvent(QResizeEvent *event)
{
	updateWidgetSize();
	QScrollArea::resizeEvent(event);
}

void VerticalScrollArea::setScrollEnabled(bool enabled)
{
	m_scrollEnabled = enabled;
	setVerticalScrollBarPolicy(enabled ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
	updateWidgetSize();
}

void VerticalScrollArea::updateWidgetSize()
{
	if (widget())
	{
		int maxWidth = width();
		if (m_scrollEnabled && verticalScrollBar()->isVisible())
			maxWidth -= verticalScrollBar()->width();
		widget()->setMaximumWidth(maxWidth);

		widget()->setMaximumHeight(m_scrollEnabled ? QWIDGETSIZE_MAX : height());
	}
}
