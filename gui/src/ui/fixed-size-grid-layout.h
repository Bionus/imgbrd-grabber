#ifndef FIXED_SIZE_GRID_LAYOUT_H
#define FIXED_SIZE_GRID_LAYOUT_H

#include <QGridLayout>
#include <QWidget>
#include <QRect>
#include <QPoint>


class FixedSizeGridLayout : public QGridLayout
{
	Q_OBJECT

	public:
		explicit FixedSizeGridLayout(QWidget *parent = Q_NULLPTR);
		void setFixedWidth(int width);
		void redoLayout(int width);
		void addFixedSizeWidget(QWidget *widget, int position, int imagesPerPage);
		void setGeometry(const QRect &rect);

	protected:
		bool shouldRedoLayout(const QRect &rect);
		int getImagesPerLine(int width, int imagesPerPage) const;
		QPoint getThumbPosition(int width, int relativePosition, int imagesPerPage) const;

	private:
		int m_fixedWidth;
		int m_oldWidth;
};

#endif // FIXED_SIZE_GRID_LAYOUT_H
