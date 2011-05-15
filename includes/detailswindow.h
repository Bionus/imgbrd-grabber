#ifndef FAVORITEWINDOW_H
#define FAVORITEWINDOW_H

#include <QtGui>



namespace Ui
{
	class detailsWindow;
}



class detailsWindow : public QWidget
{
	Q_OBJECT

	public:
		explicit detailsWindow(QString);
		~detailsWindow();

	private:
		Ui::detailsWindow *ui;

};

#endif // FAVORITEWINDOW_H
