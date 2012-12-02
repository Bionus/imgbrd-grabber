#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QDialog>
#include <QProgressBar>
#include <QMap>



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
		int count();
		int endAction();
		bool endRemove();
		bool cancelled();
		int batch(QString url);
		void setCount(int);
		void updateColumns();

	public slots:
		void clear();
		void setText(QString);
		void setValue(int);
		void setLittleValue(int);
		void setMaximum(int);
		void setImages(int);
		void setImagesCount(int);
		void addImage(QString, int, float);
		void sizeImage(QString, float);
		void loadingImage(QString);
		void statusImage(QString, int);
		void speedImage(QString, float);
		void loadedImage(QString);
		void errorImage(QString);
		void on_buttonDetails_clicked(bool visible);
		void closeEvent(QCloseEvent *);
		void copyToClipboard();
		void cancel();
		void drawSpeed();
		void imageUrlChanged(QString, QString);

	signals:
		void closed();

	private:
		Ui::batchWindow			*ui;
		QSize					m_currentSize;
		int						m_imagesCount, m_items, m_value, m_images, m_maxSpeeds;
		QStringList				m_urls;
		QList<QProgressBar*>	m_progressBars;
		QMap<QString, int>		m_speeds;
		QList<int>				m_mean;
		bool					m_cancel;
		QTime					*m_time, *m_start;
};

#endif // BATCHWINDOW_H
