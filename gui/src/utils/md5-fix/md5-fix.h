#ifndef MD5_FIX_H
#define MD5_FIX_H

#include <QDialog>
#include <QMap>


namespace Ui
{
	class md5Fix;
}


class Profile;

class md5Fix : public QDialog
{
	Q_OBJECT

	public:
		explicit md5Fix(Profile *profile, QWidget *parent = Q_NULLPTR);
		~md5Fix() override;

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonStart_clicked();

	private:
		Ui::md5Fix *ui;
		Profile *m_profile;
};

#endif // MD5_FIX_H
