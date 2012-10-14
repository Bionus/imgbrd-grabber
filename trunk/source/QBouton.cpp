#include <QPaintEvent>
#include <QPainter>
#include "QBouton.h"



QBouton::QBouton(QVariant id, bool resizeInsteadOfCropping, int border, QColor color, QWidget *parent) : QPushButton(parent), _id(id), _resizeInsteadOfCropping(resizeInsteadOfCropping), _np(false), _originalSize(QSize(-1,-1)), _penColor(color), _border(border)
{ }

QBouton::~QBouton()
{ }

QVariant QBouton::id()
{ return _id; }
void QBouton::setId(QVariant id)
{ _id = id; }

void QBouton::paintEvent(QPaintEvent *event)
{
	if (!_resizeInsteadOfCropping && _border == 0)
	{
		QPushButton::paintEvent(event);
		return;
	}
	QRect region = event->rect();
	QPainter *painter = new QPainter(this);
	int p = _border, x = region.x(), y = region.y(), w = iconSize().width(), h = iconSize().height();
	if (_resizeInsteadOfCropping)
	{
		float coef, mcoef;
		coef = float(region.width())/float(w);
		coef = float(region.height())/float(h) < coef ? float(region.height())/float(h) : coef;
		coef = coef > 1 ? 1 : coef;
		mcoef = float(region.width()-2*p)/float(w);
		mcoef = float(region.height()-2*p)/float(h) < mcoef ? float(region.height()-2*p)/float(h) : mcoef;
		mcoef = mcoef > 1 ? 1 : mcoef;
		w = w*coef;
		h = h*coef;
	}
	x += (region.width()-w)/2;
	y += (region.height()-h)/2;
	if (w > h)	{ icon().paint(painter, x+p, y+p, w-2*p, w-2*p, Qt::AlignLeft | Qt::AlignTop); h = h-((h*2*p)/w)+2*p-1; }
	else		{ icon().paint(painter, x+p, y+p, h-2*p, h-2*p, Qt::AlignLeft | Qt::AlignTop); w = w-((w*2*p)/h)+2*p-1; }
	painter->setClipRect(x, y, w, h);
	if (this->isChecked())
	{
		painter->setBrush(QBrush(QColor(0, 0, 255, 128), Qt::Dense4Pattern));
		painter->setPen(Qt::NoPen);
		painter->drawRect(x+p, y+p, w-2*p, h-2*p);
	}
	if (p > 0 && _penColor.isValid())
	{
		QPen pen(_penColor);
		pen.setWidth(p*2);
		painter->setPen(pen);
		painter->drawRect(qMax(x,0), qMax(y,0), qMin(w,size().width()), qMin(h,size().height()));
	}
	painter->end();
}

void QBouton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (event->modifiers() & Qt::ControlModifier)
		{
			this->toggle();
			emit this->toggled(_id, this->isChecked());
			emit this->toggled(_id.toString(), this->isChecked());
			emit this->toggled(_id.toInt(), this->isChecked());
		}
		else
		{
			emit this->appui(_id);
			emit this->appui(_id.toString());
			emit this->appui(_id.toInt());
		}
	}
	if (event->button() == Qt::RightButton)
	{
		emit this->rightClick(_id);
		emit this->rightClick(_id.toString());
		emit this->rightClick(_id.toInt());
	}
	if (event->button() == Qt::MidButton)
	{
		emit this->middleClick(_id);
		emit this->middleClick(_id.toString());
		emit this->middleClick(_id.toInt());
	}
	event->accept();
}
