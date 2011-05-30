#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QtGui>
#include "mainWindow.h"

namespace Ui
{
    class optionsWindow;
}



class optionsWindow : public QDialog
{
    Q_OBJECT

	public:
		explicit optionsWindow(mainWindow *parent = 0);
		~optionsWindow();

	public slots:
		void updateContainer(QTreeWidgetItem *, QTreeWidgetItem *);
		void on_buttonFolder_clicked();
		void on_buttonColoringArtists_clicked();
		void on_buttonColoringCopyrights_clicked();
		void on_buttonColoringCharacters_clicked();
		void save();

	private:
		mainWindow *m_parent;
		Ui::optionsWindow *ui;
};

#endif // OPTIONSWINDOW_H
