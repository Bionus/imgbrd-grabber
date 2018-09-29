#ifndef CLICK_MENU_H
#define CLICK_MENU_H

#include <QMenu>


class QWidget;

class ClickMenu : public QMenu
{
	Q_OBJECT

	public:
		explicit ClickMenu(QWidget *parent = nullptr);

	protected:
		void mouseReleaseEvent(QMouseEvent *event) override;

	signals:
		void triggeredMiddle(QAction *action);
};

#endif // CLICK_MENU_H
