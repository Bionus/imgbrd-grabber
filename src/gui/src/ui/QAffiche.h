#ifndef HEADER_QAFFICHE
#define HEADER_QAFFICHE

#include <QLabel>
#include <QVariant>


class QAffiche : public QLabel
{
	Q_OBJECT

	public:
		explicit QAffiche(const QVariant &id = QVariant(), int border = 0, QColor color = QColor(), QWidget *parent = nullptr);
		void setImage(const QImage &);
		void setImage(const QPixmap &);
		Qt::MouseButton lastPressed();

	signals:
		void doubleClicked();
		void doubleClicked(int);
		void clicked();
		void clicked(int);
		void clicked(const QString &);
		void rightClicked();
		void rightClicked(int);
		void rightClicked(const QString &);
		void middleClicked();
		void middleClicked(int);
		void middleClicked(const QString &);
		void pressed();
		void pressed(int);
		void released();
		void released(int);
		void mouseOver();
		void mouseOver(int);
		void mouseOver(const QString &);
		void mouseOut();
		void mouseOut(int);
		void mouseOut(const QString &);

	protected:
		// void paintEvent(QPaintEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;
		void enterEvent(QEnterEvent *event) override;
		void leaveEvent(QEvent *event) override;
		void resizeEvent(QResizeEvent *event) override;
		bool hitLabel(QPoint point);
		bool m_pressed;
		QVariant m_id;
		int m_border;
		QColor m_color;
		Qt::MouseButton m_lastPressed;
};

#endif
