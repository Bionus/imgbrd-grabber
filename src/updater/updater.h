#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QNetworkAccessManager>


class Updater : public QObject
{
	Q_OBJECT

	public:
		int compareVersions(QString a, QString b);

	public slots:
		virtual void checkForUpdates() = 0;

	protected:
		QNetworkAccessManager m_networkAccessManager;
};

#endif // UPDATER_H
