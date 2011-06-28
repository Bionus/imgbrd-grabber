#include "qappli.h"
#include <QDebug>

QAppli::QAppli(int i, char** c) : QApplication(i, c)
{ }

bool QAppli::notify(QObject * rec, QEvent * ev)
{
	try
	{
		return QApplication::notify(rec,ev);
	}
	catch (std::exception & e)
	{
		QMessageBox::warning(0, tr("An error occurred"), e.what());
	}
	catch (...)
	{
		QMessageBox::warning(0, tr("An unexpected error occurred"), tr("This is likely a bug."));
	}
	qDebug() << 1;
	return false;
}
