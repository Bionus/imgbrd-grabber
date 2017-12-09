#ifndef DETAILS_WINDOW_H
#define DETAILS_WINDOW_H

#include <QDialog>


namespace Ui
{
	class DetailsWindow;
}


class Image;
class Profile;

class DetailsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit DetailsWindow(Profile *profile, QWidget *parent = Q_NULLPTR);
		~DetailsWindow() override;
		void setImage(QSharedPointer<Image> image);

	private:
		Ui::DetailsWindow *ui;
		Profile *m_profile;
};

#endif // DETAILS_WINDOW_H
