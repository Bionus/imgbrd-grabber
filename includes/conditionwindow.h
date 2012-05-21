#ifndef CONDITIONWINDOW_H
#define CONDITIONWINDOW_H

#include <QtGui>
#include "functions.h"



namespace Ui
{
	class conditionWindow;
}

class conditionWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit conditionWindow(QWidget *parent = 0);
		~conditionWindow();

	public slots:
		void accept();

	signals:
		void validated(QString, QString);

	private:
		Ui::conditionWindow *ui;
};

#endif // CONDITIONWINDOW_H
