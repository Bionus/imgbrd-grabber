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
		explicit detailsWindow(Profile *profile, QWidget *parent = 0);
		~detailsWindow();
		void setTags(QString tags);
		void setImage(QSharedPointer<Image> image);

	private:
		Ui::detailsWindow *ui;
		Profile *m_profile;

};

#endif // DETAILSWINDOW_H
