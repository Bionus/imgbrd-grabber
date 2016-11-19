#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H

#include <QDialog>
#include <QProgressDialog>
#include <QNetworkReply>
#include <QWidget>
#include "program-updater.h"


namespace Ui
{
	class UpdateDialog;
}

class UpdateDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit UpdateDialog(QWidget *parent);
		~UpdateDialog();

	public slots:
		void checkForUpdates();
		void downloadUpdate();
		void resizeToFit();

	private slots:
		void checkForUpdatesDone(QString newVersion, bool available, QString changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished();

	private:
		Ui::UpdateDialog	*ui;
		QWidget				*m_parent;
		ProgramUpdater		m_updater;
		QNetworkReply		*m_updaterReply;
		QProgressDialog		*m_updaterProgress;
};

#endif // UPDATE_DIALOG_H
