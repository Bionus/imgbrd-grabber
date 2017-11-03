#ifndef DETAILSWINDOW_H
#define DETAILSWINDOW_H

#include <QDialog>


namespace Ui
{
	class detailsWindow;
}


class Image;
class Profile;

class detailsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit detailsWindow(Profile *profile, QWidget *parent = Q_NULLPTR);
		~detailsWindow() override;
		void setTags(QString tags);
		void setImage(QSharedPointer<Image> image);

	private:
		Ui::detailsWindow *ui;
		Profile *m_profile;

};

#endif // DETAILSWINDOW_H
