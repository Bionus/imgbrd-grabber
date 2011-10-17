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

	public slots:
		void clear();
		void setText(QString);
		void setValue(int);
		void setMaximum(int);
		void setImages(int);
		void setImagesCount(int);
		void addImage(QString);
		void loadingImage(QString);
		void loadedImage(QString);
		void errorImage(QString);
		void on_buttonDetails_clicked();
		void closeEvent(QCloseEvent *);
		void copyToClipboard();

	signals:
		void closed();

	private:
		Ui::batchWindow *ui;
		QSize m_currentSize;
		int m_imagesCount, m_items;
};

#endif // BATCHWINDOW_H
