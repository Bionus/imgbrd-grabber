#ifndef BATCHWINDOW_H
#define BATCHWINDOW_H

#include <QDialog>
#include <QMap>
#include <QProgressBar>
#include <QSettings>
#include "loader/downloadable.h"
#ifdef Q_OS_WIN
	#include <QWinTaskbarButton>
	#include <QWinTaskbarProgress>
#endif



namespace Ui
{
	class batchWindow;
}



class batchWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit batchWindow(QSettings *settings, QWidget *parent = Q_NULLPTR);
		~batchWindow() override;
		int currentValue() const;
		int currentMax() const;
		int totalValue() const;
		int totalMax() const;
		int endAction();
		int indexOf(QString);
		int batch(QString);
		void setCount(int);
		void updateColumns();
		bool endRemove();
		bool cancelled();
		bool isPaused();

	public slots:
		void clear();
		void clearImages();
		void setText(QString);
		void setCurrentValue(int val);
		void setCurrentMax(int max);
		void setTotalValue(int val);
		void setTotalMax(int max);
		void addImage(QString, int, float);
		void sizeImage(QString, float);
		void loadingImage(QString);
		void statusImage(QString, int);
		void speedImage(QString, float);
		void loadedImage(QString url, Downloadable::SaveResult result);
		void on_buttonDetails_clicked(bool visible);
		void closeEvent(QCloseEvent *) override;
		void copyToClipboard();
		void cancel();
		void drawSpeed();
		void imageUrlChanged(QString, QString);
		void pause();
		void skip();

	signals:
		void closed();
		void paused();
		void skipped();

	private:
		Ui::batchWindow			*ui;
		QSettings				*m_settings;
		QSize					m_currentSize;
		int						m_imagesCount, m_items, m_images, m_maxSpeeds, m_lastDownloading;
		QStringList				m_urls;
		QList<QProgressBar*>	m_progressBars;
		QMap<QString, int>		m_speeds;
		QList<int>				m_mean;
		bool					m_cancel, m_paused;
		QTime					*m_time, *m_start;
		#ifdef Q_OS_WIN
			QWinTaskbarButton   *m_taskBarButton;
			QWinTaskbarProgress *m_taskBarProgress;
		#endif
};

#endif // BATCHWINDOW_H
