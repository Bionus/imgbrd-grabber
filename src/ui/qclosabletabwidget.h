#ifndef QCLOSABLETABBAR_H
#define QCLOSABLETABBAR_H

#include <QTabWidget>
#include <QMouseEvent>


class QClosableTabWidget : public QTabWidget
{
	public:
		QClosableTabWidget(QWidget *parent);
		bool eventFilter(QObject *o, QEvent *e);
};

#endif // QCLOSABLETABBAR_H
