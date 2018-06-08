#ifndef HEADER_QBOUTON
#define HEADER_QBOUTON

#include <QPushButton>
#include <QVariant>


class QBouton : public QPushButton
{
	Q_OBJECT

	public:
		explicit QBouton(const QVariant &id = 0, bool resizeInsteadOfCropping = false, bool smartSizeHint = false, int border = 0, QColor color = QColor(), QWidget *parent = Q_NULLPTR);
		QVariant id();
		void mousePressEvent(QMouseEvent *event) override;
		QSize sizeHint() const override;
		void resizeEvent(QResizeEvent *event) override;

	private:
		QSize getIconSize(int regionWidth, int regionHeight, bool wOnly = false) const;

	public slots:
		void setId(const QVariant &id);
		void setProgress(qint64 current, qint64 max);
		void setInvertToggle(bool invertToggle);
		void setCounter(const QString &counter);
		void scale(const QPixmap &image, float scale);
		void paintEvent(QPaintEvent *event) override;

	signals:
		void appui(const QVariant &id);
		void appui(int id);
		void appui(const QString &id);
		void rightClick(const QVariant &id);
		void rightClick(int id);
		void rightClick(const QString &id);
		void middleClick(const QVariant &id);
		void middleClick(int id);
		void middleClick(const QString &id);
		void toggled(const QVariant &id, bool toggle, bool range);
		void toggled(int id, bool toggle, bool range);
		void toggled(const QString &id, bool toggle, bool range);

	private:
		QVariant m_id;
		bool m_resizeInsteadOfCropping;
		bool m_smartSizeHint;
		QColor m_penColor;
		int m_border;
		bool m_center;
		qint64 m_progress;
		qint64 m_progressMax;
		bool m_invertToggle;
		QString m_counter;
};

#endif
