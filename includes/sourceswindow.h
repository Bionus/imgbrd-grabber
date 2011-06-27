#ifndef SOURCESWINDOW_H
#define SOURCESWINDOW_H

#include <QtGui>
#include "QBouton.h"
#include "functions.h"



namespace Ui
{
    class sourcesWindow;
}

class sourcesWindow : public QDialog
{
    Q_OBJECT

	public:
		explicit sourcesWindow(QList<bool> selected, QStringMapMap *sites, QWidget *parent = 0);
		~sourcesWindow();
		QList<bool> getSelected();
		int getColumns();

	public slots:
		void valid();
		void closeEvent(QCloseEvent *);
		void checkAll(int check = 2);
		void addSite();
		void insertCheckBox();
		void deleteSite(QVariant);

	signals:
		void closed(sourcesWindow *);
		void valid(sourcesWindow *);

	private:
		Ui::sourcesWindow *ui;
		QList<bool> m_selected;
		QList<QCheckBox*> m_checks;
		QList<QLabel*> m_labels;
		QList<QBouton*> m_buttons;
		QStringMapMap *m_sites;
};

#endif // SOURCESWINDOW_H
