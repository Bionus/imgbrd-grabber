#include <QPaintEvent>
#include <QPainter>
#include "QBouton.h"
#include <QDebug>



QBouton::QBouton(QVariant id, bool resizeInsteadOfCropping, int border, QColor color, QWidget *parent)
	: QPushButton(parent), _id(id), _resizeInsteadOfCropping(resizeInsteadOfCropping), _np(false), _originalSize(QSize(-1,-1)), _penColor(color), _border(border)
{ }

QBouton::~QBouton()
{ }

void QBouton::scale(QPixmap &image, float scale)
{
	QSize size;
	if (scale > 1.00001f)
	{
		size = image.size() * scale;
		setIcon(image.scaled(size));
	}
	else
	{
		setIcon(image);
		size = image.size();
	}
	setIconSize(size);
	resize(size);
}

QVariant QBouton::id()
{ return _id; }
void QBouton::setId(QVariant id)
{ _id = id; }

void QBouton::paintEvent(QPaintEvent *event)
{
	// Used for normal buttons
	if (!_resizeInsteadOfCropping && _border == 0)
	{
		QPushButton::paintEvent(event);
		return;
	}

	QRect region = event->rect();
	int p = _border, x = region.x(), y = region.y(), w = iconSize().width(), h = iconSize().height();

	// Ignore invalid images
	if (w == 0 || h == 0)
		return;

	QPainter painter(this);

	// Calculate ratio to resize by keeping proportions
	if (_resizeInsteadOfCropping)
	{
		float coef = qMin(1.0f, qMin(float(region.width()) / float(w), float(region.height()) / float(h)));
		w *= coef;
		h *= coef;
	}
	// Center the image
	bool center = true;
	if (center)
	{
		x += (region.width() - w) / 2;
		y += (region.height() - h) / 2;
	}

	// Draw image
	QIcon::Mode mode = this->isChecked() ? QIcon::Selected : QIcon::Normal;
	if (w > h)
	{
		icon().paint(&painter, x+p, y+p, w-2*p, w-2*p, Qt::AlignLeft | Qt::AlignTop, mode);
		h = h-((h*2*p)/w)+2*p-1;
	}
	else
	{
		icon().paint(&painter, x+p, y+p, h-2*p, h-2*p, Qt::AlignLeft | Qt::AlignTop, mode);
		w = w-((w*2*p)/h)+2*p-1;
	}

	// Clip borders overflows
	painter.setClipRect(x, y, w, h);

	// Draw borders
	if (p > 0 && _penColor.isValid())
	{
		QPen pen(_penColor);
		pen.setWidth(p*2);
		painter.setPen(pen);
		painter.drawRect(qMax(x,0), qMax(y,0), qMin(w,size().width()), qMin(h,size().height()));
	}
}

void QBouton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (event->modifiers() & Qt::ControlModifier)
		{
			this->toggle();
			bool range = event->modifiers() & Qt::ShiftModifier;
			emit this->toggled(_id, this->isChecked(), range);
			emit this->toggled(_id.toString(), this->isChecked(), range);
			emit this->toggled(_id.toInt(), this->isChecked(), range);
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
