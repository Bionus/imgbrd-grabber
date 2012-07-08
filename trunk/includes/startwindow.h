#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>



namespace Ui
{
	class startWindow;
}



class startWindow : public QDialog
{
	Q_OBJECT

	public:
		startWindow(QWidget *parent);
		~startWindow();

	public slots:
		void openOptions();

	private:
		Ui::startWindow *ui;

};

#endif // STARTWINDOW_H
