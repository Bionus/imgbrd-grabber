#include "fixed-size-grid-layout.h"
#include "math.h"


FixedSizeGridLayout::FixedSizeGridLayout(QWidget *parent)
	: QGridLayout(parent), m_fixedWidth(0), m_width(0), m_oldImagesPerLine(0), m_imagesPerPage(0)
{
}

void FixedSizeGridLayout::setFixedWidth(int width)
{
	m_fixedWidth = width;
}

bool FixedSizeGridLayout::shouldRedoLayout(int width)
{
	if (m_oldImagesPerLine == 0)
		return true;

	int newImagesPerLine = getImagesPerLine(width, m_imagesPerPage);
	return newImagesPerLine != m_oldImagesPerLine;
}

void FixedSizeGridLayout::setGeometry(const QRect &rect)
{
	QGridLayout::setGeometry(rect);

	if (shouldRedoLayout(rect.width()))
		redoLayout(rect.width());

	m_oldImagesPerLine = getImagesPerLine(rect.width(), m_imagesPerPage);
	m_width = rect.width();
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

		int imagesPerPage = m_imagesPerPage == 0 ? children.count() : m_imagesPerPage;
		QPoint pos = getThumbPosition(width, i, imagesPerPage);
		addWidget(widget, pos.y(), pos.x());
	}
}

void FixedSizeGridLayout::addFixedSizeWidget(QWidget *widget, int position, int imagesPerPage)
{
	// Redo layout if necessary when the total number of images changes in merged results mode
	if (imagesPerPage != m_imagesPerPage)
	{
		m_imagesPerPage = imagesPerPage;
		if (shouldRedoLayout(m_width))
			redoLayout(m_width);
		m_oldImagesPerLine = getImagesPerLine(m_width, m_imagesPerPage);
	}

	QPoint pos = getThumbPosition(m_width, position, imagesPerPage);
	addWidget(widget, pos.y(), pos.x());
}
