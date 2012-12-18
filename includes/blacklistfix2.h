#ifndef BLACKLISTFIX2_H
#define BLACKLISTFIX2_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QDir>
#include "page.h"
#include "site.h"



namespace Ui
{
	class BlacklistFix2;
}



class BlacklistFix2 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix2(QMap<QString,Site*> sites, QString folder, QString filename, QStringList blacklist, Site *site, QWidget *parent = 0);
		~BlacklistFix2();
		QList<QTreeWidgetItem*> mkTree(QDir);
		QString remakePath(QTreeWidgetItem*);
		QStringList getAllFiles(QString);

	private slots:
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();
		void getAll(Page *p = NULL);

	private:
		Ui::BlacklistFix2						*ui;
		QString									m_folder, m_filename;
		QStringList								m_blacklist;
		Site									*m_site;
		QMap<QString,QMap<QString,QString> >	m_getAll;
		QList<QMap<QString,QString> >			m_details;
		QMap<QString,Site*>						m_sites;
};

#endif // BLACKLISTFIX2_H
