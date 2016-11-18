#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QNetworkAccessManager>


class Updater : public QObject
{
	Q_OBJECT

	public slots:
		virtual void checkForUpdates() = 0;

	protected:
		int versionToInt(QString version);

	protected:
		QNetworkAccessManager m_networkAccessManager;
};

#endif // UPDATER_H
