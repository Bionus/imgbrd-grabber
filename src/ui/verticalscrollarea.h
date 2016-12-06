#ifndef VERTICAL_SCROLL_AREA_H
#define VERTICAL_SCROLL_AREA_H

#include <QScrollArea>
#include <QResizeEvent>


class VerticalScrollArea : public QScrollArea
{
	Q_OBJECT

	public:
		explicit VerticalScrollArea(QWidget *parent = 0);
		virtual void resizeEvent(QResizeEvent *event);
		void setScrollEnabled(bool enabled);

	protected:
		void updateWidgetSize();

	private:
		bool m_scrollEnabled;
};

#endif // VERTICAL_SCROLL_AREA_H
