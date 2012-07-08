#include <QMouseEvent>
#include "QAffiche.h"

using namespace std;



QAffiche::QAffiche(QVariant id, int border, QColor color, QWidget *parent) : QLabel(parent)
{
	m_pressed = false;
	m_id = id;
	m_border = border;
	m_color = color;
	setText("");
}
QAffiche::~QAffiche()
{  }

/*void QAffiche::paintEvent(QPaintEvent *event)
{
	QRect region = event->rect();
	QPainter *painter = new QPainter(this);
	int x = region.x(), y = region.y(), w = pixmap()->width(), h = pixmap()->height();
	float coef, mcoef;
	coef = float(region.width())/float(w);
	coef = float(region.height())/float(h) < coef ? float(region.height())/float(h) : coef;
	coef = coef > 1 ? 1 : coef;
	mcoef = float(region.width()-2*m_border)/float(w);
	mcoef = float(region.height()-2*m_border)/float(h) < mcoef ? float(region.height()-2*m_border)/float(h) : mcoef;
	mcoef = mcoef > 1 ? 1 : mcoef;
	w = w*coef;
	h = h*coef;
	x += (region.width()-w)/2;
	y += (region.height()-h)/2;
	if (w > h)	{ painter->drawPixmap(QRect(x+m_border, y+m_border, w-2*m_border, w-2*m_border), *pixmap(), pixmap()->rect()); h = h-((h*2*m_border)/w)+2*m_border-1; }
	else		{ painter->drawPixmap(QRect(x+m_border, y+m_border, h-2*m_border, h-2*m_border), *pixmap(), pixmap()->rect()); w = w-((w*2*m_border)/h)+2*m_border-1; }
	painter->setClipRect(x, y, w, h);
	if (m_border > 0 && m_color.isValid())
	{
		QPen pen(m_color);
		pen.setWidth(m_border*2);
		painter->setPen(pen);
		painter->drawLine(x,y,x+w,y);
		painter->drawLine(x,y,x,y+h);
		painter->drawLine(x,y+h,x+w,y+h);
		painter->drawLine(x+w,y,x+w,y+h);
	}
}*/

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
