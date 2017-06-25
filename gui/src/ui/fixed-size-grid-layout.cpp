#include "fixed-size-grid-layout.h"
#include "math.h"


FixedSizeGridLayout::FixedSizeGridLayout(QWidget *parent)
	: QGridLayout(parent), m_fixedWidth(0), m_width(0), m_oldImagesPerLine(0), m_imagesPerPage(0), m_frozen(false)
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

	if (!m_frozen && shouldRedoLayout(rect.width()))
	{
		m_frozen = true;
		redoLayout(rect.width());
		m_frozen = false;
	}

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

#include <QDebug>
void FixedSizeGridLayout::redoLayout(int width)
{
	QList<QWidget*> children;
	QLayoutItem *child;
	while ((child = takeAt(0)) != 0)
	{
		QWidget *widget = child->widget();
		if (widget != nullptr)
		{
			widget->hide();
			if (widget->minimumHeight() == 1)
				widget->deleteLater();
			else
				children.append(widget);
		}
		removeItem(child);
	}

	if (children.isEmpty())
		return;

	int imagesPerPage = m_imagesPerPage == 0 ? children.count() : m_imagesPerPage;
	fillLayout(children.count(), width, imagesPerPage);
	for (int i = 0; i < children.count(); ++i)
	{
		QWidget *widget = children[i];
		widget->show();

		QPoint pos = getThumbPosition(width, i, imagesPerPage);
		addWidget(widget, pos.y(), pos.x());
	}
}

void FixedSizeGridLayout::addFixedSizeWidget(QWidget *widget, int position, int imagesPerPage)
{
	m_frozen = true;

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

	fillLayout(imagesPerPage, m_width, imagesPerPage);

	m_frozen = false;
}

void FixedSizeGridLayout::fillLayout(int count, int width, int imagesPerPage)
{
	int imagesPerLine = getImagesPerLine(width, imagesPerPage);
	if (count < imagesPerLine)
	{
		for (int i = count; i < imagesPerLine; ++i)
		{
			auto l = new QWidget();
			l->setFixedWidth(m_fixedWidth);
			l->setMinimumHeight(1);
			addWidget(l, 0, i);
		}
	}
}
