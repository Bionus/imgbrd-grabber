#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <string>
#include <QtGui>
#include <QtNetwork>

using namespace std;



class QBouton : public QPushButton
{
	Q_OBJECT
 
	public:
		QBouton(QVariant id = 0, bool resizeInsteadOfCropping = false, int border = 0, QColor color = QColor(), QWidget *parent = 0);
		QVariant id();
		void mousePressEvent(QMouseEvent *);

	public slots:
		void setId(QVariant);
		void paintEvent(QPaintEvent *event);
		
	signals:
		void appui(QVariant);
		void rightClick(QVariant);
		void middleClick(QVariant);
		void appui(int);
		void rightClick(int);
		void middleClick(int);
		void appui(QString);
		void rightClick(QString);
		void middleClick(QString);

	private:
		QVariant	_id;
		bool		_resizeInsteadOfCropping, _np;
		QSize		_originalSize;
		QColor		_penColor;
		int			_border;
};

#endif
