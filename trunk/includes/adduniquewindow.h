#ifndef ADDUNIQUEWINDOW_H
#define ADDUNIQUEWINDOW_H

#include <QtGui>
#include <QtNetwork>
#include "mainwindow.h"



class AddUniqueWindow : public QWidget
{
	Q_OBJECT

	public:
		AddUniqueWindow(QMap<QString,QStringList>, mainWindow *parent);

	public slots:
		void ok();
		void replyFinished(QNetworkReply *r);

	private:
		mainWindow					*m_parent;
		QMap<QString,QStringList>	m_sites;
		QLineEdit					*m_lineMd5, *m_lineId;
		QComboBox					*m_comboSite;
};

#endif // ADDUNIQUEWINDOW_H
