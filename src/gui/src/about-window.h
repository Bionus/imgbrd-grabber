#ifndef ABOUT_WINDOW_H
#define ABOUT_WINDOW_H

#include <QDialog>
#include <QString>
#include <QWidget>
#include "updater/program-updater.h"


namespace Ui
{
	class AboutWindow;
}


class AboutWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit AboutWindow(const QString &v, QWidget *parent = nullptr);
		~AboutWindow() override;

	public slots:
		void finished(const QString &newVersion, bool available);

	private:
		Ui::AboutWindow *ui;
		ProgramUpdater m_updater;
};

#endif // ABOUT_WINDOW_H
