#ifndef UPDATE_DIALOG_H
#define UPDATE_DIALOG_H

#include <QDialog>
#include <QNetworkReply>
#include <QWidget>
#include "updater/program-updater.h"


namespace Ui
{
	class UpdateDialog;
}

class UpdateDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit UpdateDialog(QWidget *parent = Q_NULLPTR);
		~UpdateDialog();

	signals:
		void noUpdateAvailable();

	public slots:
		void accept();
		void checkForUpdates();
		void downloadUpdate();
		void resizeToFit();

	private slots:
		void checkForUpdatesDone(QString newVersion, bool available, QString changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished(QString path);

	private:
		Ui::UpdateDialog	*ui;
		QWidget				*m_parent;
		ProgramUpdater		m_updater;
};

#endif // UPDATE_DIALOG_H
