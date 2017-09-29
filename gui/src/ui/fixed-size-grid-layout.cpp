#include "fixed-size-grid-layout.h"
#include <QtWidgets>


FixedSizeGridLayout::FixedSizeGridLayout(QWidget *parent, int hSpacing, int vSpacing)
	: QLayout(parent), m_hSpace(hSpacing), m_vSpace(vSpacing)
{}

FixedSizeGridLayout::FixedSizeGridLayout(int hSpacing, int vSpacing)
	: m_hSpace(hSpacing), m_vSpace(vSpacing)
{}

FixedSizeGridLayout::~FixedSizeGridLayout()
{
	QLayoutItem *item;
	while ((item = takeAt(0)) != 0)
		delete item;
}


void FixedSizeGridLayout::setFixedWidth(int width)
{
	m_fixedWidth = width;
}


void FixedSizeGridLayout::addItem(QLayoutItem *item)
{
	m_items.append(item);
}

void FixedSizeGridLayout::insertItem(int index, QLayoutItem *item)
{
	if (index < 0)
		index = m_items.count();

	m_items.insert(index, item);
}

void FixedSizeGridLayout::insertWidget(int index, QWidget *widget)
{
	addChildWidget(widget);
	insertItem(index, new QWidgetItemV2(widget));
}

int FixedSizeGridLayout::count() const
{
	return m_items.size();
}

QLayoutItem *FixedSizeGridLayout::itemAt(int index) const
{
	return m_items.value(index);
}

QLayoutItem *FixedSizeGridLayout::takeAt(int index)
{
	if (index >= 0 && index < m_items.size())
		return m_items.takeAt(index);

	return Q_NULLPTR;
}


int FixedSizeGridLayout::horizontalSpacing() const
{
	if (m_hSpace >= 0) {
		return m_hSpace;
	} else {
		return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
	}
}

int FixedSizeGridLayout::verticalSpacing() const
{
	if (m_vSpace >= 0) {
		return m_vSpace;
	} else {
		return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
	}
}

Qt::Orientations FixedSizeGridLayout::expandingDirections() const
{
	return Q_NULLPTR;
}

bool FixedSizeGridLayout::hasHeightForWidth() const
{
	return true;
}

int FixedSizeGridLayout::heightForWidth(int width) const
{
	int height = doLayout(QRect(0, 0, width, 0), true);
	return height;
}

QSize FixedSizeGridLayout::minimumSize() const
{
	QSize size;
	QLayoutItem *item;
	foreach (item, m_items)
		size = size.expandedTo(item->minimumSize());

	size += QSize(2 * margin(), 2 * margin());
	return size;
}

QSize FixedSizeGridLayout::sizeHint() const
{
	return minimumSize();
}

void FixedSizeGridLayout::setGeometry(const QRect &rect)
{
	QLayout::setGeometry(rect);
	doLayout(rect, false);
}


int FixedSizeGridLayout::doLayout(const QRect &rect, bool testOnly) const
{
	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
	int x = effectiveRect.x();
	int y = effectiveRect.y();
	int w = effectiveRect.width();

	int lineHeight = 0;
	for (QLayoutItem *item : m_items)
	{
		int spaceX = widgetSpacing(horizontalSpacing(), item->widget(), Qt::Horizontal);
		int spaceY = widgetSpacing(verticalSpacing(), item->widget(), Qt::Vertical);

		int nbElts = qMax(1, (w + spaceX) / (m_fixedWidth + spaceX));
		int totalSpace = w - (m_fixedWidth * nbElts);
		spaceX = qMax(spaceX, totalSpace / qMax(1, nbElts - 1));

		int nextX = x + item->sizeHint().width() + spaceX;
		if (nextX - spaceX - 1 > effectiveRect.right() && lineHeight > 0)
		{
			x = effectiveRect.x();
			y = y + lineHeight + spaceY;
			nextX = x + item->sizeHint().width() + spaceX;
			lineHeight = 0;
		}

		if (!testOnly)
			item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

		x = nextX;
		lineHeight = qMax(lineHeight, item->sizeHint().height());
	}
	return y + lineHeight - rect.y() + bottom;
}

int FixedSizeGridLayout::smartSpacing(QStyle::PixelMetric pm) const
{
	QObject *parent = this->parent();
	if (!parent)
		return -1;

	if (parent->isWidgetType())
	{
		auto *pw = static_cast<QWidget*>(parent);
		return pw->style()->pixelMetric(pm, Q_NULLPTR, pw);
	}

	return static_cast<QLayout*>(parent)->spacing();
}

int FixedSizeGridLayout::widgetSpacing(int spacing, QWidget *widget, Qt::Orientation orientation) const
{
	if (spacing >= 0)
		return spacing;

	QSizePolicy::ControlType controlType = widget->sizePolicy().controlType();
	return widget->style()->layoutSpacing(controlType, controlType, orientation);
}
