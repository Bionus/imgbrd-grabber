#ifndef HEADER_QAFFICHE
#define HEADER_QAFFICHE

#include <string>
#include <QtGui>



class QAffiche : public QLabel
{
	Q_OBJECT
	
	public:
		QAffiche(int id = -1, QWidget *parent = 0);
		~QAffiche();
		void setImage(QImage);
		void setImage(QPixmap);
	
	signals:
		void doubleClicked();
		void doubleClicked(int);
		void clicked();
		void clicked(int);
		void pressed();
		void pressed(int);
		void released();
		void released(int);
		void mouseOver();
		void mouseOver(int);
		void mouseOut();
		void mouseOut(int);
		
	protected:
		void mouseDoubleClickEvent ( QMouseEvent * e );
		void mousePressEvent( QMouseEvent *e);
		void mouseReleaseEvent( QMouseEvent *e);
		void enterEvent(QEvent *);
		void leaveEvent(QEvent *);
		bool hitLabel(const QPoint &p);
		bool m_pressed;
		int m_id;
};

#endif
