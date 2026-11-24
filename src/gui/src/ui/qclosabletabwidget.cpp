#include "ui/qclosabletabwidget.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QTabBar>
#include <QTabWidget>
#include <QtMath>
#include "tabs/search-tab.h"


namespace
{
	class ResponsiveTabBar : public QTabBar
	{
		public:
			explicit ResponsiveTabBar(QWidget *parent = nullptr)
				: QTabBar(parent)
			{
				setElideMode(Qt::ElideRight);
				setExpanding(false);
				setUsesScrollButtons(true);
			}

			QSize tabSizeHint(int index) const override
			{
				QSize size = QTabBar::tabSizeHint(index);
				constexpr int minWidth = 72;
				constexpr int maxWidth = 200;
				int availableWidth = parentWidget() != nullptr ? parentWidget()->width() : width();
				if (const auto *tabs = qobject_cast<const QTabWidget*>(parentWidget())) {
					for (Qt::Corner corner : { Qt::TopLeftCorner, Qt::TopRightCorner }) {
						const QWidget *cornerWidget = tabs->cornerWidget(corner);
						if (cornerWidget != nullptr && cornerWidget->isVisible()) {
							availableWidth -= cornerWidget->width();
						}
					}
				}

				if (availableWidth > 0 && count() > 0) {
					size.setWidth(qBound(minWidth, availableWidth / count(), maxWidth));
				} else {
					size.setWidth(qMin(size.width(), maxWidth));
				}

				return size;
			}

		protected:
			void resizeEvent(QResizeEvent *event) override
			{
				QTabBar::resizeEvent(event);
				updateGeometry();
				update();
			}
	};
}


QClosableTabWidget::QClosableTabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	setTabBar(new ResponsiveTabBar(this));
	tabBar()->installEventFilter(this);
}

bool QClosableTabWidget::eventFilter(QObject *o, QEvent *e)
{
	if (o == tabBar() && e->type() == QEvent::MouseButtonPress) {
		auto *mouseEvent = dynamic_cast<QMouseEvent*>(e);
		if (mouseEvent != nullptr && mouseEvent->button() == Qt::MiddleButton) {
			const int index = tabBar()->tabAt(mouseEvent->pos());
			if (index < 0) {
				return false;
			}

			QWidget *w = widget(index);
			SearchTab *tab = dynamic_cast<SearchTab*>(w);
			const bool hasCloseButton = tabBar()->tabButton(index, QTabBar::LeftSide) != nullptr || tabBar()->tabButton(index, QTabBar::RightSide) != nullptr;

			if (tab != nullptr && !tab->isLocked() && hasCloseButton) {
				w->close();
				return true;
			}
		}
	}

	return QTabWidget::eventFilter(o, e);
}
