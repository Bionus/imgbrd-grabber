#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QDialog>
#include "image.h"



namespace Ui
{
	class detailsWindow;
}



class detailsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit detailsWindow(Image *img, QWidget *parent = 0);
		~detailsWindow();
		void setTags(QString);

	private:
		Ui::detailsWindow *ui;

};

#endif // DETAILSWINDOW_H
