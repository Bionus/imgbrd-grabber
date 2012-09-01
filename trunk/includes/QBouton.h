#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <QPushButton>
#include <QVariant>



class QBouton : public QPushButton
{
	Q_OBJECT
 
	public:
		QBouton(QVariant id = 0, bool resizeInsteadOfCropping = false, int border = 0, QColor color = QColor(), QWidget *parent = 0);
		~QBouton();
		QVariant id();
		void mousePressEvent(QMouseEvent *);

	public slots:
		void setId(QVariant);
		void paintEvent(QPaintEvent *event);
		
	signals:
		void appui(QVariant);
		void appui(int);
		void appui(QString);
		void rightClick(QVariant);
		void rightClick(int);
		void rightClick(QString);
		void middleClick(QVariant);
		void middleClick(int);
		void middleClick(QString);
		void toggled(QVariant, bool);
		void toggled(int, bool);
		void toggled(QString, bool);

	private:
		QVariant	_id;
		bool		_resizeInsteadOfCropping, _np, _toggled;
		QSize		_originalSize;
		QColor		_penColor;
		int			_border;
};

#endif
