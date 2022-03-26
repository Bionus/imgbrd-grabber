#ifndef CRASH_REPORTER_WINDOW_H
#define CRASH_REPORTER_WINDOW_H

#include <QMainWindow>


namespace Ui
{
	class CrashReporterWindow;
}

class CrashReporterWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit CrashReporterWindow(QWidget *parent = nullptr);
		~CrashReporterWindow() override;

	public slots:
		void restart();
		void sendCrashReport();
		void finished();

	private:
		Ui::CrashReporterWindow *ui;
		bool m_restart;
};

#endif // CRASH_REPORTER_WINDOW_H
