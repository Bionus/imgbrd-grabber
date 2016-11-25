#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <QDialog>
#include <QNetworkReply>
#include "updater/program-updater.h"


namespace Ui
{
	class AboutWindow;
}


class AboutWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit AboutWindow(QString v, QWidget *parent = Q_NULLPTR);
		~AboutWindow();

	public slots:
		void finished(QString newVersion, bool available);

	private:
		Ui::AboutWindow *ui;
		ProgramUpdater m_updater;
};

#endif // ABOUT_WINDOW_H
