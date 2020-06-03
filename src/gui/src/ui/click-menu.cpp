#include "ui/click-menu.h"
#include <QMouseEvent>


ClickMenu::ClickMenu(QWidget *parent)
	: QMenu(parent)
{}

void ClickMenu::mouseReleaseEvent(QMouseEvent *event)
{
	QAction *action = activeAction();
	if (action == nullptr) {
		QMenu::mouseReleaseEvent(event);
		return;
	}

	if (event->button() == Qt::MiddleButton) {
		emit triggeredMiddle(action);
	} else {
		action->trigger();
	}

	close();
	event->accept();
}
