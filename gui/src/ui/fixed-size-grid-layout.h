#ifndef FIXED_SIZE_GRID_LAYOUT_H
#define FIXED_SIZE_GRID_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>


class FixedSizeGridLayout : public QLayout
{
	public:
		explicit FixedSizeGridLayout(QWidget *parent, int hSpacing = -1, int vSpacing = -1);
		explicit FixedSizeGridLayout(int hSpacing = -1, int vSpacing = -1);
		~FixedSizeGridLayout();

		void setFixedWidth(int width);

		void addItem(QLayoutItem *item) override;
		void insertItem(int index, QLayoutItem *item);
		void insertWidget(int index, QWidget *widget);
		QLayoutItem *itemAt(int index) const override;
		QLayoutItem *takeAt(int index) override;
		int count() const override;

		int horizontalSpacing() const;
		int verticalSpacing() const;
		Qt::Orientations expandingDirections() const override;
		bool hasHeightForWidth() const override;
		int heightForWidth(int) const override;
		QSize minimumSize() const override;
		QSize sizeHint() const override;
		void setGeometry(const QRect &rect) override;

	private:
		int doLayout(const QRect &rect, bool testOnly) const;
		int smartSpacing(QStyle::PixelMetric pm) const;
		int widgetSpacing(int spacing, QWidget *widget, Qt::Orientation orientation) const;

		int m_hSpace;
		int m_vSpace;
		int m_fixedWidth;
		QList<QLayoutItem*> m_items;
};

#endif // FIXED_SIZE_GRID_LAYOUT_H
