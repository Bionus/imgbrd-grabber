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
		explicit UpdateDialog(bool *shouldQuit, QWidget *parent = nullptr);
		~UpdateDialog() override;

	signals:
		void noUpdateAvailable();

	public slots:
		void accept() override;
		void checkForUpdates();
		void downloadUpdate();
		void resizeToFit();

	private slots:
		void checkForUpdatesDone(const QString &newVersion, bool available, const QString &changelog);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadFinished(const QString &path);

	private:
		Ui::UpdateDialog *ui;
		bool *m_shouldQuit;
		QWidget *m_parent;
		ProgramUpdater m_updater;
};

#endif // UPDATE_DIALOG_H
