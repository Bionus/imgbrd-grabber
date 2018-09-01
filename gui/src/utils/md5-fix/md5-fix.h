#ifndef MD5_FIX_H
#define MD5_FIX_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class Md5Fix;
}


class Profile;

class Md5Fix : public QDialog
{
	Q_OBJECT

	public:
		explicit Md5Fix(Profile *profile, QWidget *parent = nullptr);
		~Md5Fix() override;

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonStart_clicked();

	private:
		Ui::Md5Fix *ui;
		Profile *m_profile;
};

#endif // MD5_FIX_H
