#include <QApplication>
#include "QBouton.h"

using namespace std;



QBouton::QBouton(QVariant id, QWidget * parent) : QPushButton(parent)
{
	this->_id = id;
}

QVariant QBouton::id()
{ return this->_id; }
void QBouton::setId(QVariant id)
{ this->_id = id; }

void QBouton::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit this->appui(this->_id);
		emit this->appui(this->_id.toString());
		emit this->appui(this->_id.toInt());
	}
	if (event->button() == Qt::RightButton)
	{
		emit this->rightClick(this->_id);
		emit this->rightClick(this->_id.toString());
		emit this->rightClick(this->_id.toInt());
	}
	if (event->button() == Qt::MidButton)
	{
		emit this->middleClick(this->_id);
		emit this->middleClick(this->_id.toString());
		emit this->middleClick(this->_id.toInt());
	}
	event->accept();
}
