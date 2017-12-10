#ifndef HEADER_QAFFICHE
#define HEADER_QAFFICHE

#include <QLabel>
#include <QVariant>


class QAffiche : public QLabel
{
	Q_OBJECT

	public:
		explicit QAffiche(const QVariant &id = QVariant(), int border = 0, QColor color = QColor(), QWidget *parent = Q_NULLPTR);
		void setImage(const QImage &);
		void setImage(const QPixmap &);
		Qt::MouseButton lastPressed();

	signals:
		void doubleClicked();
		void doubleClicked(int);
		void clicked();
		void clicked(int);
		void clicked(const QString &);
		void middleClicked();
		void middleClicked(int);
		void middleClicked(const QString &);
		void pressed();
		void pressed(int);
		void released();
		void released(int);
		void mouseOver();
		void mouseOver(int);
		void mouseOut();
		void mouseOut(int);

	protected:
		//void paintEvent(QPaintEvent*);
		void mouseDoubleClickEvent(QMouseEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void mouseReleaseEvent(QMouseEvent*) override;
		void enterEvent(QEvent*) override;
		void leaveEvent(QEvent*) override;
		bool hitLabel(QPoint p);
		bool m_pressed;
		QVariant m_id;
		int m_border;
		QColor m_color;
		Qt::MouseButton m_lastPressed;
};

#endif
