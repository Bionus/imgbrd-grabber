#ifndef BLACKLISTFIX_H
#define BLACKLISTFIX_H

#include <QtGui>

namespace Ui {
	class BlacklistFix;
}

class BlacklistFix : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix(QMap<QString,QMap<QString,QString> > sites, QWidget *parent = 0);
		~BlacklistFix();

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		Ui::BlacklistFix *ui;
		QMap<QString,QMap<QString,QString> >	m_sites;
};

#endif // BLACKLISTFIX_H
