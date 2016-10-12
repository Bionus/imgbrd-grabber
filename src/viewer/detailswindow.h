#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QDialog>
#include "models/image.h"



namespace Ui
{
	class detailsWindow;
}



class detailsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit detailsWindow(QWidget *parent = 0);
		~detailsWindow();
		void setTags(QString);
		void setImage(QSharedPointer<Image> image);

	private:
		Ui::detailsWindow *ui;

};

#endif // DETAILSWINDOW_H
