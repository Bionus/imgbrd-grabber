#ifndef MD5_DATABASE_CONVERTER_H
#define MD5_DATABASE_CONVERTER_H

#include <QDialog>


namespace Ui
{
	class Md5DatabaseConverter;
}


class Md5DatabaseSqlite;
class Md5DatabaseText;
class Profile;
class Site;

class Md5DatabaseConverter : public QDialog
{
	Q_OBJECT

	public:
		explicit Md5DatabaseConverter(Profile *profile, QWidget *parent = nullptr);
		~Md5DatabaseConverter() override;

	private slots:
		void start();
		void cancel();

	private:
		Ui::Md5DatabaseConverter *ui;
		Profile *m_profile;
		Md5DatabaseSqlite *m_dbSqlite;
		Md5DatabaseText *m_dbText;
};

#endif // MD5_DATABASE_CONVERTER_H
