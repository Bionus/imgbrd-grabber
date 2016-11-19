#ifndef UPDATE_WINDOW_H
#define UPDATE_WINDOW_H

#include <QDialog>
#include <QProgressDialog>
#include <QNetworkReply>
#include "program-updater.h"


namespace Ui
{
	class UpdateWindow;
}

class UpdateWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit UpdateWindow(QWidget *parent);
		~UpdateWindow();

	public slots:
		void checkForUpdates();
		void downloadUpdate();

	private slots:
		void checkForUpdatesDone(QString newVersion, bool available, QString changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished();

	private:
		Ui::UpdateWindow	*ui;
		QWidget				*m_parent;
		ProgramUpdater		m_updater;
		QNetworkReply		*m_updaterReply;
		QProgressDialog		*m_updaterProgress;
};

#endif // UPDATE_WINDOW_H
