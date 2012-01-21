#ifndef ADDGROUPWINDOW_H
#define ADDGROUPWINDOW_H

#include <QtGui>
#include "mainwindow.h"
#include "textedit.h"

class AddGroupWindow : public QWidget
{
	Q_OBJECT

	public:
		AddGroupWindow(QString, QStringList, QStringList, mainWindow *parent);

	public slots:
		void ok();

	private:
		mainWindow	*m_parent;
		TextEdit	*m_lineTags;
		QSpinBox	*m_spinPage, *m_spinPP, *m_spinLimit;
		QComboBox	*m_comboDwl, *m_comboSites;
		QStringList m_sites;
};

#endif // ADDGROUPWINDOW_H
