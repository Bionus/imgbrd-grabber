#ifndef QCLOSABLETABBAR_H
#define QCLOSABLETABBAR_H

#include <QTabWidget>


class QClosableTabWidget : public QTabWidget
{
	public:
		explicit QClosableTabWidget(QWidget *parent);
		bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // QCLOSABLETABBAR_H
