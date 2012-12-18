#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include "QBouton.h"
#include "site.h"



namespace Ui
{
	class sourcesWindow;
}



class sourcesWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit sourcesWindow(QList<bool> selected, QMap<QString,Site*> *sites, QWidget *parent = 0);
		~sourcesWindow();
		QList<bool> getSelected();
		int getColumns();

	public slots:
		void valid();
		void closeEvent(QCloseEvent *);
		void checkAll(int check = 2);
		void addSite();
		void insertCheckBox();
		void settingsSite(QString);
		void deleteSite(QString);
		void checkUpdate();
		void checkClicked();

	signals:
		void closed();
		void valid(QList<bool>);

	private:
		Ui::sourcesWindow *ui;
		QList<bool> m_selected;
		QList<QCheckBox*> m_checks;
		QList<QLabel*> m_labels;
		QList<QBouton*> m_buttons;
		QMap<QString,Site*> *m_sites;
};

#endif // SOURCESWINDOW_H
