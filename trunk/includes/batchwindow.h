#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QtGui>



namespace Ui
{
    class batchWindow;
}



class batchWindow : public QDialog
{
    Q_OBJECT

	public:
		explicit batchWindow(QWidget *parent = 0);
		~batchWindow();
		int value();
		int maximum();
		int images();
		int endAction();

	public slots:
		void clear();
		void setText(QString);
		void setValue(int);
		void setLittleValue(int);
		void setMaximum(int);
		void setImages(int);
		void setImagesCount(int);
		void addImage(QString, int batch, int size);
		void loadingImage(QString);
		void statusImage(QString, int);
		void speedImage(QString, int);
		void loadedImage(QString);
		void errorImage(QString);
		void on_buttonDetails_clicked();
		void closeEvent(QCloseEvent *);
		void copyToClipboard();
		void setSpeed(int);
		void drawSpeed();

	signals:
		void closed();

	private:
		Ui::batchWindow			*ui;
		QSize					m_currentSize;
		int						m_imagesCount, m_items, m_value, m_images, m_currentSpeed;
		QList<QProgressBar*>	m_progressBars;
		QMap<QString, int>		m_speeds;
};

#endif // BATCHWINDOW_H
