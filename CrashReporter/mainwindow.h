#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>



namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	public slots:
		void restart();
		void sendCrashReport();
		void finished();

	private:
		Ui::MainWindow *ui;
		bool m_restart;
};

#endif // MAINWINDOW_H
