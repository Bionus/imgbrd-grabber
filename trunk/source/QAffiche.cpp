#include <QApplication>
#include "QAffiche.h"

using namespace std;



QAffiche::QAffiche(QVariant id, QWidget *parent) : QLabel(parent)
{
	m_pressed = false;
	m_id = id;
	setText("");
}
QAffiche::~QAffiche()
{  }


void QAffiche::mouseDoubleClickEvent(QMouseEvent* e)
{
	if(e->button() == Qt::LeftButton)
	{
		emit doubleClicked();
		emit doubleClicked(m_id.toInt());
	}
	QLabel::mouseDoubleClickEvent(e);
}


void QAffiche::mousePressEvent(QMouseEvent* e)
{
	if(e->button() == Qt::LeftButton)
	{ m_pressed = true; }
	else
	{ m_pressed = false; }
	emit pressed();
	emit pressed(m_id.toInt());
}


void QAffiche::mouseReleaseEvent(QMouseEvent* e)
{
	if(m_pressed && e->button() == Qt::LeftButton && hitLabel(e->pos()))
	{
		emit clicked();
		emit clicked(m_id.toInt());
		emit clicked(m_id.toString());
	}
	m_pressed = false;
	emit released();
	emit released(m_id.toInt());

}

void QAffiche::enterEvent(QEvent* e)
{
	QLabel::enterEvent(e);
	emit mouseOver();
	emit mouseOver(m_id.toInt());
}

void QAffiche::leaveEvent(QEvent* e)
{
	QLabel::leaveEvent(e);
	emit mouseOut();
	emit mouseOut(m_id.toInt());
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
