#ifndef ADDGROUPWINDOW_H
#define ADDGROUPWINDOW_H

#include <QtGui>
#include "mainWindow.h"
#include "TextEdit.h"

class AddGroupWindow : public QWidget
{
	Q_OBJECT

	public:
		AddGroupWindow(QMap<QString,int>, mainWindow *parent);

	public slots:
		void ok();

	private:
		mainWindow	*m_parent;
		TextEdit	*m_lineTags;
		QSpinBox	*m_spinPage, *m_spinPP, *m_spinLimit;
		QComboBox	*m_comboDwl;
};

#endif // ADDGROUPWINDOW_H
