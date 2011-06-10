#include <QApplication>
#include "QBouton.h"

using namespace std;



QBouton::QBouton(int id, QWidget * parent) : QPushButton(parent)
{
	this->_id = id;
}

int QBouton::id()
{ return this->_id; }
void QBouton::setId(int id)
{ this->_id = id; }

void QBouton::mousePressEvent(QMouseEvent * event)
{
	if (event->button()==Qt::LeftButton)
		emit this->appui(this->_id);
	if (event->button()==Qt::RightButton)
		emit this->rightClick(this->_id);
	event->accept();
}
