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
		void on_lineFilename_textChanged(QString);
		void on_buttonFolder_clicked();
		void on_lineColoringArtists_textChanged();
		void on_lineColoringCopyrights_textChanged();
		void on_lineColoringCharacters_textChanged();
		void on_lineColoringModels_textChanged();
		void on_lineColoringGenerals_textChanged();
		void on_buttonColoringArtists_clicked();
		void on_buttonColoringCopyrights_clicked();
		void on_buttonColoringCharacters_clicked();
		void on_buttonColoringModels_clicked();
		void on_buttonColoringGenerals_clicked();
		void save();

	private:
		mainWindow *m_parent;
		Ui::optionsWindow *ui;
};

#endif // OPTIONSWINDOW_H
