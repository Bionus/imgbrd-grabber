#ifndef MD5FIX_H
#define MD5FIX_H

#include <QDialog>
#include <QMap>



namespace Ui
{
	class md5Fix;
}



class md5Fix : public QDialog
{
	Q_OBJECT

	public:
		explicit md5Fix(QMap<QString,QMap<QString,QString> > sites, QWidget *parent = 0);
		~md5Fix();

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::md5Fix *ui;
		QMap<QString,QMap<QString,QString> >	m_sites;
};

#endif // MD5FIX_H
