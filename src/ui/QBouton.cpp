#include <QPaintEvent>
#include <QPainter>
#include "QBouton.h"
#include <QDebug>


QBouton::QBouton(QVariant id, bool resizeInsteadOfCropping, bool smartSizeHint, int border, QColor color, QWidget *parent)
	: QPushButton(parent), _id(id), _resizeInsteadOfCropping(resizeInsteadOfCropping), _smartSizeHint(smartSizeHint), _np(false), _originalSize(QSize(-1,-1)), _penColor(color), _border(border)
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

	QPainter painter(this);
	QRect region = _smartSizeHint ? contentsRect() : event->rect();
	QSize iconSize = getIconSize(region.width(), region.height());
	int p = _border;
	int x = region.x();
	int y = region.y();
	int w = iconSize.width();
	int h = iconSize.height();

	// Ignore invalid images
	if (w == 0 || h == 0)
		return;

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

QSize QBouton::getIconSize(int regionWidth, int regionHeight, bool wOnly) const
{
	int w = iconSize().width();
	int h = iconSize().height();

	if (wOnly && w <= regionWidth)
		return iconSize();

	// Calculate ratio to resize by keeping proportions
	if (_resizeInsteadOfCropping)
	{
		float coef = wOnly
					 ? qMin(1.0f, float(regionWidth) / float(w))
					 : qMin(1.0f, qMin(float(regionWidth) / float(w), float(regionHeight) / float(h)));
		w *= coef;
		h *= coef;
	}

	return QSize(w, h);
}

void QBouton::resizeEvent(QResizeEvent *event)
{
	QPushButton::resizeEvent(event);

	if (_smartSizeHint)
		updateGeometry();
}

QSize QBouton::sizeHint() const
{
	// Used for normal buttons
	if (!_smartSizeHint || (!_resizeInsteadOfCropping && _border == 0))
		return QPushButton::sizeHint();

	QSize current = size();
	return getIconSize(current.width(), current.height(), true);
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
