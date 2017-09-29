#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <QPushButton>
#include <QVariant>


class QBouton : public QPushButton
{
	Q_OBJECT

	public:
		explicit QBouton(QVariant id = 0, bool resizeInsteadOfCropping = false, bool smartSizeHint = false, int border = 0, QColor color = QColor(), QWidget *parent = Q_NULLPTR);
		QVariant id();
		void mousePressEvent(QMouseEvent *event) override;
		QSize sizeHint() const override;
		void resizeEvent(QResizeEvent *event) override;

	private:
		QSize getIconSize(int regionWidth, int regionHeight, bool wOnly = false) const;

	public slots:
		void setId(QVariant);
		void setProgress(qint64 current, qint64 max);
		void scale(const QPixmap &image, float scale);
		void paintEvent(QPaintEvent *event) override;

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
		QVariant m_id;
		bool m_resizeInsteadOfCropping;
		bool m_smartSizeHint;
		QColor m_penColor;
		int m_border;
		bool m_center;
		qint64 m_progress;
		qint64 m_progressMax;
};

#endif
