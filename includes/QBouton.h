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
		QBouton(QVariant id = 0, QWidget * parent = 0);
		QVariant id();
		void mousePressEvent(QMouseEvent *);

	public slots:
		void setId(QVariant);
		
	signals:
		void appui(QVariant);
		void rightClick(QVariant);
		void appui(int);
		void rightClick(int);
		void appui(QString);
		void rightClick(QString);
	
	private:
		QVariant _id;
};

#endif
