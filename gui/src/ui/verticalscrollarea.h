#ifndef VERTICAL_SCROLL_AREA_H
#define VERTICAL_SCROLL_AREA_H

#include <QScrollArea>
#include <QResizeEvent>
#include <QWheelEvent>


class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		explicit VerticalScrollArea(QWidget *parent = 0);
		virtual void resizeEvent(QResizeEvent *event);
		void setScrollEnabled(bool enabled);
		void wheelEvent(QWheelEvent* event);

	protected:
		void updateWidgetSize();

	signals:
		void endOfScrollReached();

	private:
		bool m_scrollEnabled;
		int m_endOfScroll;
};

#endif // VERTICAL_SCROLL_AREA_H
