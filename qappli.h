#ifndef QAPPLI_H
#define QAPPLI_H

#include <QApplication>
#include <QMessageBox>

class QAppli : public QApplication
{
	public:
		QAppli(int, char**);
		bool notify(QObject * rec, QEvent * ev);
};

#endif // QAPPLI_H
