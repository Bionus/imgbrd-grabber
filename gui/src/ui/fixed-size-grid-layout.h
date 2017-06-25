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
		bool shouldRedoLayout(int width);
		int getImagesPerLine(int width, int imagesPerPage) const;
		QPoint getThumbPosition(int width, int relativePosition, int imagesPerPage) const;
		void fillLayout(int count, int width, int imagesPerPage);

	private:
		int m_fixedWidth;
		int m_width;
		int m_oldImagesPerLine;
		int m_imagesPerPage;
		bool m_frozen;
};

#endif // FIXED_SIZE_GRID_LAYOUT_H
