#ifndef VERTICAL_SCROLL_AREA_H
#define VERTICAL_SCROLL_AREA_H

#include <QResizeEvent>
#include <QScrollArea>
#include <QWheelEvent>


class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		explicit VerticalScrollArea(QWidget *parent = nullptr);
		void resizeEvent(QResizeEvent *event) override;
		void setScrollEnabled(bool enabled);
		void wheelEvent(QWheelEvent* e) override;

	protected:
		void updateWidgetSize();

	signals:
		void endOfScrollReached();

	private:
		bool m_scrollEnabled;
		int m_endOfScroll;
};

#endif // VERTICAL_SCROLL_AREA_H
