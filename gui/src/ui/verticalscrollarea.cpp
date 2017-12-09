#include "ui/verticalscrollarea.h"
#include <QScrollBar>


VerticalScrollArea::VerticalScrollArea(QWidget *parent)
	: QScrollArea(parent), m_scrollEnabled(true), m_endOfScroll(0)
{
	setWidgetResizable(true);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	updateWidgetSize();
}

void VerticalScrollArea::resizeEvent(QResizeEvent *event)
{
	updateWidgetSize();
	QScrollArea::resizeEvent(event);
}

void VerticalScrollArea::setScrollEnabled(bool enabled)
{
	if (m_scrollEnabled == enabled)
		return;

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

void VerticalScrollArea::wheelEvent(QWheelEvent* e)
{
	QScrollBar *scrollBar = verticalScrollBar();

	if (scrollBar->value() == scrollBar->maximum())
	{
		m_endOfScroll++;
		if (m_endOfScroll == 3)
		{
			m_endOfScroll = 0;
			emit endOfScrollReached();
		}
	}
	else
	{ m_endOfScroll = 0; }

	QScrollArea::wheelEvent(e);
}
