#ifndef HEADER_QAFFICHE
#define HEADER_QAFFICHE

#include <string>
#include <QtGui>



class QAffiche : public QLabel
{
	Q_OBJECT
	
	public:
		QAffiche(QWidget *parent = 0);
		~QAffiche();
		void setImage(QImage);
		void setImage(QPixmap);
	
	signals:
		void doubleClicked();
		void clicked();
		void pressed();
		void released();
		void mouseOver();
		void mouseOut();
		
	protected:
		void mouseDoubleClickEvent ( QMouseEvent * e );
		void mousePressEvent( QMouseEvent *e);
		void mouseReleaseEvent( QMouseEvent *e);
		void enterEvent(QEvent *);
		void leaveEvent(QEvent *);
		bool hitLabel(const QPoint &p);
		bool m_pressed;
};

#endif
