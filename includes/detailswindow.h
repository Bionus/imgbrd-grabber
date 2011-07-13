#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QtGui>
#include "image.h"



namespace Ui
{
	class detailsWindow;
}



class detailsWindow : public QWidget
{
	Q_OBJECT

	public:
		explicit detailsWindow(Image *);
		~detailsWindow();
		void setTags(QString);

	private:
		Ui::detailsWindow *ui;

};

#endif // DETAILSWINDOW_H
