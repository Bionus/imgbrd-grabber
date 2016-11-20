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
};

#endif // VERTICAL_SCROLL_AREA_H
