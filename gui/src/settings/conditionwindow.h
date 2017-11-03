#ifndef CONDITIONWINDOW_H
#define CONDITIONWINDOW_H

#include <QDialog>


namespace Ui
{
	class conditionWindow;
}

class conditionWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit conditionWindow(QWidget *parent = Q_NULLPTR);
		~conditionWindow() override;

	public slots:
		void accept() override;

	signals:
		void validated(QString, QString, QString);

	private:
		Ui::conditionWindow *ui;
};

#endif // CONDITIONWINDOW_H
