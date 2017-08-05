#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <QPushButton>
#include <QVariant>


class QBouton : public QPushButton
{
	Q_OBJECT

	public:
		QBouton(QVariant id = 0, bool resizeInsteadOfCropping = false, bool smartSizeHint = false, int border = 0, QColor color = QColor(), QWidget *parent = Q_NULLPTR);
		QVariant id();
		void mousePressEvent(QMouseEvent *event);
		virtual QSize sizeHint() const;
		virtual void resizeEvent(QResizeEvent *event);

	private:
		QSize getIconSize(int regionWidth, int regionHeight, bool wOnly = false) const;

	public slots:
		void setId(QVariant);
		void setProgress(qint64 current, qint64 max);
		void scale(const QPixmap &image, float scale);
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
		void toggled(QVariant id, bool toggle, bool range);
		void toggled(int id, bool toggle, bool range);
		void toggled(QString id, bool toggle, bool range);

	private:
		QVariant	_id;
		bool		_resizeInsteadOfCropping, _smartSizeHint;
		QColor		_penColor;
		int			_border;
		qint64		_progress;
		qint64		_progressMax;
};

#endif
