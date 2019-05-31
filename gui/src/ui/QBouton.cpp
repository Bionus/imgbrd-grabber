#include "ui/QBouton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QtMath>


QBouton::QBouton(QVariant id, bool resizeInsteadOfCropping, bool smartSizeHint, int border, QColor color, QWidget *parent)
	: QPushButton(parent), m_id(std::move(id)), m_resizeInsteadOfCropping(resizeInsteadOfCropping), m_smartSizeHint(smartSizeHint), m_penColor(std::move(color)), m_border(border), m_center(true), m_progress(0), m_progressMax(0), m_invertToggle(false), m_counter(QString())
{}

void QBouton::scale(const QPixmap &image, qreal scale)
{
	QSize size;
	if (scale - 1.0 > 0.001) {
		size = image.size() * scale;
		setIcon(image.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	} else {
		setIcon(image);
		size = image.size();
	}
	setIconSize(size);
	resize(size);
}

QVariant QBouton::id() const
{ return m_id; }
void QBouton::setId(const QVariant &id)
{ m_id = id; }

void QBouton::setProgress(qint64 current, qint64 max)
{
	m_progress = current;
	m_progressMax = max;

	repaint();
}

void QBouton::setInvertToggle(bool invertToggle)
{ m_invertToggle = invertToggle; }
void QBouton::setCounter(const QString &counter)
{ m_counter = counter; }

void QBouton::paintEvent(QPaintEvent *event)
{
	// Used for normal buttons
	if (!m_resizeInsteadOfCropping && m_border == 0 && m_progressMax == 0 && m_counter.isEmpty()) {
		QPushButton::paintEvent(event);
		return;
	}

	QPainter painter(this);
	const QRect region = m_smartSizeHint ? contentsRect() : event->rect();
	const QSize iconSize = getIconSize(region.width(), region.height());
	const int p = m_border;
	int x = region.x();
	int y = region.y();
	int w = iconSize.width() + 2 * p;
	int h = iconSize.height() + 2 * p;

	// Ignore invalid images
	if (w == 0 || h == 0) {
		return;
	}

	// Center the image
	if (m_center) {
		x += (region.width() - w) / 2;
		y += (region.height() - h) / 2;
	}

	// Draw image
	const QIcon::Mode mode = this->isChecked() ? QIcon::Selected : QIcon::Normal;
	if (w > h) {
		icon().paint(&painter, x + p, y + p, w - 2 * p, w - 2 * p, Qt::AlignLeft | Qt::AlignTop, mode);
		h = h - ((h * 2 * p) / w) + 2 * p - 1;
	} else {
		icon().paint(&painter, x + p, y + p, h - 2 * p, h - 2 * p, Qt::AlignLeft | Qt::AlignTop, mode);
		w = w - ((w * 2 * p) / h) + 2 * p - 1;
	}

	// Clip borders overflows
	painter.setClipRect(x, y, w, h);

	// Draw borders
	if (p > 0 && m_penColor.isValid()) {
		QPen pen(m_penColor);
		pen.setWidth(p * 2);
		painter.setPen(pen);
		painter.drawRect(qMax(x, 0), qMax(y, 0), qMin(w, size().width()), qMin(h, size().height()));
	}

	// Draw progress
	if (m_progressMax > 0 && m_progress > 0 && m_progress < m_progressMax) {
		const int lineHeight = 6;
		const int a = p + lineHeight / 2;

		const qreal ratio = static_cast<qreal>(m_progress) / m_progressMax;
		QPoint p1(qMax(x, 0) + a, qMax(y, 0) + a);
		QPoint p2(qFloor(p1.x() + (iconSize.width() - a) * ratio), p1.y());

		if (p2.x() > p1.x()) {
			QPen pen(QColor(0, 200, 0));
			pen.setWidth(lineHeight);
			painter.setPen(pen);
			painter.drawLine(p1, p2);
		}
	}

	// Draw counter
	if (!m_counter.isEmpty()) {
		const int right = qMax(x, 0) + qMin(w, size().width());
		const int dim = 10 + 5 * m_counter.length();
		const double pad = 2.5;
		const QRectF notificationRect(right - dim - pad, qMax(y, 0) + pad, dim, 20);
		const int radius = qFloor(qMin(dim, 20) / 2.0);

		painter.setRenderHint(QPainter::Antialiasing);

		QPainterPath path;
		path.addRoundedRect(notificationRect, radius, radius);

		const QPen pen(Qt::black, 1);
		painter.setPen(pen);
		painter.fillPath(path, QColor(255, 0, 0));
		painter.drawPath(path);

		painter.setPen(QPen(Qt::white));
		painter.drawText(notificationRect, Qt::AlignCenter, m_counter);
	}
}

QSize QBouton::getIconSize(int regionWidth, int regionHeight, bool wOnly) const
{
	int w = iconSize().width();
	int h = iconSize().height();

	if (wOnly && w <= regionWidth) {
		return iconSize();
	}

	// Calculate ratio to resize by keeping proportions
	if (m_resizeInsteadOfCropping) {
		const qreal coefficient = wOnly
			? qMin(1.0, static_cast<qreal>(regionWidth) / static_cast<qreal>(w))
			: qMin(1.0, qMin(static_cast<qreal>(regionWidth) / static_cast<qreal>(w), static_cast<qreal>(regionHeight) / static_cast<qreal>(h)));
		w *= coefficient;
		h *= coefficient;
	}

	return { w, h };
}

void QBouton::resizeEvent(QResizeEvent *event)
{
	QPushButton::resizeEvent(event);

	if (m_smartSizeHint) {
		updateGeometry();
	}
}

QSize QBouton::sizeHint() const
{
	// Used for normal buttons
	if (!m_smartSizeHint || (!m_resizeInsteadOfCropping && m_border == 0)) {
		return QPushButton::sizeHint();
	}

	QSize current = size();
	return getIconSize(current.width(), current.height(), true);
}

void QBouton::mousePressEvent(QMouseEvent *event)
{
	// Ignore clicks outside the thumbnail
	const QSize imgSize = sizeHint();
	const QSize size = this->size();
	const int wMargin = (size.width() - imgSize.width()) / 2;
	const int hMargin = (size.height() - imgSize.height()) / 2;
	const QPoint pos = event->pos();
	if (pos.x() < wMargin
			|| pos.y() < hMargin
			|| pos.x() > imgSize.width() + wMargin
			|| pos.y() > imgSize.height() + hMargin) {
		event->ignore();
		return;
	}

	if (event->button() == Qt::LeftButton) {
		const bool ctrlPressed = event->modifiers().testFlag(Qt::ControlModifier);
		if (ctrlPressed != m_invertToggle) {
			this->toggle();
			const bool range = event->modifiers().testFlag(Qt::ShiftModifier);
			emit this->toggled(m_id, this->isChecked(), range);
			emit this->toggled(m_id.toString(), this->isChecked(), range);
			emit this->toggled(m_id.toInt(), this->isChecked(), range);
		} else {
			emit this->appui(m_id);
			emit this->appui(m_id.toString());
			emit this->appui(m_id.toInt());
		}
	} else if (event->button() == Qt::RightButton) {
		emit this->rightClick(m_id);
		emit this->rightClick(m_id.toString());
		emit this->rightClick(m_id.toInt());
	} else if (event->button() == Qt::MidButton) {
		emit this->middleClick(m_id);
		emit this->middleClick(m_id.toString());
		emit this->middleClick(m_id.toInt());
	} else {
		event->ignore();
		return;
	}

	event->accept();
}
