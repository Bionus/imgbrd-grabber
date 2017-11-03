#ifndef VERTICAL_SCROLL_AREA_H
#define VERTICAL_SCROLL_AREA_H

#include <QScrollArea>
#include <QResizeEvent>
#include <QWheelEvent>


class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		explicit VerticalScrollArea(QWidget *parent = Q_NULLPTR);
		void resizeEvent(QResizeEvent *event) override;
		void setScrollEnabled(bool enabled);
		void wheelEvent(QWheelEvent* event) override;

	protected:
		void updateWidgetSize();

	signals:
		void endOfScrollReached();

	private:
		bool m_scrollEnabled;
		int m_endOfScroll;
};

#endif // VERTICAL_SCROLL_AREA_H
