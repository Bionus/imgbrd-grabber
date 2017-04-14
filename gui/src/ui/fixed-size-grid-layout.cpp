#include "fixed-size-grid-layout.h"
#include "math.h"


FixedSizeGridLayout::FixedSizeGridLayout(QWidget *parent)
	: QGridLayout(parent), m_fixedWidth(0), m_oldWidth(0)
{
}

void FixedSizeGridLayout::setFixedWidth(int width)
{
	m_fixedWidth = width;
}

bool FixedSizeGridLayout::shouldRedoLayout(const QRect &rect)
{
	if (m_oldWidth == 0)
		return true;

	int oldImagesPerLine = getImagesPerLine(m_oldWidth, 0);
	int newImagesPerLine = getImagesPerLine(rect.width(), 0);

	return newImagesPerLine != oldImagesPerLine;
}

void FixedSizeGridLayout::setGeometry(const QRect &rect)
{
	QGridLayout::setGeometry(rect);

	if (shouldRedoLayout(rect))
		redoLayout(rect.width());

	m_oldWidth = rect.width();
}

int FixedSizeGridLayout::getImagesPerLine(int width, int imagesPerPage) const
{
	int result;

	if (m_fixedWidth > 0)
	{
		int marginLeft, marginTop, marginRight, marginBottom;
		getContentsMargins(&marginLeft, &marginTop, &marginRight, &marginBottom);

		int betweenImages = horizontalSpacing();
		int aroundLayout = marginLeft + marginRight;
		result = floor((width + betweenImages - aroundLayout) / (m_fixedWidth + betweenImages));
	}
	else
	{ result = ceil(sqrt((double)imagesPerPage)); }

	// There must always be at least one image per line
	if (result < 1)
		return 1;

	return result;
}

QPoint FixedSizeGridLayout::getThumbPosition(int width, int relativePosition, int imagesPerPage) const
{
	int imagesPerLine = getImagesPerLine(width, imagesPerPage);

	int row = floor(float(relativePosition % imagesPerPage) / imagesPerLine);
	int column = relativePosition % imagesPerLine;

	return QPoint(column, row);
}

void FixedSizeGridLayout::redoLayout(int width)
{
	QList<QWidget*> children;
	QLayoutItem *child;
	while ((child = takeAt(0)) != 0)
	{
		QWidget *widget = child->widget();
		widget->hide();

		removeItem(child);
		children.append(widget);
	}

	for (int i = 0; i < children.count(); ++i)
	{
		QWidget *widget = children[i];
		widget->show();

		QPoint pos = getThumbPosition(width, i, children.count());
		addWidget(widget, pos.y(), pos.x());
	}
}

void FixedSizeGridLayout::addFixedSizeWidget(QWidget *widget, int position, int imagesPerPage)
{
	QPoint pos = getThumbPosition(m_oldWidth, position, imagesPerPage);
	addWidget(widget, pos.y(), pos.x());
}
