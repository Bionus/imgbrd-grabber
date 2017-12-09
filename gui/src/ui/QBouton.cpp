#include "QBouton.h"
#include <QPainter>
#include <QPaintEvent>
#include <cmath>


QBouton::QBouton(QVariant id, bool resizeInsteadOfCropping, bool smartSizeHint, int border, QColor color, QWidget *parent)
	: QPushButton(parent), m_id(id), m_resizeInsteadOfCropping(resizeInsteadOfCropping), m_smartSizeHint(smartSizeHint), m_penColor(color), m_border(border), m_center(true), m_progress(0), m_progressMax(0)
{ }

void QBouton::scale(const QPixmap &image, float scale)
{
	QSize size;
	if (fabs(scale - 1.0f) < 0.001f)
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
{ return m_id; }
void QBouton::setId(const QVariant &id)
{ m_id = id; }

void QBouton::setProgress(qint64 current, qint64 max)
{
	m_progress = current;
	m_progressMax = max;

	repaint();
}

void QBouton::paintEvent(QPaintEvent *event)
{
	// Used for normal buttons
	if (!m_resizeInsteadOfCropping && m_border == 0 && m_progressMax == 0)
	{
		QPushButton::paintEvent(event);
		return;
	}

	QPainter painter(this);
	QRect region = m_smartSizeHint ? contentsRect() : event->rect();
	QSize iconSize = getIconSize(region.width(), region.height());
	int p = m_border;
	int x = region.x();
	int y = region.y();
	int w = iconSize.width() + 2*p;
	int h = iconSize.height() + 2*p;

	// Ignore invalid images
	if (w == 0 || h == 0)
		return;

	// Center the image
	if (m_center)
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

	// Draw progress
	if (m_progressMax > 0 && m_progress > 0 && m_progress != m_progressMax)
	{
		int lineHeight = 6;
		int a = p + lineHeight/2;

		float ratio = (float)m_progress / m_progressMax;
		QPoint p1(qMax(x, 0) + a, qMax(y, 0) + a);
		QPoint p2(p1.x() + (iconSize.width() - a) * ratio, p1.y());

		if (p2.x() > p1.x())
		{
			QPen pen(QColor(0, 200, 0));
			pen.setWidth(lineHeight);
			painter.setPen(pen);
			painter.drawLine(p1, p2);
		}
	}

	// Draw borders
	if (p > 0 && m_penColor.isValid())
	{
		QPen pen(m_penColor);
		pen.setWidth(p*2);
		painter.setPen(pen);
		painter.drawRect(qMax(x, 0), qMax(y, 0), qMin(w, size().width()), qMin(h, size().height()));
	}
}

QSize QBouton::getIconSize(int regionWidth, int regionHeight, bool wOnly) const
{
	int w = iconSize().width();
	int h = iconSize().height();

	if (wOnly && w <= regionWidth)
		return iconSize();

	// Calculate ratio to resize by keeping proportions
	if (m_resizeInsteadOfCropping)
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

	if (m_smartSizeHint)
		updateGeometry();
}

QSize QBouton::sizeHint() const
{
	// Used for normal buttons
	if (!m_smartSizeHint || (!m_resizeInsteadOfCropping && m_border == 0))
		return QPushButton::sizeHint();

	QSize current = size();
	return getIconSize(current.width(), current.height(), true);
}

void QBouton::mousePressEvent(QMouseEvent *event)
{
	// Ignore clicks outside the thumbnail
	QSize imgSize = sizeHint();
	QSize size = this->size();
	int wMargin = (size.width() - imgSize.width()) / 2;
	int hMargin = (size.height() - imgSize.height()) / 2;
	QPoint pos = event->pos();
	if (pos.x() < wMargin
			|| pos.y() < hMargin
			|| pos.x() > imgSize.width() + wMargin
			|| pos.y() > imgSize.height() + hMargin)
		return;

	if (event->button() == Qt::LeftButton)
	{
		if (event->modifiers() & Qt::ControlModifier)
		{
			this->toggle();
			bool range = event->modifiers() & Qt::ShiftModifier;
			emit this->toggled(m_id, this->isChecked(), range);
			emit this->toggled(m_id.toString(), this->isChecked(), range);
			emit this->toggled(m_id.toInt(), this->isChecked(), range);
		}
		else
		{
			emit this->appui(m_id);
			emit this->appui(m_id.toString());
			emit this->appui(m_id.toInt());
		}
	}
	if (event->button() == Qt::RightButton)
	{
		emit this->rightClick(m_id);
		emit this->rightClick(m_id.toString());
		emit this->rightClick(m_id.toInt());
	}
	if (event->button() == Qt::MidButton)
	{
		emit this->middleClick(m_id);
		emit this->middleClick(m_id.toString());
		emit this->middleClick(m_id.toInt());
	}
	event->accept();
}
