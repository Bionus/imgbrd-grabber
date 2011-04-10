#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <string>
#include <QtGui>
#include <QtNetwork>

using namespace std;



class QBouton : public QPushButton
{
	Q_OBJECT
 
	public:
		QBouton(int id = 0, QWidget * parent = 0);
		int id();
		void mousePressEvent(QMouseEvent *);
		
	signals:
		void appui(int);
		void rightClick(int);
	
	private:
		int _id;
};

#endif
