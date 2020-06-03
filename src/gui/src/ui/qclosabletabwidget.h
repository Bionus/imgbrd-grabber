#ifndef QCLOSABLE_TAB_WIDGET_H
#define QCLOSABLE_TAB_WIDGET_H

#include <QTabWidget>


class QClosableTabWidget : public QTabWidget
{
	Q_OBJECT

	public:
		explicit QClosableTabWidget(QWidget *parent);
		bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // QCLOSABLE_TAB_WIDGET_H
