#include <QApplication>
#include "QAffiche.h"

using namespace std;



QAffiche::QAffiche(QWidget *parent) : QLabel(parent)
{
	m_pressed = false;
	setText("");
}
QAffiche::~QAffiche()
{  }


void QAffiche::mouseDoubleClickEvent(QMouseEvent* e)
{
	if(e->button() == Qt::LeftButton)
	{ emit doubleClicked(); }
	QLabel::mouseDoubleClickEvent(e);
}


void QAffiche::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::LeftButton)
	{ m_pressed = true; }
	else
	{ m_pressed = false; }
	emit pressed();
}


void QAffiche::mouseReleaseEvent(QMouseEvent* e)
{
	if(m_pressed && e->button() == Qt::LeftButton && hitLabel(e->pos()))
	{ emit clicked(); }
	m_pressed = false;
	emit released();

}

void QAffiche::enterEvent(QEvent* e)
{
	QLabel::enterEvent(e);
	emit mouseOver();
}

void QAffiche::leaveEvent(QEvent* e)
{
	QLabel::leaveEvent(e);
	emit mouseOut();
}

bool QAffiche::hitLabel(const QPoint &p)
{ return rect().contains(p); }

void QAffiche::setImage(QImage image)
{
	this->setPixmap(QPixmap::fromImage(image));
}
void QAffiche::setImage(QPixmap image)
{
	this->setPixmap(image);
}
